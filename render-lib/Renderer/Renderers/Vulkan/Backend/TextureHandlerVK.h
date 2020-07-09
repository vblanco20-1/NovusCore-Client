#pragma once
#include <NovusTypes.h>
#include <vector>
#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"

#include "../../../Descriptors/TextureDesc.h"
#include "../../../Descriptors/TextureArrayDesc.h"

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

            void LoadDebugTexture(RenderDeviceVK* device, const TextureDesc& desc);

            TextureID LoadTexture(RenderDeviceVK* device, const TextureDesc& desc);
            TextureID LoadTextureIntoArray(RenderDeviceVK* device, const TextureDesc& desc, TextureArrayID textureArray, u32& arrayIndex);

            TextureArrayID CreateTextureArray(RenderDeviceVK* device, const TextureArrayDesc& desc);

            TextureID CreateDataTexture(RenderDeviceVK* device, const DataTextureDesc& desc);
            TextureID CreateDataTextureIntoArray(RenderDeviceVK* device, const DataTextureDesc& desc, TextureArrayID textureArray, u32& arrayIndex);

            const std::vector<TextureID>& GetTextureIDsInArray(const TextureArrayID id);

            VkImageView GetImageView(const TextureID id);
            VkImageView GetDebugTextureImageView();

            u32 GetTextureArraySize(const TextureArrayID id);

        private:
            struct Texture
            {
                u64 hash;

                i32 width;
                i32 height;
                i32 layers = 1;

                VkFormat format;

                VmaAllocation allocation;
                VkImage image;
                VkImageView imageView;

                std::string debugName = "";
            };

            struct TextureArray
            {
                u32 size;
                std::vector<TextureID> textures;
                std::vector<u64> textureHashes;
            };

        private:
            u64 CalculateDescHash(const TextureDesc& desc);
            bool TryFindExistingTexture(u64 descHash, size_t& id);
            bool TryFindExistingTextureInArray(TextureArrayID arrayID, u64 descHash, size_t& arrayIndex, TextureID& textureId);

            u8* ReadFile(const std::string& filename, i32& width, i32& height, VkFormat& format);
            void CreateTexture(RenderDeviceVK* device, Texture& texture, u8* pixels);

        private:
            Texture _debugTexture;
            std::vector<Texture> _textures;
            std::vector<TextureArray> _textureArrays;
        };
    }
}