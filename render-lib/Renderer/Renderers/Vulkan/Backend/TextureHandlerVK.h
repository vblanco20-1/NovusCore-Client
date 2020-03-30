#pragma once
#include <NovusTypes.h>
#include <vector>
#include <vulkan/vulkan.h>

#include "../../../Descriptors/TextureDesc.h"

namespace Renderer
{
    namespace Backend
    {
        class RenderDeviceVK;

        class TextureHandlerVK
        {
        public:
            TextureHandlerVK();
            ~TextureHandlerVK();

            TextureID LoadTexture(RenderDeviceVK* device, const TextureDesc& desc);
            TextureID CreateDataTexture(RenderDeviceVK* device, const DataTextureDesc& desc);

            VkImageView GetImageView(const TextureID id);

        private:
            struct Texture
            {
                i32 width;
                i32 height;
                i32 pixelSize;
                ImageFormat format;

                VkDeviceMemory memory;
                VkImage image;
                VkImageView imageView;

                std::string debugName = "";
            };

        private:
            u8* ReadFile(const std::string& filename, i32& width, i32& height, i32& channels);
            void CreateTexture(RenderDeviceVK* device, Texture& texture, u8* pixels);

        private:
            std::vector<Texture> _textures;
        };
    }
}