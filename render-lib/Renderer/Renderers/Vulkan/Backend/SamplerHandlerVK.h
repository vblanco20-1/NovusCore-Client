#pragma once
#include <NovusTypes.h>
#include <vector>
#include <vulkan/vulkan.h>
#include <robin_hood.h>

#include "../../../Descriptors/TextureDesc.h"
#include "../../../Descriptors/SamplerDesc.h"
#include "../../../Descriptors/GraphicsPipelineDesc.h"

namespace Renderer
{
    namespace Backend
    {
        class RenderDeviceVK;
        class TextureHandlerVK;
        class PipelineHandlerVK;

        class SamplerHandlerVK
        {
        public:
            SamplerHandlerVK();
            ~SamplerHandlerVK();

            SamplerID CreateSampler(RenderDeviceVK* device, const SamplerDesc& desc);

            VkDescriptorSet GetCombinedSampler(RenderDeviceVK* device, TextureHandlerVK* textureHandler, PipelineHandlerVK* pipelineHandler, const SamplerID samplerID, const u32 slot, const TextureID textureID, const GraphicsPipelineID pipelineID);

            const SamplerDesc& GetSamplerDesc(const SamplerID samplerID);

        private:
            struct CombinedSampler
            {
                VkDescriptorPool descriptorPool = NULL;
                VkDescriptorSet descriptorSet;
            };

            using _SamplerID = type_safe::underlying_type<SamplerID>;
            using _TextureID = type_safe::underlying_type<TextureID>;
            struct SamplerContainer
            {
                u64 samplerHash;
                SamplerDesc desc;

                VkSampler sampler;
                robin_hood::unordered_map<_TextureID, CombinedSampler> combinedSamplers;
            };

        private:
            u64 CalculateSamplerHash(const Sampler& desc);
            bool TryFindExistingSamplerContainer(u64 descHash, size_t& id);

            VkFilter ToVkFilterMag(SamplerFilter filter);
            VkFilter ToVkFilterMin(SamplerFilter filter);
            bool ToAnisotropyEnabled(SamplerFilter filter);
            VkSamplerAddressMode ToVkSamplerAddressMode(TextureAddressMode mode);
            VkBorderColor ToVkBorderColor(StaticBorderColor borderColor);
            VkCompareOp ToVkCompareOp(ComparisonFunc func);

        private:
            std::vector<SamplerContainer> _samplerContainers;
        };
    }
}