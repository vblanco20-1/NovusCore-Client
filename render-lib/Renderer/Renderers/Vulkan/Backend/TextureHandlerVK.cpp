#include "TextureHandlerVK.h"
#include <Utils/DebugHandler.h>
#include <Utils/StringUtils.h>
#include "RenderDeviceVK.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Renderer
{
    namespace Backend
    {
        TextureHandlerVK::TextureHandlerVK()
        {

        }

        TextureHandlerVK::~TextureHandlerVK()
        {

        }

        TextureID TextureHandlerVK::LoadTexture(RenderDeviceVK* device, const TextureDesc& desc)
        {
            size_t nextHandle = _textures.size();

            // Make sure we haven't exceeded the limit of the ImageID type, if this hits you need to change type of ImageID to something bigger
            assert(nextHandle < TextureID::MaxValue());
            using type = type_safe::underlying_type<TextureID>;

            Texture texture;
            texture.desc = desc;

            u8* pixels;
            pixels = ReadFile(desc.path, texture.width, texture.height, texture.channels);
            if (!pixels)
            {
                NC_LOG_FATAL("Failed to load texture image!");
            }

            CreateTexture(device, texture, pixels);

            _textures.push_back(texture);
            return TextureID(static_cast<type>(nextHandle));
        }

        VkImageView TextureHandlerVK::GetImageView(const TextureID id)
        {
            using type = type_safe::underlying_type<TextureID>;

            // Lets make sure this id exists
            assert(_textures.size() > static_cast<type>(id));
            return _textures[static_cast<type>(id)].imageView;
        }

        const TextureDesc& TextureHandlerVK::GetDescriptor(const TextureID id)
        {
            using type = type_safe::underlying_type<TextureID>;

            // Lets make sure this id exists
            assert(_textures.size() > static_cast<type>(id));
            return _textures[static_cast<type>(id)].desc;
        }

        u8* TextureHandlerVK::ReadFile(const std::string& filename, i32& width, i32& height, i32& channels)
        {
            return stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb_alpha);
        }

        void TextureHandlerVK::CreateTexture(RenderDeviceVK* device, Texture& texture, u8* pixels)
        {
            VkBuffer stagingBuffer;
            VkDeviceMemory stagingBufferMemory;

            VkDeviceSize imageSize = texture.width * texture.height * 4;

            device->CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

            void* data;
            vkMapMemory(device->_device, stagingBufferMemory, 0, imageSize, 0, &data);
            memcpy(data, pixels, static_cast<size_t>(imageSize));
            vkUnmapMemory(device->_device, stagingBufferMemory);

            stbi_image_free(pixels);

            VkImageCreateInfo imageInfo = {};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = VK_IMAGE_TYPE_2D;
            imageInfo.extent.width = static_cast<u32>(texture.width);
            imageInfo.extent.height = static_cast<u32>(texture.height);
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels = 1;
            imageInfo.arrayLayers = 1;
            imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
            imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            imageInfo.flags = 0; // Optional

            if (vkCreateImage(device->_device, &imageInfo, nullptr, &texture.image) != VK_SUCCESS)
            {
                NC_LOG_FATAL("Failed to create texture image!");
            }

            VkMemoryRequirements memRequirements;
            vkGetImageMemoryRequirements(device->_device, texture.image, &memRequirements);

            VkMemoryAllocateInfo allocInfo = {};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = device->FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

            if (vkAllocateMemory(device->_device, &allocInfo, nullptr, &texture.memory) != VK_SUCCESS)
            {
                NC_LOG_FATAL("Failed to allocate texture image memory!");
            }

            vkBindImageMemory(device->_device, texture.image, texture.memory, 0);

            device->TransitionImageLayout(texture.image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
            device->CopyBufferToImage(stagingBuffer, texture.image, static_cast<u32>(texture.width), static_cast<u32>(texture.height));
            device->TransitionImageLayout(texture.image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

            VkImageViewCreateInfo viewInfo = {};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = texture.image;
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(device->_device, &viewInfo, nullptr, &texture.imageView) != VK_SUCCESS)
            {
                NC_LOG_FATAL("Failed to create texture image view!");
            }
        }

    }
}