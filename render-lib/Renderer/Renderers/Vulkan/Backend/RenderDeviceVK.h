#pragma once
#include <NovusTypes.h>
#include <vector>
#include <optional>
#include <vulkan/vulkan.h>

#include "../../../Descriptors/ImageDesc.h"
#include "../../../Descriptors/DepthImageDesc.h"

class Window;
struct GLFWwindow;

namespace Renderer
{
    namespace Backend
    {
        struct ConstantBufferBackend;
        struct ConstantBufferBackendVK;
        struct SwapChainVK;
        class ShaderHandlerVK;

        struct QueueFamilyIndices
        {
            std::optional<uint32_t> graphicsFamily;
            std::optional<uint32_t> presentFamily;

            bool IsComplete()
            {
                return graphicsFamily.has_value() && presentFamily.has_value();
            }
        };

        class RenderDeviceVK
        {
        public:
            ~RenderDeviceVK();

            void Init();
            void InitWindow(ShaderHandlerVK* shaderHandler, Window* window);

            ConstantBufferBackend* CreateConstantBufferBackend(size_t size);

            u32 GetFrameIndex() { return _frameIndex; }
            void EndFrame() { _frameIndex = (_frameIndex + 1) % FRAME_INDEX_COUNT; }

            void FlushGPU();

        private:
            void InitOnce();

            // InitOnce helper functions
            void InitVulkan();
            void SetupDebugMessenger();
            void PickPhysicalDevice();
            void CreateLogicalDevice();
            void CreateCommandPool();
            void CreateCommandBuffers();

            // InitWindow helper functions
            void CreateSurface(GLFWwindow* window, SwapChainVK* swapChain);
            void CreateSwapChain(const Vector2i& windowSize, SwapChainVK* swapChain);
            void CreateImageViews(SwapChainVK* swapChain);
            void CreateFrameBuffers(SwapChainVK* swapChain);
            void CreateBlitPipeline(ShaderHandlerVK* shaderHandler, SwapChainVK* swapChain);

            int RateDeviceSuitability(VkPhysicalDevice device);
            QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
            bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

            void CheckValidationLayerSupport();
            std::vector<const char*> GetRequiredExtensions();

            VkCommandBuffer BeginSingleTimeCommands();
            void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

            void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
            u32 FindMemoryType(u32 typeFilter, VkMemoryPropertyFlags properties);
            void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
            void TransitionImageLayout(VkImage image, VkImageAspectFlags aspects, VkImageLayout oldLayout, VkImageLayout newLayout);
            void TransitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkImageAspectFlags aspects, VkImageLayout oldLayout, VkImageLayout newLayout);

        private:
            static const u32 FRAME_INDEX_COUNT = 2;
            static bool _initialized;
            u32 _frameIndex;

            VkInstance _instance;
            VkDebugUtilsMessengerEXT _debugMessenger;

            VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
            VkDevice _device = VK_NULL_HANDLE;
            VkCommandPool _commandPool = VK_NULL_HANDLE;
            VkCommandBuffer _commandBuffers[FRAME_INDEX_COUNT];

            VkQueue _graphicsQueue = VK_NULL_HANDLE;
            VkQueue _presentQueue = VK_NULL_HANDLE;

            std::vector<ConstantBufferBackendVK*> _constantBufferBackends;
            std::vector<SwapChainVK*> _swapChains;

            friend class RendererVK;
            friend struct ConstantBufferBackendVK;
            friend class ImageHandlerVK;
            friend class ModelHandlerVK;
            friend class ShaderHandlerVK;
            friend class PipelineHandlerVK;
            friend class CommandListHandlerVK;
        };
    }
}