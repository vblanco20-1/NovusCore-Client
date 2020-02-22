#include "ImageHandlerVK.h"
#include <Utils/DebugHandler.h>
#include <Utils/StringUtils.h>
#include "RenderDeviceVK.h"

namespace Renderer
{
    namespace Backend
    {
        ImageHandlerVK::ImageHandlerVK()
        {

        }

        ImageHandlerVK::~ImageHandlerVK()
        {

        }

        ImageID ImageHandlerVK::CreateImage(RenderDeviceVK* device, const ImageDesc& desc)
        {
            size_t nextHandle = _images.size();

            // Make sure we haven't exceeded the limit of the ImageID type, if this hits you need to change type of ImageID to something bigger
            assert(nextHandle < ImageID::MaxValue());
            using type = type_safe::underlying_type<ImageID>;

            Image image;
            image.desc = desc;

            assert(desc.dimensions.x > 0); // Make sure the width is valid
            assert(desc.dimensions.y > 0); // Make sure the height is valid
            assert(desc.depth > 0); // Make sure the depth is valid
            assert(desc.format != IMAGE_FORMAT_UNKNOWN); // Make sure the format is valid

            // Create image
            VkImageCreateInfo imageInfo = {};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.pNext = nullptr;
            imageInfo.flags = 0; // TODO: VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT and VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT? https://github.com/DiligentGraphics/DiligentCore/blob/1edcafe9bd41bdde86869d4e1c0212c78ce123b7/Graphics/GraphicsEngineVulkan/src/TextureVkImpl.cpp
            
            if (desc.depth == 1)
            {
                imageInfo.imageType = VK_IMAGE_TYPE_2D;
            }
            else
            {
                NC_LOG_FATAL("Non-3d images is currently unsupported");
            }

            imageInfo.format = ToVKFormat(desc.format);
            imageInfo.extent.width = desc.dimensions.x;
            imageInfo.extent.height = desc.dimensions.y;
            imageInfo.extent.depth = desc.depth;
            imageInfo.mipLevels = 1;
            imageInfo.arrayLayers = 1;
            imageInfo.samples = ToVKSampleCount(desc.sampleCount);
            imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            // We will sample directly from the color attachment
            imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT; // Transfers
            imageInfo.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // RTV
            imageInfo.usage |= VK_IMAGE_USAGE_SAMPLED_BIT; // SRV 
            imageInfo.usage |= VK_IMAGE_USAGE_STORAGE_BIT; // UAV

            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.queueFamilyIndexCount = 0;
            imageInfo.pQueueFamilyIndices = nullptr;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

            if (vkCreateImage(device->_device, &imageInfo, nullptr, &image.image) != VK_SUCCESS)
            {
                NC_LOG_FATAL("Failed to create image!");
            }

            // Bind memory
            VkMemoryRequirements memRequirements;
            vkGetImageMemoryRequirements(device->_device, image.image, &memRequirements);

            VkMemoryAllocateInfo allocInfo = {};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = device->FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

            if (vkAllocateMemory(device->_device, &allocInfo, nullptr, &image.memory) != VK_SUCCESS) 
            {
                NC_LOG_FATAL("Failed to allocate image memory!");
            }

            vkBindImageMemory(device->_device, image.image, image.memory, 0);

            // Create Color View
            VkImageViewCreateInfo colorViewInfo = {};
            colorViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            colorViewInfo.image = image.image;
            if (desc.depth == 1)
            {
                colorViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            }
            else
            {
                NC_LOG_FATAL("Non-3d images is currently unsupported");
            }
            colorViewInfo.format = imageInfo.format;
            colorViewInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
            colorViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            colorViewInfo.subresourceRange.baseMipLevel = 0;
            colorViewInfo.subresourceRange.levelCount = 1;
            colorViewInfo.subresourceRange.baseArrayLayer = 0;
            colorViewInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(device->_device, &colorViewInfo, nullptr, &image.colorView) != VK_SUCCESS)
            {
                NC_LOG_FATAL("Failed to create color image view!");
            }
            
            // Transition image from VK_IMAGE_LAYOUT_UNDEFINED to VK_IMAGE_LAYOUT_GENERAL
            device->TransitionImageLayout(image.image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

            _images.push_back(image);

            return ImageID(static_cast<type>(nextHandle));
        }

        DepthImageID ImageHandlerVK::CreateDepthImage(RenderDeviceVK* device, const DepthImageDesc& desc)
        {
            size_t nextHandle = _images.size();

            // Make sure we haven't exceeded the limit of the DepthImageID type, if this hits you need to change type of DepthImageID to something bigger
            assert(nextHandle < DepthImageID::MaxValue());
            using type = type_safe::underlying_type<DepthImageID>;

            DepthImage image;
            image.desc = desc;

            // Create image
            VkImageCreateInfo imageInfo = {};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.pNext = nullptr;
            imageInfo.flags = 0; // TODO: VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT and VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT? https://github.com/DiligentGraphics/DiligentCore/blob/1edcafe9bd41bdde86869d4e1c0212c78ce123b7/Graphics/GraphicsEngineVulkan/src/TextureVkImpl.cpp
            imageInfo.imageType = VK_IMAGE_TYPE_2D;

            imageInfo.format = ToVKFormat(desc.format);
            imageInfo.extent.width = desc.dimensions.x;
            imageInfo.extent.height = desc.dimensions.y;
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels = 1;
            imageInfo.arrayLayers = 1;
            imageInfo.samples = ToVKSampleCount(desc.sampleCount);
            imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            // We will sample directly from the color attachment
            imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT; // Transfers
            imageInfo.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT; // DSV
            imageInfo.usage |= VK_IMAGE_USAGE_SAMPLED_BIT; // SRV 
            //imageInfo.usage |= VK_IMAGE_USAGE_STORAGE_BIT; // UAV TODO

            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.queueFamilyIndexCount = 0;
            imageInfo.pQueueFamilyIndices = nullptr;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

            if (vkCreateImage(device->_device, &imageInfo, nullptr, &image.image) != VK_SUCCESS)
            {
                NC_LOG_FATAL("Failed to create image!");
            }

            // Bind memory
            VkMemoryRequirements memRequirements;
            vkGetImageMemoryRequirements(device->_device, image.image, &memRequirements);

            VkMemoryAllocateInfo allocInfo = {};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = device->FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

            if (vkAllocateMemory(device->_device, &allocInfo, nullptr, &image.memory) != VK_SUCCESS)
            {
                NC_LOG_FATAL("Failed to allocate image memory!");
            }

            vkBindImageMemory(device->_device, image.image, image.memory, 0);

            // Create Depth View
            VkImageViewCreateInfo depthViewInfo = {};
            depthViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            depthViewInfo.image = image.image;
            depthViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
           
            depthViewInfo.format = imageInfo.format;
            depthViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            depthViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            depthViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            depthViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            depthViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            depthViewInfo.subresourceRange.baseMipLevel = 0;
            depthViewInfo.subresourceRange.levelCount = 1;
            depthViewInfo.subresourceRange.baseArrayLayer = 0;
            depthViewInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(device->_device, &depthViewInfo, nullptr, &image.depthView) != VK_SUCCESS)
            {
                NC_LOG_FATAL("Failed to create depth image view!");
            }

            _depthImages.push_back(image);

            return DepthImageID(static_cast<type>(nextHandle));
        }

        const ImageDesc& ImageHandlerVK::GetDescriptor(const ImageID id)
        {
            using type = type_safe::underlying_type<ImageID>;

            // Lets make sure this id exists
            assert(_images.size() > static_cast<type>(id));
            return _images[static_cast<type>(id)].desc;
        }

        const DepthImageDesc& ImageHandlerVK::GetDescriptor(const DepthImageID id)
        {
            using type = type_safe::underlying_type<DepthImageID>;

            // Lets make sure this id exists
            assert(_depthImages.size() > static_cast<type>(id));
            return _depthImages[static_cast<type>(id)].desc;
        }

        VkImage ImageHandlerVK::GetImage(const ImageID id)
        {
            using type = type_safe::underlying_type<ImageID>;

            // Lets make sure this id exists
            assert(_images.size() > static_cast<type>(id));
            return _images[static_cast<type>(id)].image;
        }

        VkImageView ImageHandlerVK::GetColorView(const ImageID id)
        {
            using type = type_safe::underlying_type<ImageID>;

            // Lets make sure this id exists
            assert(_images.size() > static_cast<type>(id));
            return _images[static_cast<type>(id)].colorView;
        }

        VkImage ImageHandlerVK::GetImage(const DepthImageID id)
        {
            using type = type_safe::underlying_type<DepthImageID>;

            // Lets make sure this id exists
            assert(_depthImages.size() > static_cast<type>(id));
            return _depthImages[static_cast<type>(id)].image;
        }

        VkImageView ImageHandlerVK::GetDepthView(const DepthImageID id)
        {
            using type = type_safe::underlying_type<DepthImageID>;

            // Lets make sure this id exists
            assert(_depthImages.size() > static_cast<type>(id));
            return _depthImages[static_cast<type>(id)].depthView;
        }

        VkFormat ImageHandlerVK::ToVKFormat(const ImageFormat& format)
        {
            switch (format)
            {
            case IMAGE_FORMAT_UNKNOWN:                  assert(false); break; // This is an invalid format
            case IMAGE_FORMAT_R32G32B32A32_FLOAT:       return VK_FORMAT_R32G32B32A32_SFLOAT; // RGBA32, 128 bits per pixel
            case IMAGE_FORMAT_R32G32B32A32_UINT:        return VK_FORMAT_R32G32B32A32_UINT;
            case IMAGE_FORMAT_R32G32B32A32_SINT:        return VK_FORMAT_R32G32B32A32_SINT;
            case IMAGE_FORMAT_R32G32B32_FLOAT:          return VK_FORMAT_R32G32B32_SFLOAT; // RGB32, 96 bits per pixel
            case IMAGE_FORMAT_R32G32B32_UINT:           return VK_FORMAT_R32G32B32_UINT;
            case IMAGE_FORMAT_R32G32B32_SINT:           return VK_FORMAT_R32G32B32_SINT;
            case IMAGE_FORMAT_R16G16B16A16_FLOAT:       return VK_FORMAT_R16G16B16A16_SFLOAT; // RGBA16, 64 bits per pixel
            case IMAGE_FORMAT_R16G16B16A16_UNORM:       return VK_FORMAT_R16G16B16A16_UNORM;
            case IMAGE_FORMAT_R16G16B16A16_UINT:        return VK_FORMAT_R16G16B16A16_UINT;
            case IMAGE_FORMAT_R16G16B16A16_SNORM:       return VK_FORMAT_R16G16B16A16_SNORM;
            case IMAGE_FORMAT_R16G16B16A16_SINT:        return VK_FORMAT_R16G16B16A16_SINT;
            case IMAGE_FORMAT_R32G32_FLOAT:             return VK_FORMAT_R32G32_SFLOAT; // RG32, 64 bits per pixel
            case IMAGE_FORMAT_R32G32_UINT:              return VK_FORMAT_R32G32_UINT;
            case IMAGE_FORMAT_R32G32_SINT:              return VK_FORMAT_R32G32_SINT;
            case IMAGE_FORMAT_R10G10B10A2_UNORM:        return VK_FORMAT_A2R10G10B10_UNORM_PACK32; // RGB10A2, 32 bits per pixel
            case IMAGE_FORMAT_R10G10B10A2_UINT:         return VK_FORMAT_A2R10G10B10_UINT_PACK32;
            case IMAGE_FORMAT_R11G11B10_FLOAT:          return VK_FORMAT_B10G11R11_UFLOAT_PACK32; //RG11B10, 32 bits per pixel
            case IMAGE_FORMAT_R8G8B8A8_UNORM:           return VK_FORMAT_R8G8B8A8_UNORM; //RGBA8, 32 bits per pixel
            case IMAGE_FORMAT_R8G8B8A8_UNORM_SRGB:      return VK_FORMAT_R8G8B8A8_SRGB;
            case IMAGE_FORMAT_R8G8B8A8_UINT:            return VK_FORMAT_R8G8B8A8_UINT;
            case IMAGE_FORMAT_R8G8B8A8_SNORM:           return VK_FORMAT_R8G8B8A8_SNORM;
            case IMAGE_FORMAT_R8G8B8A8_SINT:            return VK_FORMAT_R8G8B8A8_SINT;
            case IMAGE_FORMAT_R16G16_FLOAT:             return VK_FORMAT_R16G16_SFLOAT; // RG16, 32 bits per pixel
            case IMAGE_FORMAT_R16G16_UNORM:             return VK_FORMAT_R16G16_UNORM;
            case IMAGE_FORMAT_R16G16_UINT:              return VK_FORMAT_R16G16_UINT;
            case IMAGE_FORMAT_R16G16_SNORM:             return VK_FORMAT_R16G16_SNORM;
            case IMAGE_FORMAT_R16G16_SINT:              return VK_FORMAT_R16G16_SINT;
            case IMAGE_FORMAT_R32_FLOAT:                return VK_FORMAT_R32_SFLOAT; // R32, 32 bits per pixel
            case IMAGE_FORMAT_R32_UINT:                 return VK_FORMAT_R32_UINT;
            case IMAGE_FORMAT_R32_SINT:                 return VK_FORMAT_R32_SINT;
            case IMAGE_FORMAT_R8G8_UNORM:               return VK_FORMAT_R8G8_UNORM; // RG8, 16 bits per pixel
            case IMAGE_FORMAT_R8G8_UINT:                return VK_FORMAT_R8G8_UINT;
            case IMAGE_FORMAT_R8G8_SNORM:               return VK_FORMAT_R8G8_SNORM;
            case IMAGE_FORMAT_R8G8_SINT:                return VK_FORMAT_R8G8_SINT;
            case IMAGE_FORMAT_R16_FLOAT:                return VK_FORMAT_R16_SFLOAT; // R16, 16 bits per pixel
            case IMAGE_FORMAT_D16_UNORM:                return VK_FORMAT_D16_UNORM; // Depth instead of Red
            case IMAGE_FORMAT_R16_UNORM:                return VK_FORMAT_R16_UNORM;
            case IMAGE_FORMAT_R16_UINT:                 return VK_FORMAT_R16_UINT;
            case IMAGE_FORMAT_R16_SNORM:                return VK_FORMAT_R16_SNORM;
            case IMAGE_FORMAT_R16_SINT:                 return VK_FORMAT_R16_SINT;
            case IMAGE_FORMAT_R8_UNORM:                 return VK_FORMAT_R8_UNORM; // R8, 8 bits per pixel
            case IMAGE_FORMAT_R8_UINT:                  return VK_FORMAT_R8_UINT;
            case IMAGE_FORMAT_R8_SNORM:                 return VK_FORMAT_R8_SNORM;
            case IMAGE_FORMAT_R8_SINT:                  return VK_FORMAT_R8_SINT;
            default:
                assert(false); // We have tried to convert a image format we don't know about, did we just add it?
            }
            return VK_FORMAT_UNDEFINED;
        }

        VkSampleCountFlagBits ImageHandlerVK::ToVKSampleCount(const SampleCount& sampleCount)
        {
            switch (sampleCount)
            {
            case SAMPLE_COUNT_1:    return VK_SAMPLE_COUNT_1_BIT;
            case SAMPLE_COUNT_2:    return VK_SAMPLE_COUNT_2_BIT;
            case SAMPLE_COUNT_4:    return VK_SAMPLE_COUNT_4_BIT;
            case SAMPLE_COUNT_8:    return VK_SAMPLE_COUNT_8_BIT;
            default:
                assert(false); // We have tried to convert a image format we don't know about, did we just add it?
            }
            return VK_SAMPLE_COUNT_1_BIT;
        }

        VkFormat ImageHandlerVK::ToVKFormat(const DepthImageFormat& format)
        {
            switch (format)
            {
            case DEPTH_IMAGE_FORMAT_UNKNOWN:                    assert(false); break; // This is an invalid format
            case DEPTH_IMAGE_FORMAT_D32_FLOAT_S8X24_UINT:       return VK_FORMAT_D32_SFLOAT_S8_UINT;
            case DEPTH_IMAGE_FORMAT_D32_FLOAT:                  return VK_FORMAT_D32_SFLOAT;
            case DEPTH_IMAGE_FORMAT_R32_FLOAT:                  return VK_FORMAT_R32_SFLOAT;
            case DEPTH_IMAGE_FORMAT_D24_UNORM_S8_UINT:          return VK_FORMAT_D24_UNORM_S8_UINT;
            case DEPTH_IMAGE_FORMAT_D16_UNORM:                  return VK_FORMAT_D16_UNORM;
            case DEPTH_IMAGE_FORMAT_R16_UNORM:                  return VK_FORMAT_R16_UNORM;

            default:
                assert(false); // We have tried to convert a image format we don't know about, did we just add it?
            }
            return VK_FORMAT_UNDEFINED;
        }
    }
}