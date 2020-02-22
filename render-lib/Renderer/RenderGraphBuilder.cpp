#include "RenderGraphBuilder.h"
#include "Renderer.h"
#include "RenderGraph.h"

namespace Renderer
{
    RenderGraphBuilder::RenderGraphBuilder(Memory::Allocator* allocator, Renderer* renderer)
        : _renderer(renderer)
        , _trackedImages(allocator, 32)
        , _trackedDepthImages(allocator, 32)
    {

    }

    void RenderGraphBuilder::Compile(CommandList* /*commandList*/)
    {

    }

    ImageID RenderGraphBuilder::Create(ImageDesc& /*desc*/)
    {
        return ImageID::Invalid();
    }

    DepthImageID RenderGraphBuilder::Create(DepthImageDesc& /*desc*/)
    {
        return DepthImageID::Invalid();
    }

    RenderPassResource RenderGraphBuilder::Read(ImageID id, ShaderStage /*shaderStage*/)
    {
        RenderPassResource resource = GetResource(id);

        return resource;
    }

    RenderPassResource RenderGraphBuilder::Read(DepthImageID id, ShaderStage /*shaderStage*/)
    {
        RenderPassResource resource = GetResource(id);

        return resource;
    }

    RenderPassMutableResource RenderGraphBuilder::Write(ImageID id, WriteMode /*writeMode*/, LoadMode /*loadMode*/)
    {
        RenderPassMutableResource resource = GetMutableResource(id);

        return resource;
    }

    RenderPassMutableResource RenderGraphBuilder::Write(DepthImageID id, WriteMode /*writeMode*/, LoadMode /*loadMode*/)
    {
        RenderPassMutableResource resource = GetMutableResource(id);

        return resource;
    }

    ImageID RenderGraphBuilder::GetImage(RenderPassResource resource)
    {
        using type = type_safe::underlying_type<RenderPassResource>;
        return _trackedImages[static_cast<type>(resource)];
    }

    ImageID RenderGraphBuilder::GetImage(RenderPassMutableResource resource)
    {
        using type = type_safe::underlying_type<RenderPassMutableResource>;
        return _trackedImages[static_cast<type>(resource)];
    }

    DepthImageID RenderGraphBuilder::GetDepthImage(RenderPassResource resource)
    {
        using type = type_safe::underlying_type<RenderPassResource>;
        return _trackedDepthImages[static_cast<type>(resource)];
    }

    DepthImageID RenderGraphBuilder::GetDepthImage(RenderPassMutableResource resource)
    {
        using type = type_safe::underlying_type<RenderPassMutableResource>;
        return _trackedDepthImages[static_cast<type>(resource)];
    }

    RenderPassResource RenderGraphBuilder::GetResource(ImageID id)
    {
        using _type = type_safe::underlying_type<ImageID>;

        _type i = 0;
        for (ImageID& trackedID : _trackedImages)
        {
            if (trackedID == id)
            {
                return RenderPassResource(i);
            }

            i++;
        }

        _trackedImages.Insert(id);
        return RenderPassResource(i);
    }

    RenderPassResource RenderGraphBuilder::GetResource(DepthImageID id)
    {
        using _type = type_safe::underlying_type<DepthImageID>;

        _type i = 0;
        for (DepthImageID& trackedID : _trackedDepthImages)
        {
            if (trackedID == id)
            {
                return RenderPassResource(i);
            }

            i++;
        }

        _trackedDepthImages.Insert(id);
        return RenderPassResource(i);
    }

    RenderPassMutableResource RenderGraphBuilder::GetMutableResource(ImageID id)
    {
        using _type = type_safe::underlying_type<ImageID>;

        _type i = 0;
        for (ImageID& trackedID : _trackedImages)
        {
            if (trackedID == id)
            {
                return RenderPassMutableResource(i);
            }

            i++;
        }

        _trackedImages.Insert(id);
        return RenderPassMutableResource(i);
    }

    RenderPassMutableResource RenderGraphBuilder::GetMutableResource(DepthImageID id)
    {
        using _type = type_safe::underlying_type<DepthImageID>;

        _type i = 0;
        for (DepthImageID& trackedID : _trackedDepthImages)
        {
            if (trackedID == id)
            {
                return RenderPassMutableResource(i);
            }

            i++;
        }

        _trackedDepthImages.Insert(id);
        return RenderPassMutableResource(i);
    }
}