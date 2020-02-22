#pragma once
#include <NovusTypes.h>
#include <vector>
#include <vulkan/vulkan.h>

#include "../../../Descriptors/ImageDesc.h"
#include "../../../Descriptors/DepthImageDesc.h"

namespace Renderer
{
    namespace Backend
    {
        class RenderDeviceVK;

        class ImageHandlerVK
        {
        public:
            ImageHandlerVK();
            ~ImageHandlerVK();

            ImageID CreateImage(RenderDeviceVK* device, const ImageDesc& desc);
            DepthImageID CreateDepthImage(RenderDeviceVK* device, const DepthImageDesc& desc);

            const ImageDesc& GetDescriptor(const ImageID id);
            const DepthImageDesc& GetDescriptor(const DepthImageID id);

            VkImage GetImage(const ImageID id);
            VkImageView GetColorView(const ImageID id);

            VkImage GetImage(const DepthImageID id);
            VkImageView GetDepthView(const DepthImageID id);

        private:
            struct Image
            {
                ImageDesc desc;

                VkDeviceMemory memory;
                VkImage image;
                VkImageView colorView;
            };

            struct DepthImage
            {
                DepthImageDesc desc;

                VkDeviceMemory memory;
                VkImage image;
                VkImageView depthView;
            };

        private:
            VkFormat ToVKFormat(const ImageFormat& format);
            VkSampleCountFlagBits ToVKSampleCount(const SampleCount& sampleCount);

            VkFormat ToVKFormat(const DepthImageFormat& format);

        private:
            std::vector<Image> _images;
            std::vector<DepthImage> _depthImages;
        };
    }
}