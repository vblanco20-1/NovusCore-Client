#pragma once
#include <NovusTypes.h>
#include "../../../ConstantBuffer.h"
#include <vulkan/vulkan.h>

namespace Renderer
{
    namespace Backend
    {
        class RenderDeviceVK;

        struct ConstantBufferBackendVK : public ConstantBufferBackend
        {
            ConstantBufferBackendVK() {};
            ~ConstantBufferBackendVK()
            {

            }

            static const u32 FRAME_BUFFER_COUNT = 2;
            RenderDeviceVK* device;

            VkBuffer uniformBuffers[FRAME_BUFFER_COUNT] = { nullptr, nullptr };
            VkDeviceMemory uniformBuffersMemory[FRAME_BUFFER_COUNT] = { nullptr, nullptr };

            VkDescriptorPool descriptorPool = NULL;
            VkDescriptorSet descriptorSet[FRAME_BUFFER_COUNT];        

            size_t bufferSize;
            u8 useIndex = 0;

        private:
            void Apply(u32 frameIndex, void* data, size_t size) override;

            void* GetGPUResource(u32 frameIndex) override;
        };
    }
}