#include "PipelineHandlerVK.h"
#include <Utils/DebugHandler.h>
#include <Utils/XXHash64.h>
#include "FormatConverterVK.h"
#include "RenderDeviceVK.h"
#include "ShaderHandlerVK.h"
#include "ImageHandlerVK.h"
#include "SpirvReflect.h"


namespace Renderer
{
    namespace Backend
    {
        PipelineHandlerVK::PipelineHandlerVK()
        {

        }

        PipelineHandlerVK::~PipelineHandlerVK()
        {
            for (auto& pipeline : _graphicsPipelines)
            {
                // TODO: Cleanup
            }
            for (auto& pipeline : _computePipelines)
            {
                // TODO: Cleanup
            }
            _graphicsPipelines.clear();
            _computePipelines.clear();
        }

        GraphicsPipelineID PipelineHandlerVK::CreatePipeline(RenderDeviceVK* device, ShaderHandlerVK* shaderHandler, ImageHandlerVK* imageHandler, const GraphicsPipelineDesc& desc)
        {
            assert(desc.ResourceToImageID != nullptr); // You need to bind this function pointer before creating pipeline, maybe use RenderGraph::InitializePipelineDesc?
            assert(desc.ResourceToDepthImageID != nullptr); // You need to bind this function pointer before creating pipeline, maybe use RenderGraph::InitializePipelineDesc?
            assert(desc.MutableResourceToImageID != nullptr); // You need to bind this function pointer before creating pipeline, maybe use RenderGraph::InitializePipelineDesc?
            assert(desc.MutableResourceToDepthImageID != nullptr); // You need to bind this function pointer before creating pipeline, maybe use RenderGraph::InitializePipelineDesc?

            // Check the cache
            size_t nextID;
            u64 cacheDescHash = CalculateCacheDescHash(desc);
            if (TryFindExistingGPipeline(cacheDescHash, nextID))
            {
                return GraphicsPipelineID(static_cast<gIDType>(nextID));
            }
            nextID = _graphicsPipelines.size();

            // Make sure we haven't exceeded the limit of the GraphicsPipelineID type, if this hits you need to change type of GraphicsPipelineID to something bigger
            assert(nextID < GraphicsPipelineID::MaxValue());

            GraphicsPipeline pipeline;
            pipeline.desc = desc;
            pipeline.cacheDescHash = cacheDescHash;

            // -- Get number of rendertargets --
            u8 numRenderTargets = 0;
            for (int i = 0; i < MAX_RENDER_TARGETS; i++)
            {
                if (desc.renderTargets[i] == RenderPassMutableResource::Invalid())
                    break;

                numRenderTargets++;
            }

            // -- Create Render Pass --
            std::vector<VkAttachmentDescription> colorAttachments(numRenderTargets);
            std::vector< VkAttachmentReference> colorAttachmentRefs(numRenderTargets);
            for (int i = 0; i < numRenderTargets; i++)
            {
                ImageID imageID = desc.MutableResourceToImageID(desc.renderTargets[i]);
                const ImageDesc& imageDesc = imageHandler->GetDescriptor(imageID);
                colorAttachments[i].format = FormatConverterVK::ToVkFormat(imageDesc.format);
                colorAttachments[i].samples = FormatConverterVK::ToVkSampleCount(imageDesc.sampleCount);
                colorAttachments[i].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
                colorAttachments[i].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                colorAttachments[i].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                colorAttachments[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                colorAttachments[i].initialLayout = VK_IMAGE_LAYOUT_GENERAL;
                colorAttachments[i].finalLayout = VK_IMAGE_LAYOUT_GENERAL;

                colorAttachmentRefs[i].attachment = i;
                colorAttachmentRefs[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            }

            VkSubpassDescription subpass = {};
            subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.colorAttachmentCount = numRenderTargets;
            subpass.pColorAttachments = colorAttachmentRefs.data();

            VkSubpassDependency dependency = {};
            dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            dependency.dstSubpass = 0;
            dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency.srcAccessMask = 0;
            dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

            VkRenderPassCreateInfo renderPassInfo = {};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            renderPassInfo.attachmentCount = numRenderTargets;
            renderPassInfo.pAttachments = colorAttachments.data();
            renderPassInfo.subpassCount = 1;
            renderPassInfo.pSubpasses = &subpass;
            renderPassInfo.dependencyCount = 1;
            renderPassInfo.pDependencies = &dependency;

            if (vkCreateRenderPass(device->_device, &renderPassInfo, nullptr, &pipeline.renderPass) != VK_SUCCESS)
            {
                NC_LOG_FATAL("Failed to create render pass!");
            }

            // -- Create Framebuffer --
            u8 numAttachments = numRenderTargets;
            if (desc.depthStencil != RenderPassMutableResource::Invalid())
                numAttachments++;

            std::vector<VkImageView> attachments(numAttachments);
            // Add all color rendertargets as attachments
            for (int i = 0; i < numRenderTargets; i++)
            {
                ImageID imageID = desc.MutableResourceToImageID(desc.renderTargets[i]);
                attachments[i] = imageHandler->GetColorView(imageID);
            }
            // Add depthstencil as attachment
            if (desc.depthStencil != RenderPassMutableResource::Invalid())
            {
                DepthImageID depthImageID = desc.MutableResourceToDepthImageID(desc.depthStencil);
                attachments[numRenderTargets] = imageHandler->GetDepthView(depthImageID);
            }

            VkFramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = pipeline.renderPass;
            framebufferInfo.attachmentCount = numAttachments;
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = static_cast<u32>(desc.states.viewport.width);
            framebufferInfo.height = static_cast<u32>(desc.states.viewport.height);
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(device->_device, &framebufferInfo, nullptr, &pipeline.framebuffer) != VK_SUCCESS)
            {
                NC_LOG_FATAL("Failed to create framebuffer!");
            }
            
            // -- Create Descriptor Set Layout from reflected SPIR-V --
            const ShaderBinary* shaderBinaries[2] = { shaderHandler->GetSPIRV(desc.states.vertexShader), shaderHandler->GetSPIRV(desc.states.pixelShader) };

            for (int i = 0; i < 2; i++)
            {
                SpvReflectShaderModule reflectModule = {};
                SpvReflectResult result = spvReflectCreateShaderModule(shaderBinaries[i]->size(), shaderBinaries[i]->data(), &reflectModule);

                if (result != SPV_REFLECT_RESULT_SUCCESS)
                {
                    NC_LOG_FATAL("We failed to reflect the spirv");
                }

                uint32_t count = 0;
                result = spvReflectEnumerateDescriptorSets(&reflectModule, &count, NULL);
                
                if (result != SPV_REFLECT_RESULT_SUCCESS)
                {
                    NC_LOG_FATAL("We failed to reflect the spirv descriptor set count");
                }

                std::vector<SpvReflectDescriptorSet*> sets(count);
                result = spvReflectEnumerateDescriptorSets(&reflectModule, &count, sets.data());
                
                if (result != SPV_REFLECT_RESULT_SUCCESS)
                {
                    NC_LOG_FATAL("We failed to reflect the spirv descriptor sets");
                }

                for (size_t set = 0; set < sets.size(); set++)
                {
                    const SpvReflectDescriptorSet& reflectionSet = *(sets[set]);

                    DescriptorSetLayoutData& layout = GetDescriptorSet(reflectionSet.set, pipeline.descriptorSetLayoutDatas);

                    for (uint32_t binding = 0; binding < reflectionSet.binding_count; binding++)
                    {
                        const SpvReflectDescriptorBinding& reflectionBinding = *(reflectionSet.bindings[binding]);

                        layout.bindings.push_back(VkDescriptorSetLayoutBinding());
                        VkDescriptorSetLayoutBinding& layoutBinding = layout.bindings.back();
                        layoutBinding.binding = reflectionBinding.binding;
                        layoutBinding.descriptorType = static_cast<VkDescriptorType>(reflectionBinding.descriptor_type);
                        layoutBinding.descriptorCount = 1;

                        for (uint32_t dim = 0; dim < reflectionBinding.array.dims_count; dim++)
                        {
                            layoutBinding.descriptorCount *= reflectionBinding.array.dims[dim];
                        }
                        layoutBinding.stageFlags = static_cast<VkShaderStageFlagBits>(reflectModule.shader_stage);
                    }
                    layout.setNumber = reflectionSet.set;
                    layout.createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                    layout.createInfo.bindingCount = static_cast<u32>(layout.bindings.size());
                    layout.createInfo.pBindings = layout.bindings.data();
                }
            }

            size_t numDescriptorSets = pipeline.descriptorSetLayoutDatas.size();
            pipeline.descriptorSetLayouts.resize(numDescriptorSets);

            for (size_t i = 0; i < numDescriptorSets; i++)
            {
                if (vkCreateDescriptorSetLayout(device->_device, &pipeline.descriptorSetLayoutDatas[i].createInfo, nullptr, &pipeline.descriptorSetLayouts[i]) != VK_SUCCESS)
                {
                    NC_LOG_FATAL("Failed to create descriptor set layout!");
                }
            }

            // -- Create shader stage infos --
            VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
            vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;

            vertShaderStageInfo.module = shaderHandler->GetShaderModule(desc.states.vertexShader);
            vertShaderStageInfo.pName = "main";

            VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
            fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;

            fragShaderStageInfo.module = shaderHandler->GetShaderModule(desc.states.pixelShader);
            fragShaderStageInfo.pName = "main";

            VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

            // Calculate stride
            u8 numAttributes = 0;
            u32 stride = 0;
            for (auto& inputLayout : desc.states.inputLayouts)
            {
                if (!inputLayout.enabled)
                    break;

                numAttributes++;
                stride += FormatConverterVK::ToByteSize(inputLayout.format);
            }

            // -- Create binding description --
            VkVertexInputBindingDescription bindingDescription = {};
            bindingDescription.binding = 0;
            bindingDescription.stride = stride;
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            // -- Create attribute descriptions --
            std::vector<VkVertexInputAttributeDescription> attributeDescriptions(numAttributes);
            u32 offset = 0;
            for (int i = 0; i < numAttributes; i++)
            {
                attributeDescriptions[i].binding = 0;
                attributeDescriptions[i].location = i;
                attributeDescriptions[i].format = FormatConverterVK::ToVkFormat(desc.states.inputLayouts[i].format);
                attributeDescriptions[i].offset = offset;

                offset += FormatConverterVK::ToByteSize(desc.states.inputLayouts[i].format);
            }

            VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
            vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertexInputInfo.vertexBindingDescriptionCount = 1;
            vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
            vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
            vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

            VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
            inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            inputAssembly.primitiveRestartEnable = VK_FALSE;

            // -- Set viewport and scissor rect --
            VkViewport viewport = {};
            viewport.x = desc.states.viewport.topLeftX;
            viewport.y = desc.states.viewport.topLeftY;
            viewport.width = static_cast<f32>(desc.states.viewport.width);
            viewport.height = static_cast<f32>(desc.states.viewport.height);
            viewport.minDepth = desc.states.viewport.minDepth;
            viewport.maxDepth = desc.states.viewport.maxDepth;

            VkRect2D scissor = {};
            scissor.offset = { desc.states.scissorRect.left, desc.states.scissorRect.top };
            scissor.extent = { static_cast<u32>(desc.states.scissorRect.right - desc.states.scissorRect.left), static_cast<u32>(desc.states.scissorRect.bottom - desc.states.scissorRect.top) };

            VkPipelineViewportStateCreateInfo viewportState = {};
            viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewportState.viewportCount = 1;
            viewportState.pViewports = &viewport;
            viewportState.scissorCount = 1;
            viewportState.pScissors = &scissor;

            // -- Rasterizer --
            VkPipelineRasterizationStateCreateInfo rasterizer = {};
            rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizer.depthClampEnable = VK_FALSE;
            rasterizer.rasterizerDiscardEnable = VK_FALSE;
            rasterizer.polygonMode = FormatConverterVK::ToVkPolygonMode(desc.states.rasterizerState.fillMode);
            rasterizer.lineWidth = 1.0f;
            rasterizer.cullMode = FormatConverterVK::ToVkCullModeFlags(desc.states.rasterizerState.cullMode);
            rasterizer.frontFace = FormatConverterVK::ToVkFrontFace(desc.states.rasterizerState.frontFaceMode);
            rasterizer.depthBiasEnable = desc.states.rasterizerState.depthBiasEnabled;
            rasterizer.depthBiasConstantFactor = static_cast<f32>(desc.states.rasterizerState.depthBias);
            rasterizer.depthBiasClamp = desc.states.rasterizerState.depthBiasClamp;
            rasterizer.depthBiasSlopeFactor = desc.states.rasterizerState.depthBiasSlopeFactor;

            // -- Multisampling --
            VkPipelineMultisampleStateCreateInfo multisampling = {};
            multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multisampling.sampleShadingEnable = VK_FALSE;
            multisampling.rasterizationSamples = FormatConverterVK::ToVkSampleCount(desc.states.rasterizerState.sampleCount);
            multisampling.minSampleShading = 1.0f; // Optional
            multisampling.pSampleMask = nullptr; // Optional
            multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
            multisampling.alphaToOneEnable = VK_FALSE; // Optional

            // -- Blenders --
            std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments(numRenderTargets);
            
            for (int i = 0; i < numRenderTargets; i++)
            {
                colorBlendAttachments[i].blendEnable = desc.states.blendState.renderTargets[i].blendEnable;
                colorBlendAttachments[i].srcColorBlendFactor = FormatConverterVK::ToVkBlendFactor(desc.states.blendState.renderTargets[i].srcBlend);
                colorBlendAttachments[i].dstColorBlendFactor = FormatConverterVK::ToVkBlendFactor(desc.states.blendState.renderTargets[i].destBlend);
                colorBlendAttachments[i].colorBlendOp = FormatConverterVK::ToVkBlendOp(desc.states.blendState.renderTargets[i].blendOp);
                colorBlendAttachments[i].srcAlphaBlendFactor = FormatConverterVK::ToVkBlendFactor(desc.states.blendState.renderTargets[i].srcBlendAlpha);
                colorBlendAttachments[i].dstAlphaBlendFactor = FormatConverterVK::ToVkBlendFactor(desc.states.blendState.renderTargets[i].destBlendAlpha);
                colorBlendAttachments[i].alphaBlendOp = FormatConverterVK::ToVkBlendOp(desc.states.blendState.renderTargets[i].blendOpAlpha);
                colorBlendAttachments[i].colorWriteMask = FormatConverterVK::ToVkColorComponentFlags(desc.states.blendState.renderTargets[i].renderTargetWriteMask);
            }

            VkPipelineColorBlendStateCreateInfo colorBlending = {};
            colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            colorBlending.logicOpEnable = desc.states.blendState.renderTargets[0].logicOpEnable;
            colorBlending.logicOp = FormatConverterVK::ToVkLogicOp(desc.states.blendState.renderTargets[0].logicOp);
            colorBlending.attachmentCount = numRenderTargets;
            colorBlending.pAttachments = colorBlendAttachments.data();
            colorBlending.blendConstants[0] = 0.0f; // TODO: Blend constants
            colorBlending.blendConstants[1] = 0.0f; // TODO: Blend constants
            colorBlending.blendConstants[2] = 0.0f; // TODO: Blend constants
            colorBlending.blendConstants[3] = 0.0f; // TODO: Blend constants
            
            VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = static_cast<u32>(pipeline.descriptorSetLayouts.size());
            pipelineLayoutInfo.pSetLayouts = pipeline.descriptorSetLayouts.data();
            pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
            pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

            if (vkCreatePipelineLayout(device->_device, &pipelineLayoutInfo, nullptr, &pipeline.pipelineLayout) != VK_SUCCESS)
            {
                NC_LOG_FATAL("Failed to create pipeline layout!");
            }

            VkGraphicsPipelineCreateInfo pipelineInfo = {};
            pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            pipelineInfo.stageCount = 2;
            pipelineInfo.pStages = shaderStages;
            pipelineInfo.pVertexInputState = &vertexInputInfo;
            pipelineInfo.pInputAssemblyState = &inputAssembly;
            pipelineInfo.pViewportState = &viewportState;
            pipelineInfo.pRasterizationState = &rasterizer;
            pipelineInfo.pMultisampleState = &multisampling;
            pipelineInfo.pDepthStencilState = nullptr; // Optional
            pipelineInfo.pColorBlendState = &colorBlending;
            pipelineInfo.pDynamicState = nullptr; // Optional
            pipelineInfo.layout = pipeline.pipelineLayout;
            pipelineInfo.renderPass = pipeline.renderPass;
            pipelineInfo.subpass = 0;
            pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
            pipelineInfo.basePipelineIndex = -1; // Optional

            if (vkCreateGraphicsPipelines(device->_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline.pipeline) != VK_SUCCESS)
            {
                NC_LOG_FATAL("Failed to create graphics pipeline!");
            }

            _graphicsPipelines.push_back(pipeline);
            return GraphicsPipelineID(static_cast<gIDType>(nextID));
        }

        ComputePipelineID PipelineHandlerVK::CreatePipeline(RenderDeviceVK* device, ShaderHandlerVK* shaderHandler, ImageHandlerVK* imageHandler, const ComputePipelineDesc& desc)
        {
            return ComputePipelineID();
        }

        u64 PipelineHandlerVK::CalculateCacheDescHash(const GraphicsPipelineDesc& desc)
        {
            GraphicsPipelineCacheDesc cacheDesc;
            cacheDesc.states = desc.states;

            cacheDesc.numSRVs = 0;
            for (auto& texture : desc.textures)
            {
                if (texture == RenderPassResource::Invalid())
                    break;

                cacheDesc.numSRVs++;
            }

            for (int i = 0; i < MAX_RENDER_TARGETS; i++)
            {
                if (desc.renderTargets[i] == RenderPassMutableResource::Invalid())
                    break;

                cacheDesc.renderTargets[i] = desc.MutableResourceToImageID(desc.renderTargets[i]);
            }

            if (desc.depthStencil != RenderPassMutableResource::Invalid())
            {
                cacheDesc.depthStencil = desc.MutableResourceToDepthImageID(desc.depthStencil);
            }

            u64 hash = XXHash64::hash(&cacheDesc, sizeof(cacheDesc), 0);

            return hash;
        }

        bool PipelineHandlerVK::TryFindExistingGPipeline(u64 descHash, size_t& id)
        {
            id = 0;

            for (auto& pipeline : _graphicsPipelines)
            {
                if (descHash == pipeline.cacheDescHash)
                {
                    return true;
                }
                id++;
            }

            return false;
        }

        bool PipelineHandlerVK::TryFindExistingCPipeline(u64 descHash, size_t& id)
        {
            id = 0;

            for (auto& pipeline : _computePipelines)
            {
                if (descHash == pipeline.cacheDescHash)
                {
                    return true;
                }
                id++;
            }

            return false;
        }

        DescriptorSetLayoutData& PipelineHandlerVK::GetDescriptorSet(u32 setNumber, std::vector<DescriptorSetLayoutData>& sets)
        {
            for (DescriptorSetLayoutData& set : sets)
            {
                if (set.setNumber == setNumber)
                {
                    return set;
                }
            }

            sets.push_back(DescriptorSetLayoutData());
            return sets.back();
        }
    }
}