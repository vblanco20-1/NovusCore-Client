#include "RendererVK.h"
#include "../../../Window/Window.h"
#include <Utils/StringUtils.h>
#include <Utils/DebugHandler.h>
#include <tracy/Tracy.hpp>
#include <tracy/TracyVulkan.hpp>

#include "Backend/RenderDeviceVK.h"
#include "Backend/BufferHandlerVK.h"
#include "Backend/ImageHandlerVK.h"
#include "Backend/TextureHandlerVK.h"
#include "Backend/ModelHandlerVK.h"
#include "Backend/ShaderHandlerVK.h"
#include "Backend/PipelineHandlerVK.h"
#include "Backend/CommandListHandlerVK.h"
#include "Backend/SamplerHandlerVK.h"
#include "Backend/SemaphoreHandlerVK.h"
#include "Backend/SwapChainVK.h"
#include "Backend/DebugMarkerUtilVK.h"
#include "Backend/DescriptorSetBuilderVK.h"
#include "Backend/FormatConverterVK.h"

#include "imgui/imgui_impl_vulkan.h"

namespace Renderer
{
    RendererVK::RendererVK(TextureDesc& debugTexture)
        : _device(new Backend::RenderDeviceVK())
    {
        // Create handlers
        _bufferHandler = new Backend::BufferHandlerVK();
        _imageHandler = new Backend::ImageHandlerVK();
        _textureHandler = new Backend::TextureHandlerVK();
        _modelHandler = new Backend::ModelHandlerVK();
        _shaderHandler = new Backend::ShaderHandlerVK();
        _pipelineHandler = new Backend::PipelineHandlerVK();
        _commandListHandler = new Backend::CommandListHandlerVK();
        _samplerHandler = new Backend::SamplerHandlerVK();
        _semaphoreHandler = new Backend::SemaphoreHandlerVK();

        // Init
        _device->Init();
        _bufferHandler->Init(_device);
        _imageHandler->Init(_device);
        _textureHandler->Init(_device, _bufferHandler);
        _modelHandler->Init(_device, _bufferHandler);
        _shaderHandler->Init(_device);
        _pipelineHandler->Init(_device, _shaderHandler, _imageHandler);
        _commandListHandler->Init(_device);
        _samplerHandler->Init(_device);
        _semaphoreHandler->Init(_device);

        _textureHandler->LoadDebugTexture(debugTexture);

        // Load dummy pipeline containing our global descriptorset
        VertexShaderDesc dummyShaderDesc;
        dummyShaderDesc.path = "Data/shaders/globalDataDummy.vs.hlsl.spv";

        GraphicsPipelineDesc dummyPipelineDesc;
        dummyPipelineDesc.states.vertexShader = _shaderHandler->LoadShader(dummyShaderDesc);

        _globalDummyPipeline = _pipelineHandler->CreatePipeline(dummyPipelineDesc);
    }

    void RendererVK::InitWindow(Window* window)
    {
        _device->InitWindow(_shaderHandler, window);
    }

    void RendererVK::Deinit()
    {
        _device->FlushGPU(); // Make sure it has finished rendering

        delete(_device);
        delete(_bufferHandler);
        delete(_imageHandler);
        delete(_textureHandler);
        delete(_modelHandler);
        delete(_shaderHandler);
        delete(_pipelineHandler);
        delete(_commandListHandler);
        delete(_samplerHandler);
        delete(_semaphoreHandler);
    }

    BufferID RendererVK::CreateBuffer(BufferDesc& desc)
    {
        return _bufferHandler->CreateBuffer(desc);
    }

    BufferID RendererVK::CreateTemporaryBuffer(BufferDesc& desc, u32 framesLifetime)
    {
        return _bufferHandler->CreateTemporaryBuffer(desc, framesLifetime);
    }

    void RendererVK::QueueDestroyBuffer(BufferID buffer)
    {
        _destroyLists[_destroyListIndex].buffers.push_back(buffer);
    }

    ImageID RendererVK::CreateImage(ImageDesc& desc)
    {
        return _imageHandler->CreateImage(desc);
    }

    DepthImageID RendererVK::CreateDepthImage(DepthImageDesc& desc)
    {
        return _imageHandler->CreateDepthImage(desc);
    }

    SamplerID RendererVK::CreateSampler(SamplerDesc& desc)
    {
        return _samplerHandler->CreateSampler(desc);
    }

    GPUSemaphoreID RendererVK::CreateGPUSemaphore()
    {
        return _semaphoreHandler->CreateGPUSemaphore();
    }

    GraphicsPipelineID RendererVK::CreatePipeline(GraphicsPipelineDesc& desc)
    {
        return _pipelineHandler->CreatePipeline(desc);
    }

    ComputePipelineID RendererVK::CreatePipeline(ComputePipelineDesc& desc)
    {
        return _pipelineHandler->CreatePipeline(desc);
    }

    ModelID RendererVK::CreatePrimitiveModel(PrimitiveModelDesc& desc)
    {
        return _modelHandler->CreatePrimitiveModel(desc);
    }

    void RendererVK::UpdatePrimitiveModel(ModelID model, PrimitiveModelDesc& desc)
    {
        _modelHandler->UpdatePrimitiveModel(model, desc);
    }

    TextureArrayID RendererVK::CreateTextureArray(TextureArrayDesc& desc)
    {
        return _textureHandler->CreateTextureArray(desc);
    }

    TextureID RendererVK::CreateDataTexture(DataTextureDesc& desc)
    {
        return _textureHandler->CreateDataTexture(desc);
    }

    TextureID RendererVK::CreateDataTextureIntoArray(DataTextureDesc& desc, TextureArrayID textureArray, u32& arrayIndex)
    {
        return _textureHandler->CreateDataTextureIntoArray(desc, textureArray, arrayIndex);
    }

    ModelID RendererVK::LoadModel(ModelDesc& desc)
    {
        return _modelHandler->LoadModel(desc);
    }

    TextureID RendererVK::LoadTexture(TextureDesc& desc)
    {
        return _textureHandler->LoadTexture(desc);
    }

    TextureID RendererVK::LoadTextureIntoArray(TextureDesc& desc, TextureArrayID textureArray, u32& arrayIndex)
    {
        return _textureHandler->LoadTextureIntoArray(desc, textureArray, arrayIndex);
    }

    VertexShaderID RendererVK::LoadShader(VertexShaderDesc& desc)
    {
        return _shaderHandler->LoadShader(desc);
    }

    PixelShaderID RendererVK::LoadShader(PixelShaderDesc& desc)
    {
        return _shaderHandler->LoadShader(desc);
    }

    ComputeShaderID RendererVK::LoadShader(ComputeShaderDesc& desc)
    {
        return _shaderHandler->LoadShader(desc);
    }

    void RendererVK::UnloadTexture(TextureID textureID)
    {
        _device->FlushGPU(); // Make sure we have finished rendering

        _textureHandler->UnloadTexture(textureID);
    }

    void RendererVK::UnloadTexturesInArray(TextureArrayID textureArrayID, u32 unloadStartIndex)
    {
        _device->FlushGPU(); // Make sure we have finished rendering

        _textureHandler->UnloadTexturesInArray(textureArrayID, unloadStartIndex);
    }

    static VmaBudget sBudgets[16] = { 0 };

    void RendererVK::FlipFrame(u32 frameIndex)
    {
        ZoneScopedC(tracy::Color::Red3);

        // Reset old commandbuffers
        _commandListHandler->FlipFrame();

        // Wait on frame fence
        {
            ZoneScopedNC("Wait For Fence", tracy::Color::Red3);

            VkFence frameFence = _commandListHandler->GetCurrentFence();

            u64 timeout = 5000000000; // 5 seconds in nanoseconds
            VkResult result = vkWaitForFences(_device->_device, 1, &frameFence, true, timeout);

            if (result == VK_TIMEOUT)
            {
                NC_LOG_FATAL("Waiting for frame fence took longer than 5 seconds, something is wrong!");
            }

            vkResetFences(_device->_device, 1, &frameFence);
        }

        _commandListHandler->ResetCommandBuffers();
        _bufferHandler->OnFrameStart();

        vmaSetCurrentFrameIndex(_device->_allocator, frameIndex);
        vmaGetBudget(_device->_allocator, sBudgets);
    }

    CommandListID RendererVK::BeginCommandList()
    {
        return _commandListHandler->BeginCommandList();
    }

    void RendererVK::EndCommandList(CommandListID commandListID)
    {
        if (_renderPassOpenCount != 0)
        {
            NC_LOG_FATAL("We found unmatched calls to BeginPipeline in your commandlist, for every BeginPipeline you need to also EndPipeline!");
        }

        _commandListHandler->EndCommandList(commandListID, VK_NULL_HANDLE);
    }

    void RendererVK::Clear(CommandListID commandListID, ImageID imageID, Color color)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);
        VkImage image = _imageHandler->GetImage(imageID);

        ImageDesc desc = _imageHandler->GetImageDesc(imageID);
        
        VkClearColorValue clearColorValue = {};
        clearColorValue.float32[0] = color.r;
        clearColorValue.float32[1] = color.g;
        clearColorValue.float32[2] = color.b;
        clearColorValue.float32[3] = color.a;

        VkImageSubresourceRange imageSubresourceRange;
        imageSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageSubresourceRange.baseMipLevel = 0;
        imageSubresourceRange.levelCount = 1;
        imageSubresourceRange.baseArrayLayer = 0;
        imageSubresourceRange.layerCount = desc.depth;

        // Transition image to TRANSFER_DST_OPTIMAL
        _device->TransitionImageLayout(commandBuffer, image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, desc.depth, 1);

        vkCmdClearColorImage(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearColorValue, 1, &imageSubresourceRange);

        // Transition image back to GENERAL
        _device->TransitionImageLayout(commandBuffer, image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL, desc.depth, 1);
    }

    void RendererVK::Clear(CommandListID commandListID, DepthImageID imageID, DepthClearFlags clearFlags, f32 depth, u8 stencil)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);
        VkImage image = _imageHandler->GetImage(imageID);

        VkClearDepthStencilValue clearDepthValue = {};
        VkImageSubresourceRange range = {};

        if (clearFlags == DepthClearFlags::DEPTH_CLEAR_DEPTH || clearFlags == DepthClearFlags::DEPTH_CLEAR_BOTH)
        {
            clearDepthValue.depth = depth;
            range.aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
        }

        if (clearFlags == DepthClearFlags::DEPTH_CLEAR_STENCIL || clearFlags == DepthClearFlags::DEPTH_CLEAR_BOTH)
        {
            clearDepthValue.stencil = stencil;
            range.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }

        range.layerCount = 1;
        range.levelCount = 1;
        range.baseArrayLayer = 0;
        range.baseMipLevel = 0;

        // Transition image to TRANSFER_DST_OPTIMAL
        _device->TransitionImageLayout(commandBuffer, image, range.aspectMask, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, 1);

        vkCmdClearDepthStencilImage(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearDepthValue, 1, &range);

        // Transition image back to DEPTH_STENCIL_ATTACHMENT_OPTIMAL
        _device->TransitionImageLayout(commandBuffer, image, range.aspectMask, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1, 1);
    }

    void RendererVK::Draw(CommandListID commandListID, u32 numVertices, u32 numInstances, u32 vertexOffset, u32 instanceOffset)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        if (_renderPassOpenCount <= 0)
        {
            NC_LOG_FATAL("You tried to draw without first calling BeginPipeline!");
        }

        vkCmdDraw(commandBuffer, numVertices, numInstances, vertexOffset, instanceOffset);
    }

    void RendererVK::DrawBindless(CommandListID commandListID, u32 numVertices, u32 numInstances)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        if (_renderPassOpenCount <= 0)
        {
            NC_LOG_FATAL("You tried to draw without first calling BeginPipeline!");
        }

        // Draw
        vkCmdDraw(commandBuffer, numVertices, numInstances, 0, 0);
    }

    void RendererVK::DrawIndexedBindless(CommandListID commandListID, ModelID modelID, u32 numVertices, u32 numInstances)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        if (_renderPassOpenCount <= 0)
        {
            NC_LOG_FATAL("You tried to draw without first calling BeginPipeline!");
        }

        if (_boundModelIndexBuffer != modelID)
        {
            // Bind index buffer
            VkBuffer indexBuffer = _modelHandler->GetIndexBuffer(modelID);
            vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

            _boundModelIndexBuffer = modelID;
        }
        
        // Draw
        vkCmdDrawIndexed(commandBuffer, numVertices, numInstances, 0, 0, 0);
    }

    void RendererVK::DrawIndexed(CommandListID commandListID, u32 numIndices, u32 numInstances, u32 indexOffset, u32 vertexOffset, u32 instanceOffset)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        if (_renderPassOpenCount <= 0)
        {
            NC_LOG_FATAL("You tried to draw without first calling BeginPipeline!");
        }

        vkCmdDrawIndexed(commandBuffer, numIndices, numInstances, indexOffset, vertexOffset, instanceOffset);
    }

    void RendererVK::DrawIndexedIndirect(CommandListID commandListID, BufferID argumentBuffer, u32 argumentBufferOffset, u32 drawCount)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        if (_renderPassOpenCount <= 0)
        {
            NC_LOG_FATAL("You tried to draw without first calling BeginPipeline!");
        }

        VkBuffer vkArgumentBuffer = _bufferHandler->GetBuffer(argumentBuffer);

        vkCmdDrawIndexedIndirect(commandBuffer, vkArgumentBuffer, argumentBufferOffset, drawCount, sizeof(VkDrawIndexedIndirectCommand));
    }

    void RendererVK::DrawIndexedIndirectCount(CommandListID commandListID, BufferID argumentBuffer, u32 argumentBufferOffset, BufferID drawCountBuffer, u32 drawCountBufferOffset, u32 maxDrawCount)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        if (_renderPassOpenCount <= 0)
        {
            NC_LOG_FATAL("You tried to draw without first calling BeginPipeline!");
        }

        VkBuffer vkArgumentBuffer = _bufferHandler->GetBuffer(argumentBuffer);
        VkBuffer vkDrawCountBuffer = _bufferHandler->GetBuffer(drawCountBuffer);

        Backend::RenderDeviceVK::fnVkCmdDrawIndexedIndirectCountKHR(commandBuffer, vkArgumentBuffer, argumentBufferOffset, vkDrawCountBuffer, drawCountBufferOffset, maxDrawCount, sizeof(VkDrawIndexedIndirectCommand));
    }

    void RendererVK::Dispatch(CommandListID commandListID, u32 threadGroupCountX, u32 threadGroupCountY, u32 threadGroupCountZ)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        vkCmdDispatch(commandBuffer, threadGroupCountX, threadGroupCountY, threadGroupCountZ);
    }

    void RendererVK::DispatchIndirect(CommandListID commandListID, BufferID argumentBuffer, u32 argumentBufferOffset)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);
        VkBuffer vkArgumentBuffer = _bufferHandler->GetBuffer(argumentBuffer);

        vkCmdDispatchIndirect(commandBuffer, vkArgumentBuffer, argumentBufferOffset);
    }

    void RendererVK::PopMarker(CommandListID commandListID)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);
        Backend::DebugMarkerUtilVK::PopMarker(commandBuffer);
    }

    void RendererVK::PushMarker(CommandListID commandListID, Color color, std::string name)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);
        Backend::DebugMarkerUtilVK::PushMarker(commandBuffer, color, name);
    }

    void RendererVK::BeginPipeline(CommandListID commandListID, GraphicsPipelineID pipelineID)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        GraphicsPipelineDesc pipelineDesc = _pipelineHandler->GetDescriptor(pipelineID);
        VkPipeline pipeline = _pipelineHandler->GetPipeline(pipelineID);
        VkRenderPass renderPass = _pipelineHandler->GetRenderPass(pipelineID);
        VkFramebuffer frameBuffer = _pipelineHandler->GetFramebuffer(pipelineID);

        if (_renderPassOpenCount != 0)
        {
            NC_LOG_FATAL("You need to match your BeginPipeline calls with a EndPipeline call before beginning another pipeline!");
        }
        _renderPassOpenCount++;

        uvec2 renderSize = _device->GetMainWindowSize();

        // Set up renderpass
        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = frameBuffer;
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = { renderSize.x, renderSize.y };

        // Start renderpass
        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        // Bind pipeline
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

        _commandListHandler->SetBoundGraphicsPipeline(commandListID, pipelineID);


        _boundModelIndexBuffer = ModelID::Invalid();
    }

    void RendererVK::EndPipeline(CommandListID commandListID, GraphicsPipelineID /*pipelineID*/)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        if (_renderPassOpenCount <= 0)
        {
            NC_LOG_FATAL("You tried to call EndPipeline without first calling BeginPipeline!");
        }
        _renderPassOpenCount--;

        vkCmdEndRenderPass(commandBuffer);
        _commandListHandler->SetBoundGraphicsPipeline(commandListID, GraphicsPipelineID::Invalid());
    }

    void RendererVK::BeginPipeline(CommandListID commandListID, ComputePipelineID pipelineID)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        VkPipeline pipeline = _pipelineHandler->GetPipeline(pipelineID);

        if (_renderPassOpenCount != 0)
        {
            NC_LOG_FATAL("You need to match your BeginPipeline calls with a EndPipeline call before beginning another pipeline!");
        }
        _renderPassOpenCount++;

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);

        _commandListHandler->SetBoundComputePipeline(commandListID, pipelineID);
    }

    void RendererVK::EndPipeline(CommandListID commandListID, ComputePipelineID pipelineID)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        if (_renderPassOpenCount <= 0)
        {
            NC_LOG_FATAL("You tried to call EndPipeline without first calling BeginPipeline!");
        }
        _renderPassOpenCount--;

        VkPipeline pipeline = _pipelineHandler->GetPipeline(pipelineID);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);

        _commandListHandler->SetBoundComputePipeline(commandListID, ComputePipelineID::Invalid());
    }

    void RendererVK::SetScissorRect(CommandListID commandListID, ScissorRect scissorRect)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        VkRect2D vkScissorRect = {};
        vkScissorRect.offset = { scissorRect.left, scissorRect.top };
        vkScissorRect.extent = { static_cast<u32>(scissorRect.right - scissorRect.left), static_cast<u32>(scissorRect.bottom - scissorRect.top) };

        vkCmdSetScissor(commandBuffer, 0, 1, &vkScissorRect);
    }

    void RendererVK::SetViewport(CommandListID commandListID, Viewport viewport)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        VkViewport vkViewport = {};
        vkViewport.x = viewport.topLeftX;
        vkViewport.y = viewport.height - viewport.topLeftY;
        vkViewport.width = viewport.width;
        vkViewport.height = -viewport.height;
        vkViewport.minDepth = viewport.minDepth;
        vkViewport.maxDepth = viewport.maxDepth;

        vkCmdSetViewport(commandBuffer, 0, 1, &vkViewport);
    }

    void RendererVK::SetVertexBuffer(CommandListID commandListID, u32 slot, BufferID bufferID)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        // Bind vertex buffer
        VkBuffer vertexBuffer = _bufferHandler->GetBuffer(bufferID);
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, slot, 1, &vertexBuffer, offsets);
    }

    void RendererVK::SetIndexBuffer(CommandListID commandListID, BufferID bufferID, IndexFormat indexFormat)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        // Bind index buffer
        VkBuffer indexBuffer = _bufferHandler->GetBuffer(bufferID);
        vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, Backend::FormatConverterVK::ToVkIndexType(indexFormat));
    }

    void RendererVK::SetBuffer(CommandListID commandListID, u32 slot, BufferID buffer)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        // Bind buffer
        VkBuffer vkBuffer = _bufferHandler->GetBuffer(buffer);
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, slot, 1, &vkBuffer, offsets);
    }

    bool RendererVK::ReflectDescriptorSet(const std::string& name, u32 nameHash, u32 type, i32& set, const std::vector<Backend::BindInfo>& bindInfos, u32& outBindInfoIndex, VkDescriptorSetLayoutBinding* outDescriptorLayoutBinding)
    {
        // Try to find a BindInfo with a matching name
        for(u32 i = 0; i < bindInfos.size(); i++)
        {
            auto& bindInfo = bindInfos[i];

            // If the name and type matches
            if (nameHash == bindInfo.nameHash && type == bindInfo.descriptorType)
            {
                // If we have a set, make sure it's the correct one
                if (set != -1)
                {
                    if (set != bindInfo.set)
                    {
                        NC_LOG_ERROR("While creating DescriptorSet, we found BindInfo with matching name (%s) and type (%u), but it didn't match the location (%i != %i)", bindInfo.name, bindInfo.descriptorType, bindInfo.set, set);
                    }
                }
                else
                {
                    set = bindInfo.set;
                }

                // Fill out descriptor set layout
                outDescriptorLayoutBinding->binding = bindInfo.binding;
                outDescriptorLayoutBinding->descriptorType = bindInfo.descriptorType;
                outDescriptorLayoutBinding->descriptorCount = bindInfo.count;
                outDescriptorLayoutBinding->stageFlags = bindInfo.stageFlags;
                outDescriptorLayoutBinding->pImmutableSamplers = NULL;

                outBindInfoIndex = i;

                return true;
            }
        }

        NC_LOG_ERROR("While creating DescriptorSet we encountered binding (%s) of type (%u) which did not have a matching BindInfo in the bound shaders", name.c_str(), type);
        return false;
    }

    void RendererVK::BindDescriptor(Backend::DescriptorSetBuilderVK* builder, void* imageInfosArraysVoid, Descriptor& descriptor)
    {
        std::vector<std::vector<VkDescriptorImageInfo>>& imageInfosArrays = *static_cast<std::vector<std::vector<VkDescriptorImageInfo>>*>(imageInfosArraysVoid);

        if (descriptor.descriptorType == DescriptorType::DESCRIPTOR_TYPE_SAMPLER)
        {
            VkDescriptorImageInfo imageInfo = {};
            imageInfo.sampler = _samplerHandler->GetSampler(descriptor.samplerID);

            builder->BindSampler(descriptor.nameHash, imageInfo);
        }
        else if (descriptor.descriptorType == DescriptorType::DESCRIPTOR_TYPE_TEXTURE)
        {
            VkDescriptorImageInfo imageInfo = {};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = _textureHandler->GetImageView(descriptor.textureID);

            builder->BindImage(descriptor.nameHash, imageInfo);
        }
        else if (descriptor.descriptorType == DescriptorType::DESCRIPTOR_TYPE_TEXTURE_ARRAY)
        {
            const std::vector<TextureID>& textureIDs = _textureHandler->GetTextureIDsInArray(descriptor.textureArrayID);
            std::vector<VkDescriptorImageInfo>& imageInfos = imageInfosArrays.emplace_back();

            u32 textureArraySize = _textureHandler->GetTextureArraySize(descriptor.textureArrayID);
            imageInfos.reserve(textureArraySize);
            
            u32 numTextures = static_cast<u32>(textureIDs.size());

            // From 0 to numTextures, add our actual textures
            bool texturesAreOnionTextures = false;
            for (auto textureID : textureIDs)
            {
                VkDescriptorImageInfo& imageInfo = imageInfos.emplace_back();
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfo.imageView = _textureHandler->GetImageView(textureID);
                imageInfo.sampler = VK_NULL_HANDLE;

                texturesAreOnionTextures = _textureHandler->IsOnionTexture(textureID);
            }

            // from numTextures to textureArraySize, add debug texture
            VkDescriptorImageInfo imageInfoDebugTexture;
            imageInfoDebugTexture.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            if (texturesAreOnionTextures)
            {
                imageInfoDebugTexture.imageView = _textureHandler->GetDebugOnionTextureImageView();
            }
            else
            {
                imageInfoDebugTexture.imageView = _textureHandler->GetDebugTextureImageView();
            }
            
            imageInfoDebugTexture.sampler = VK_NULL_HANDLE;

            for (u32 i = numTextures; i < textureArraySize; i++)
            {
                imageInfos.push_back(imageInfoDebugTexture);
            }
            
            builder->BindImageArray(descriptor.nameHash, imageInfos.data(), static_cast<i32>(imageInfos.size()));
        }
        else if (descriptor.descriptorType == DescriptorType::DESCRIPTOR_TYPE_IMAGE)
        {
            VkDescriptorImageInfo imageInfo = {};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
            imageInfo.imageView = _imageHandler->GetColorView(descriptor.imageID);

            builder->BindImage(descriptor.nameHash, imageInfo);
        }
        else if (descriptor.descriptorType == DescriptorType::DESCRIPTOR_TYPE_BUFFER)
        {
            VkDescriptorBufferInfo bufferInfo = {};
            bufferInfo.buffer = _bufferHandler->GetBuffer(descriptor.bufferID);
            bufferInfo.range = _bufferHandler->GetBufferSize(descriptor.bufferID);

            builder->BindBuffer(descriptor.nameHash, bufferInfo);
        }
    }

    void RendererVK::RecreateSwapChain(Backend::SwapChainVK* swapChain)
    {
        _device->RecreateSwapChain(_shaderHandler, swapChain);
        _pipelineHandler->OnWindowResize();
        _imageHandler->OnWindowResize();
    }

    void RendererVK::DestroyObjects(ObjectDestroyList& destroyList)
    {
        for (const BufferID buffer : destroyList.buffers)
        {
            _bufferHandler->DestroyBuffer(buffer);
        }

        destroyList.buffers.clear();
    }

    void RendererVK::BindDescriptorSet(CommandListID commandListID, DescriptorSetSlot slot, Descriptor* descriptors, u32 numDescriptors)
    {
        ZoneScopedNC("RendererVK::BindDescriptorSet", tracy::Color::Red3);

        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);
        GraphicsPipelineID graphicsPipelineID = _commandListHandler->GetBoundGraphicsPipeline(commandListID);
        ComputePipelineID computePipelineID = _commandListHandler->GetBoundComputePipeline(commandListID);

        if (graphicsPipelineID != GraphicsPipelineID::Invalid())
        {
            std::vector<std::vector<VkDescriptorImageInfo>> imageInfosArrays; // These need to live until builder->BuildDescriptor()
            imageInfosArrays.reserve(8);

            Backend::DescriptorSetBuilderVK* builder = _pipelineHandler->GetDescriptorSetBuilder(graphicsPipelineID);

            for (u32 i = 0; i < numDescriptors; i++)
            {
                ZoneScopedNC("BindDescriptor", tracy::Color::Red3);
                Descriptor& descriptor = descriptors[i];
                BindDescriptor(builder, &imageInfosArrays, descriptor);
            }

            VkDescriptorSet descriptorSet = builder->BuildDescriptor(static_cast<i32>(slot), Backend::DescriptorLifetime::PerFrame);

            VkPipelineLayout pipelineLayout = _pipelineHandler->GetPipelineLayout(graphicsPipelineID);

            // Bind descriptor set
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, slot, 1, &descriptorSet, 0, nullptr);
        } 
        else if (computePipelineID != ComputePipelineID::Invalid())
        {
            std::vector<std::vector<VkDescriptorImageInfo>> imageInfosArrays; // These need to live until builder->BuildDescriptor()
            imageInfosArrays.reserve(8);

            Backend::DescriptorSetBuilderVK* builder = _pipelineHandler->GetDescriptorSetBuilder(computePipelineID);

            for (u32 i = 0; i < numDescriptors; i++)
            {
                ZoneScopedNC("BindDescriptor", tracy::Color::Red3);
                Descriptor& descriptor = descriptors[i];
                BindDescriptor(builder, &imageInfosArrays, descriptor);
            }

            VkDescriptorSet descriptorSet = builder->BuildDescriptor(static_cast<i32>(slot), Backend::DescriptorLifetime::PerFrame);

            VkPipelineLayout pipelineLayout = _pipelineHandler->GetPipelineLayout(computePipelineID);

            // Bind descriptor set
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, slot, 1, &descriptorSet, 0, nullptr);
        }
    }

    void RendererVK::MarkFrameStart(CommandListID commandListID, u32 frameIndex)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        // Collect tracy timings
        TracyVkCollect(_device->_tracyContext, commandBuffer);

        // Add a marker specifying the frameIndex
        Backend::DebugMarkerUtilVK::PushMarker(commandBuffer, Color(1,1,1,1), std::to_string(frameIndex));
        Backend::DebugMarkerUtilVK::PopMarker(commandBuffer);

        // Free up any old descriptors
        _device->_descriptorMegaPool->SetFrame(frameIndex);
    }

#if !TRACY_ENABLE
    void RendererVK::BeginTrace(CommandListID /*commandListID*/, const tracy::SourceLocationData* /*sourceLocation*/)
    {
#else
    void RendererVK::BeginTrace(CommandListID commandListID, const tracy::SourceLocationData* sourceLocation)
    {

        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);
        tracy::VkCtxManualScope*& tracyScope = _commandListHandler->GetTracyScope(commandListID);

        if (tracyScope != nullptr)
        {
            NC_LOG_FATAL("Tried to begin GPU trace on a commandlist that already had a begun GPU trace");
        }

        tracyScope = new tracy::VkCtxManualScope(_device->_tracyContext, sourceLocation, true);
        tracyScope->Start(commandBuffer);
#endif
    }

#if !TRACY_ENABLE
    void RendererVK::EndTrace(CommandListID /*commandListID*/)
    {
#else
    void RendererVK::EndTrace(CommandListID commandListID)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);
        tracy::VkCtxManualScope*& tracyScope = _commandListHandler->GetTracyScope(commandListID);

        if (tracyScope == nullptr)
        {
            NC_LOG_FATAL("Tried to end GPU trace on a commandlist that didn't have a running trace");
        }

        tracyScope->End();
        delete tracyScope;
        tracyScope = nullptr;
#endif
    }

    void RendererVK::AddSignalSemaphore(CommandListID commandListID, GPUSemaphoreID semaphoreID)
    {
        VkSemaphore semaphore = _semaphoreHandler->GetVkSemaphore(semaphoreID);
        _commandListHandler->AddSignalSemaphore(commandListID, semaphore);
    }

    void RendererVK::AddWaitSemaphore(CommandListID commandListID, GPUSemaphoreID semaphoreID)
    {
        VkSemaphore semaphore = _semaphoreHandler->GetVkSemaphore(semaphoreID);
        _commandListHandler->AddWaitSemaphore(commandListID, semaphore);
    }

    void RendererVK::CopyBuffer(CommandListID commandListID, BufferID dstBuffer, u64 dstOffset, BufferID srcBuffer, u64 srcOffset, u64 range)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        VkBuffer vkDstBuffer = _bufferHandler->GetBuffer(dstBuffer);
        VkBuffer vkSrcBuffer = _bufferHandler->GetBuffer(srcBuffer);

        VkBufferCopy copyRegion = {};
        copyRegion.srcOffset = srcOffset;
        copyRegion.dstOffset = dstOffset;
        copyRegion.size = range;
        vkCmdCopyBuffer(commandBuffer, vkSrcBuffer, vkDstBuffer, 1, &copyRegion);
    }

    void RendererVK::PipelineBarrier(CommandListID commandListID, PipelineBarrierType type, BufferID buffer)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        VkPipelineStageFlags srcStageMask;
        VkPipelineStageFlags dstStageMask;

        VkBufferMemoryBarrier bufferBarrier = { VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER };
        bufferBarrier.buffer = _bufferHandler->GetBuffer(buffer);
        bufferBarrier.size = VK_WHOLE_SIZE;

        switch (type)
        {
        case PipelineBarrierType::TransferDestToIndirectArguments:
            srcStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dstStageMask = VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
            bufferBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            bufferBarrier.dstAccessMask = VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
            break;

        case PipelineBarrierType::TransferDestToComputeShaderRW:
            srcStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dstStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            bufferBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            bufferBarrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;
            break;

        case PipelineBarrierType::TransferDestToVertexBuffer:
            srcStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dstStageMask = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
            bufferBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            bufferBarrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
            break;

        case PipelineBarrierType::ComputeWriteToVertexShaderRead:
            srcStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            dstStageMask = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
            bufferBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
            bufferBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            break;

        case PipelineBarrierType::ComputeWriteToPixelShaderRead:
            srcStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            bufferBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
            bufferBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            break;

        case PipelineBarrierType::ComputeWriteToComputeShaderRead:
            srcStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            dstStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            bufferBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
            bufferBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_UNIFORM_READ_BIT;
            break;

        case PipelineBarrierType::ComputeWriteToIndirectArguments:
            srcStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            dstStageMask = VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
            bufferBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
            bufferBarrier.dstAccessMask = VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
            break;

        case PipelineBarrierType::ComputeWriteToVertexBuffer:
            srcStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            dstStageMask = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
            bufferBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
            bufferBarrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
            break;

        case PipelineBarrierType::ComputeWriteToTransferSrc:
            srcStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            dstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
            bufferBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
            bufferBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            break;

        case PipelineBarrierType::AllCommands:
            srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
            dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
            bufferBarrier.srcAccessMask = VK_ACCESS_FLAG_BITS_MAX_ENUM;;
            bufferBarrier.dstAccessMask = VK_ACCESS_FLAG_BITS_MAX_ENUM;;
            break;
        }

        vkCmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, 0, 0, nullptr, 1, &bufferBarrier, 0, nullptr);
    }

    void RendererVK::ImageBarrier(CommandListID commandListID, ImageID image)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);
        const VkImage& vkImage = _imageHandler->GetImage(image);
        const ImageDesc& imageDesc = _imageHandler->GetImageDesc(image);

        _device->TransitionImageLayout(commandBuffer, vkImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_GENERAL, imageDesc.depth, 1);
    }

    struct PyramidCallDataHack {
        uint32_t magic = 1773;
        DepthImageID depth;
        ImageID pyramid;
    };

    void RendererVK::PushConstant(CommandListID commandListID, void* data, u32 offset, u32 size)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);

        if (size == sizeof(PyramidCallDataHack))
        {
            PyramidCallDataHack* hk = (PyramidCallDataHack*)(data);
            if (hk->magic == 1773)
            {
            
            BuildPyramid(commandListID, ((PyramidCallDataHack*)(data))->depth, ((PyramidCallDataHack*)(data))->pyramid);
            return;
            }
           
        }

        {
       
			GraphicsPipelineID graphicsPipelineID = _commandListHandler->GetBoundGraphicsPipeline(commandListID);

			if (graphicsPipelineID != GraphicsPipelineID::Invalid())
			{
				VkPipelineLayout layout = _pipelineHandler->GetPipelineLayout(graphicsPipelineID);
				vkCmdPushConstants(commandBuffer, layout, VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT, offset, size, data);
			}

			ComputePipelineID computePipelineID = _commandListHandler->GetBoundComputePipeline(commandListID);
			if (computePipelineID != ComputePipelineID::Invalid())
			{
				VkPipelineLayout layout = _pipelineHandler->GetPipelineLayout(computePipelineID);
				vkCmdPushConstants(commandBuffer, layout, VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT, offset, size, data);
			}
        }
    }

    void RendererVK::Present(Window* window, ImageID imageID, GPUSemaphoreID semaphoreID)
    {
        CommandListID commandListID = _commandListHandler->BeginCommandList();
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);
        
        // Tracy profiling
        PushMarker(commandListID, Color::Red, "Present Blitting");
        tracy::VkCtxManualScope*& tracyScope = _commandListHandler->GetTracyScope(commandListID);

        if (tracyScope != nullptr)
        {
            NC_LOG_FATAL("Tried to begin GPU trace on a commandlist that already had a begun GPU trace");
        }

#if TRACY_ENABLE
        TracySourceLocation(presentBlitting, "PresentBlitting", tracy::Color::Yellow2);
        tracyScope = new tracy::VkCtxManualScope(_device->_tracyContext, &presentBlitting, true);
        tracyScope->Start(commandBuffer);
#endif

        Backend::SwapChainVK* swapChain = static_cast<Backend::SwapChainVK*>(window->GetSwapChain());
        u32 semaphoreIndex = swapChain->frameIndex;

        VkFence frameFence = _commandListHandler->GetCurrentFence();

        // Acquire next swapchain image
        u32 frameIndex;
        VkResult result = vkAcquireNextImageKHR(_device->_device, swapChain->swapChain, UINT64_MAX, swapChain->imageAvailableSemaphores.Get(semaphoreIndex), VK_NULL_HANDLE, &frameIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            RecreateSwapChain(swapChain);
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            NC_LOG_FATAL("Failed to acquire swap chain image!");
        }

        if (semaphoreID != GPUSemaphoreID::Invalid())
        {
            VkSemaphore semaphore = _semaphoreHandler->GetVkSemaphore(semaphoreID);
            _commandListHandler->AddWaitSemaphore(commandListID, semaphore); // Wait for the provided semaphore to finish
        }
        
        _commandListHandler->AddWaitSemaphore(commandListID, swapChain->imageAvailableSemaphores.Get(semaphoreIndex)); // Wait for swapchain image to be available
        _commandListHandler->AddSignalSemaphore(commandListID, swapChain->blitFinishedSemaphores.Get(semaphoreIndex)); // Signal that blitting is done

        ImageDesc imageDesc = _imageHandler->GetImageDesc(imageID);
        ImageComponentType componentType = ToImageComponentType(imageDesc.format);

        Backend::BlitPipeline& pipeline = swapChain->blitPipelines[componentType];

        VkImage image = _imageHandler->GetImage(imageID);

        // Update SRV descriptor
        VkDescriptorImageInfo imageInfos[2] = {};
        imageInfos[0].sampler = swapChain->sampler;

        imageInfos[1].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfos[1].imageView = _imageHandler->GetColorView(imageID);

        VkWriteDescriptorSet descriptorWrites[2] = {};
        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = pipeline.descriptorSets.Get(swapChain->frameIndex);
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = nullptr;
        descriptorWrites[0].pImageInfo = &imageInfos[0];
        descriptorWrites[0].pTexelBufferView = nullptr;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = pipeline.descriptorSets.Get(swapChain->frameIndex);
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pBufferInfo = nullptr;
        descriptorWrites[1].pImageInfo = &imageInfos[1];
        descriptorWrites[1].pTexelBufferView = nullptr;

        vkUpdateDescriptorSets(_device->_device, 2, descriptorWrites, 0, nullptr);

        // Set up renderpass
        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = swapChain->renderPass;
        renderPassInfo.framebuffer = swapChain->framebuffers.Get(frameIndex);
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = swapChain->extent;

        VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        // Transition image from GENERAL to SHADER_READ_ONLY_OPTIMAL
        _device->TransitionImageLayout(commandBuffer, image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, imageDesc.depth, 1);
       
        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
       
        // Bind pipeline and descriptors and render
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipeline);
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipelineLayout, 0, 1, &pipeline.descriptorSets.Get(swapChain->frameIndex), 0, nullptr);

        vkCmdDraw(commandBuffer, 3, 1, 0, 0);

        vkCmdEndRenderPass(commandBuffer);

        // Transition image from SHADER_READ_ONLY_OPTIMAL to GENERAL
        _device->TransitionImageLayout(commandBuffer, image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL, imageDesc.depth, 1);
        PopMarker(commandListID);

#if TRACY_ENABLE
        tracyScope->End();
        tracyScope = nullptr;
#endif

        _commandListHandler->EndCommandList(commandListID, frameFence);

        // Present
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &swapChain->blitFinishedSemaphores.Get(semaphoreIndex); // Wait for blitting to finish

        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &swapChain->swapChain;
        presentInfo.pImageIndices = &frameIndex;
        presentInfo.pResults = nullptr; // Optional

        result = vkQueuePresentKHR(_device->_presentQueue, &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            RecreateSwapChain(swapChain);
            return;
        }
        else if (result != VK_SUCCESS)
        {
            NC_LOG_FATAL("Failed to present swap chain image!");
        }

        //vkQueueWaitIdle(_device->_presentQueue);

        // Flip frameIndex between 0 and 1
        swapChain->frameIndex = !swapChain->frameIndex;

        _destroyListIndex = (_destroyListIndex + 1) % _destroyLists.size();
        DestroyObjects(_destroyLists[_destroyListIndex]);
    }

    void RendererVK::Present(Window* /*window*/, DepthImageID /*image*/, GPUSemaphoreID /*semaphoreID*/)
    {
        
    }
    
	void RendererVK::BuildDepthPyramid(CommandList& commandListID, DepthImageID depthSource, ImageID pyramid)
	{
        static PyramidCallDataHack *phack{nullptr};
        if (!phack) phack = new PyramidCallDataHack();
        
		phack->depth = depthSource;
		phack->pyramid = pyramid;
		phack->magic = 1773u;
        commandListID.PushConstant(phack, 0, sizeof(PyramidCallDataHack));
        //commandListID.BeginPipeline(_blit)
	}

	void RendererVK::CopyBuffer(BufferID dstBuffer, u64 dstOffset, BufferID srcBuffer, u64 srcOffset, u64 range)
    {
        VkBuffer vkDstBuffer = _bufferHandler->GetBuffer(dstBuffer);
        VkBuffer vkSrcBuffer = _bufferHandler->GetBuffer(srcBuffer);
        _device->CopyBuffer(vkDstBuffer, dstOffset, vkSrcBuffer, srcOffset, range);

        DestroyObjects(_destroyLists[_destroyListIndex]);
    }

    void RendererVK::FillBuffer(CommandListID commandListID, BufferID dstBuffer, u64 dstOffset, u64 size, u32 data)
    {
        VkCommandBuffer commandBuffer = _commandListHandler->GetCommandBuffer(commandListID);
        VkBuffer vkDstBuffer = _bufferHandler->GetBuffer(dstBuffer);

        vkCmdFillBuffer(commandBuffer, vkDstBuffer, dstOffset, size, data);
    }

    struct PyramidStructures {
        void build(RendererVK* render);

        VkPipeline downsamplePipeline;
        VkSampler minSampler;
		VkPipelineLayout pipelineLayout;
        VkDescriptorPool pool;
		VkDescriptorSetLayout descriptorSetLayout;		
		//VkPipeline pipeline;

        std::vector<VkDescriptorSet> sets;
        size_t setIdx;
        
        SamplerID sampler;
    };

	void PyramidStructures::build(RendererVK* render)
	{
        auto shaderHandler = render->_shaderHandler;
		// Load shaders
		VertexShaderDesc shaderDesc;
        shaderDesc.path = "Data/shaders/blitDepth.cs.hlsl.spv";
		VertexShaderID cmpShader = render->LoadShader(shaderDesc);

		

		// Create shader stage infos
		VkPipelineShaderStageCreateInfo stageInfo = {};
		stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;

        stageInfo.module = shaderHandler->GetShaderModule(cmpShader);
        stageInfo.pName = "main";

		// Create Descriptor Set Layout from reflected SPIR-V
		std::vector<Backend::BindInfo> bindInfos;
		{
			const Backend::BindReflection& bindReflection = shaderHandler->GetBindReflection(cmpShader);
			bindInfos.insert(bindInfos.end(), bindReflection.dataBindings.begin(), bindReflection.dataBindings.end());
		}

		std::vector<VkDescriptorSetLayoutBinding> bindings;
#if 0
		for (Backend::BindInfo& bindInfo : bindInfos)
		{
			VkDescriptorSetLayoutBinding layoutBinding = {};

			layoutBinding.binding = bindInfo.binding;
			layoutBinding.descriptorType = bindInfo.descriptorType;
			layoutBinding.descriptorCount = bindInfo.count;
			layoutBinding.stageFlags = bindInfo.stageFlags;

			bindings.push_back(layoutBinding);
		}
#else
		{
			VkDescriptorSetLayoutBinding layoutBinding = {};

			layoutBinding.binding = 0;
			layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
			layoutBinding.descriptorCount = 1;
			layoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

			bindings.push_back(layoutBinding);
		}
		{
			VkDescriptorSetLayoutBinding layoutBinding = {};

			layoutBinding.binding = 1;
			layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			layoutBinding.descriptorCount = 1;
			layoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

			bindings.push_back(layoutBinding);
		}
		{
			VkDescriptorSetLayoutBinding layoutBinding = {};

			layoutBinding.binding = 2;
			layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			layoutBinding.descriptorCount = 1;
			layoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

			bindings.push_back(layoutBinding);
		}
#endif
		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<u32>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(render->_device->_device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
		{
			NC_LOG_FATAL("Failed to create descriptor set layout!");
		}

        std::vector<VkDescriptorPoolSize> psizes;
        psizes.push_back({ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,100 });
        psizes.push_back({ VK_DESCRIPTOR_TYPE_SAMPLER,100 });
        psizes.push_back({ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,100 });

        VkDescriptorPoolCreateInfo poolinfo{};
        poolinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolinfo.maxSets = 100;

        poolinfo.poolSizeCount = (uint32_t)psizes.size();
        poolinfo.pPoolSizes = psizes.data();

		if (vkCreateDescriptorPool(render->_device->_device, &poolinfo, nullptr, &pool) != VK_SUCCESS)
		{
			NC_LOG_FATAL("Failed to create descriptor set layout!");
		}

        for (int i = 0; i < 100; i++)
        {
            VkDescriptorSet dset;

            VkDescriptorSetAllocateInfo allocinfo{};
            allocinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocinfo.descriptorPool = pool;
            allocinfo.descriptorSetCount = 1;
            allocinfo.pSetLayouts = &descriptorSetLayout;
            if (vkAllocateDescriptorSets(render->_device->_device, &allocinfo, &dset))
            {
                NC_LOG_FATAL("Failed to create descriptor set");
            }

            sets.push_back(dset);
        }

        setIdx = 0;

        VkPushConstantRange range{};
        range.offset = 0;
        range.size = 16;
        range.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
		pipelineLayoutInfo.pushConstantRangeCount = 1; // Optional
		pipelineLayoutInfo.pPushConstantRanges = &range; // Optional
        
		if (vkCreatePipelineLayout(render->_device->_device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			NC_LOG_FATAL("Failed to create pipeline layout!");
		}

        VkComputePipelineCreateInfo pipInfo{};
        pipInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipInfo.layout = pipelineLayout;
        pipInfo.stage = stageInfo;
       
		if (vkCreateComputePipelines(render->_device->_device,VK_NULL_HANDLE,1,&pipInfo, nullptr, &downsamplePipeline) != VK_SUCCESS)
		{
			NC_LOG_FATAL("Failed to create pipeline ");
		}


        SamplerDesc samplerDesc;
        samplerDesc.filter = SAMPLER_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR;
       
        samplerDesc.addressU = TEXTURE_ADDRESS_MODE_CLAMP;
        samplerDesc.addressV = TEXTURE_ADDRESS_MODE_CLAMP;
        samplerDesc.addressW = TEXTURE_ADDRESS_MODE_CLAMP;
        samplerDesc.minLOD = 0.f;
        samplerDesc.maxLOD = 16.f;
        samplerDesc.mode = SAMPLER_REDUCTION_MIN;

        sampler = render->CreateSampler(samplerDesc);
        minSampler = render->_samplerHandler->GetSampler(sampler);
		//VkSamplerCreateInfo createInfo = {};
		//
		//auto reductionMode = VK_SAMPLER_REDUCTION_MODE_MIN;
		//
		//createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		//createInfo.magFilter = VK_FILTER_LINEAR;
		//createInfo.minFilter = VK_FILTER_LINEAR;
		//createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		//createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		//createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		//createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		//createInfo.minLod = 0;
		//createInfo.maxLod = 16.f;
		//
		//VkSamplerReductionModeCreateInfoEXT createInfoReduction = { VK_STRUCTURE_TYPE_SAMPLER_REDUCTION_MODE_CREATE_INFO_EXT };
		//
		//if (reductionMode != VK_SAMPLER_REDUCTION_MODE_WEIGHTED_AVERAGE_EXT)
		//{
		//	createInfoReduction.reductionMode = reductionMode;
		//
		//	createInfo.pNext = &createInfoReduction;
		//}
		//
		//if (vkCreateSampler(render->_device->_device, &createInfo, 0, &minSampler) != VK_SUCCESS)
		//{
		//    NC_LOG_FATAL("Failed to create sampler ");
		//}
	}
    VkImageMemoryBarrier image_barrier(VkImage image, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlags aspectMask)
	{
		VkImageMemoryBarrier result = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };

		result.srcAccessMask = srcAccessMask;
		result.dstAccessMask = dstAccessMask;
		result.oldLayout = oldLayout;
		result.newLayout = newLayout;
		result.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		result.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		result.image = image;
		result.subresourceRange.aspectMask = aspectMask;
		result.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
		result.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

		return result;
	}

	inline uint32_t getGroupCount(uint32_t threadCount, uint32_t localSize)
	{
		return (threadCount + localSize - 1) / localSize;
	}
    void RendererVK::BuildPyramid(CommandListID commandListID, DepthImageID depthSource, ImageID image)
    {
        static PyramidStructures* pyr{ nullptr };

        if (pyr == nullptr)
        {
            pyr = new PyramidStructures();
            pyr->build(this);
        }
        VkCommandBuffer cmd = _commandListHandler->GetCommandBuffer(commandListID);
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, pyr->downsamplePipeline);


		VkImageMemoryBarrier depthReadBarriers[] =
		{
			image_barrier(_imageHandler->GetImage(depthSource), VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_DEPTH_BIT),
		};

		vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_DEPENDENCY_BY_REGION_BIT, 0, 0, 0, 0, 1, depthReadBarriers);


		for (uint32_t i = 0; i < _imageHandler->GetImageDesc(image).mipLevels; ++i)
		{
			VkDescriptorImageInfo destTarget;
			destTarget.sampler = pyr->minSampler;
			destTarget.imageView = _imageHandler->GetColorView(image,i);
			destTarget.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

			VkDescriptorImageInfo sourceTarget;
			sourceTarget.sampler = pyr->minSampler;
			if (i == 0)
			{
				sourceTarget.imageView = _imageHandler->GetDepthView(depthSource);
				sourceTarget.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			}
			else {
				sourceTarget.imageView = _imageHandler->GetColorView(image, i-1);
				sourceTarget.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
			}

			//VkDescriptorSet depthSet;
			//vkutil::DescriptorBuilder::begin(_descriptorLayoutCache, get_current_frame().dynamicDescriptorAllocator)
			//	.bind_image(0, &destTarget, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT)
			//	.bind_image(1, &sourceTarget, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_COMPUTE_BIT)
			//	.build(depthSet);

            std::vector<VkWriteDescriptorSet> writes;
            {
                VkWriteDescriptorSet write0{};
                write0.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write0.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
                write0.dstBinding = 0;
                write0.dstSet = pyr->sets[pyr->setIdx];
                write0.descriptorCount = 1;

                
                write0.pImageInfo = &destTarget;

                writes.push_back(write0);
            }
			{
				VkWriteDescriptorSet write1{};
				write1.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				write1.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
				write1.dstBinding = 1;
				write1.dstSet = pyr->sets[pyr->setIdx];
				write1.descriptorCount = 1;
				
				write1.pImageInfo = &sourceTarget;

				writes.push_back(write1);
			} 
            {
				VkWriteDescriptorSet write1{};
				write1.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				write1.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
				write1.dstBinding = 2;
				write1.dstSet = pyr->sets[pyr->setIdx];
				write1.descriptorCount = 1;

				write1.pImageInfo = &destTarget;

				writes.push_back(write1);
			}
            vkUpdateDescriptorSets(_device->_device,(uint32_t) writes.size(), writes.data(), 0, nullptr);


			vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, pyr->pipelineLayout, 0, 1, &pyr->sets[pyr->setIdx], 0, nullptr);

            pyr->setIdx = (pyr->setIdx+1) % pyr->sets.size();
            
			uint32_t levelWidth = _imageHandler->GetDimension(image).x >> i;
			uint32_t levelHeight = _imageHandler->GetDimension(image).y >> i;
			if (levelHeight < 1) levelHeight = 1;
			if (levelWidth < 1) levelWidth = 1;

			struct alignas(16) DepthReduceData
			{
				glm::vec2 imageSize;
			}; 
           
			DepthReduceData reduceData = { glm::vec2(levelWidth, levelHeight) };

			vkCmdPushConstants(cmd, pyr->pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(reduceData), &reduceData);
			vkCmdDispatch(cmd, getGroupCount(levelWidth, 32), getGroupCount(levelHeight, 32), 1);


			VkImageMemoryBarrier reduceBarrier = image_barrier(_imageHandler->GetImage(image), VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_ASPECT_COLOR_BIT);

			vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_DEPENDENCY_BY_REGION_BIT, 0, 0, 0, 0, 1, &reduceBarrier);
		}

		VkImageMemoryBarrier depthWriteBarrier = image_barrier(_imageHandler->GetImage(depthSource), VK_ACCESS_SHADER_READ_BIT, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_ASPECT_DEPTH_BIT);

		vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, VK_DEPENDENCY_BY_REGION_BIT, 0, 0, 0, 0, 1, &depthWriteBarrier);

	}

	void* RendererVK::MapBuffer(BufferID buffer)
    {
        void* mappedMemory;

        VkResult result = vmaMapMemory(_device->_allocator, _bufferHandler->GetBufferAllocation(buffer), &mappedMemory);
        if (result != VK_SUCCESS)
        {
            NC_LOG_ERROR("vmaMapMemory failed!\n");
            return nullptr;
        }
        return mappedMemory;
    }
    
    void RendererVK::UnmapBuffer(BufferID buffer)
    {
        vmaUnmapMemory(_device->_allocator, _bufferHandler->GetBufferAllocation(buffer));
    }

    size_t RendererVK::GetVRAMUsage()
    {
        size_t usage = sBudgets[0].usage;

        return usage;
    }

    size_t RendererVK::GetVRAMBudget()
    {
        size_t budget = sBudgets[0].budget;

        return budget;
    }

    void RendererVK::InitImgui()
    {
        _device->InitializeImguiVulkan();
    }

    void RendererVK::DrawImgui(CommandListID commandListID)
    {
        VkCommandBuffer cmd = _commandListHandler->GetCommandBuffer(commandListID);

        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
    }
}