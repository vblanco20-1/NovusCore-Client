#pragma once
#include <NovusTypes.h>
#include <vector>
#include <vulkan/vulkan.h>
#include <robin_hood.h>

#include "../../../Descriptors/GraphicsPipelineDesc.h"
#include "../../../Descriptors/ComputePipelineDesc.h"

namespace Renderer
{
    namespace Backend
    {
        class RenderDeviceVK;
        class ShaderHandlerVK;
        class ImageHandlerVK;

        struct DescriptorSetLayoutData
        {
            uint32_t setNumber;
            VkDescriptorSetLayoutCreateInfo createInfo;
            std::vector<VkDescriptorSetLayoutBinding> bindings;
        };

        class PipelineHandlerVK
        {
            using gIDType = type_safe::underlying_type<GraphicsPipelineID>;
            using cIDType = type_safe::underlying_type<ComputePipelineID>;
        public:
            PipelineHandlerVK();
            ~PipelineHandlerVK();

            GraphicsPipelineID CreatePipeline(RenderDeviceVK* device, ShaderHandlerVK* shaderHandler, ImageHandlerVK* imageHandler, const GraphicsPipelineDesc& desc);
            ComputePipelineID CreatePipeline(RenderDeviceVK* device, ShaderHandlerVK* shaderHandler, ImageHandlerVK* imageHandler, const ComputePipelineDesc& desc);

            const GraphicsPipelineDesc& GetDescriptor(GraphicsPipelineID id) { return _graphicsPipelines[static_cast<gIDType>(id)].desc; }
            const ComputePipelineDesc& GetDescriptor(ComputePipelineID id) { return _computePipelines[static_cast<gIDType>(id)].desc; }

            VkPipeline GetPipeline(GraphicsPipelineID id) { return _graphicsPipelines[static_cast<gIDType>(id)].pipeline; }
            VkRenderPass GetRenderPass(GraphicsPipelineID id) { return _graphicsPipelines[static_cast<gIDType>(id)].renderPass; }
            VkFramebuffer GetFramebuffer(GraphicsPipelineID id) { return _graphicsPipelines[static_cast<gIDType>(id)].framebuffer; }

            DescriptorSetLayoutData& GetDescriptorSetLayoutData(GraphicsPipelineID id, u32 index) { return _graphicsPipelines[static_cast<gIDType>(id)].descriptorSetLayoutDatas[index]; }
            VkDescriptorSetLayout& GetDescriptorSetLayout(GraphicsPipelineID id, u32 index) { return _graphicsPipelines[static_cast<gIDType>(id)].descriptorSetLayouts[index]; }
            VkPipelineLayout& GetPipelineLayout(GraphicsPipelineID id) { return _graphicsPipelines[static_cast<gIDType>(id)].pipelineLayout; }

        private:

            struct GraphicsPipeline
            {
                GraphicsPipelineDesc desc;
                u64 cacheDescHash;

                VkRenderPass renderPass;
                
                VkPipelineLayout pipelineLayout;
                VkPipeline pipeline;
                VkFramebuffer framebuffer;

                std::vector<DescriptorSetLayoutData> descriptorSetLayoutDatas;
                std::vector<VkDescriptorSetLayout> descriptorSetLayouts;

                VkDescriptorPool descriptorPool;
                std::vector<VkDescriptorSet> descriptorSets;
            };

            struct GraphicsPipelineCacheDesc
            {
                GraphicsPipelineDesc::States states;
                u32 numSRVs = 0;
                ImageID renderTargets[MAX_RENDER_TARGETS] = { ImageID::Invalid(), ImageID::Invalid(), ImageID::Invalid(), ImageID::Invalid(), ImageID::Invalid(), ImageID::Invalid(), ImageID::Invalid(), ImageID::Invalid() };
                DepthImageID depthStencil = DepthImageID::Invalid();
            };

            struct ComputePipeline
            {
                ComputePipelineDesc desc;
                u64 cacheDescHash;
            };

        private:
            u64 CalculateCacheDescHash(const GraphicsPipelineDesc& desc);
            bool TryFindExistingGPipeline(u64 descHash, size_t& id);
            bool TryFindExistingCPipeline(u64 descHash, size_t& id);
            DescriptorSetLayoutData& GetDescriptorSet(u32 setNumber, std::vector<DescriptorSetLayoutData>& sets);
            
            VkFormat ToVkFormat(const ImageFormat& format);
            u32 ToByteSize(const InputFormat& format);
            VkFormat ToVkFormat(const InputFormat& format);
            VkPolygonMode ToVkPolygonMode(const FillMode& fillMode);
            VkCullModeFlags ToVkCullModeFlags(const CullMode& cullMode);
            VkFrontFace ToVkFrontFace(const FrontFaceState& frontFaceState);
            VkSampleCountFlagBits ToVkSampleCount(const SampleCount& sampleCount);
            VkBlendFactor ToVkBlendFactor(const BlendMode& blendMode);
            VkBlendOp ToVkBlendOp(const BlendOp& blendOp);
            VkColorComponentFlags ToVkColorComponentFlags(const u8& componentFlags);
            VkLogicOp ToVkLogicOp(const LogicOp& logicOp);

        private:
            std::vector<GraphicsPipeline> _graphicsPipelines;
            std::vector<ComputePipeline> _computePipelines;
        };
    }
}