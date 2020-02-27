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

            VkImageView GetImageView(const TextureID id);

            const TextureDesc& GetDescriptor(const TextureID id);

        private:
            struct Texture
            {
                TextureDesc desc;
                i32 width;
                i32 height;
                i32 channels;

                VkDeviceMemory memory;
                VkImage image;
                VkImageView imageView;
            };

        private:
            u8* ReadFile(const std::string& filename, i32& width, i32& height, i32& channels);
            void CreateTexture(RenderDeviceVK* device, Texture& texture, u8* pixels);

        private:
            std::vector<Texture> _textures;
        };
    }
}