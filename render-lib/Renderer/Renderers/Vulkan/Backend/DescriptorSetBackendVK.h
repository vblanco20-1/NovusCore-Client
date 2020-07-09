#include "../../../DescriptorSet.h"
#include "../../../Descriptors/GraphicsPipelineDesc.h"
#include <robin_hood.h>

namespace Renderer
{
    namespace Backend
    {
        struct DescriptorSets
        {
            bool initialized = false;
            FrameResource<VkDescriptorSet, 2> sets;
        };

        struct DescriptorSetBackendVK : public DescriptorSetBackend
        {
            using gIDType = type_safe::underlying_type<GraphicsPipelineID>;

            robin_hood::unordered_map<gIDType, DescriptorSets> graphicsDescriptorSets;
        };
    }
}