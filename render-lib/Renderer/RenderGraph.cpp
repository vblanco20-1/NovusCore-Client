#include "RenderGraph.h"
#include "RenderGraphBuilder.h"
#include <tracy/Tracy.hpp>

#include "Renderer.h"

namespace Renderer
{
    bool RenderGraph::Init(RenderGraphDesc& desc)
    {
        _desc = desc;
        assert(desc.allocator != nullptr); // You need to set an allocator

        _renderGraphBuilder = Memory::Allocator::New<RenderGraphBuilder>(desc.allocator, desc.allocator, _renderer);

        return true;
    }

    RenderGraph::~RenderGraph()
    {
        for (IRenderPass* pass : _passes)
        {
            pass->DeInit();
        }
    }

    /*void RenderGraph::AddPass(RenderPass& pass)
    {
        _passes.push_back(pass);
    }*/

    void RenderGraph::Setup()
    {
        ZoneScopedNC("RenderGraph::Setup", tracy::Color::Red2)
        for (IRenderPass* pass : _passes)
        {
            ZoneScopedC(tracy::Color::Red2)
            ZoneName(pass->_name, pass->_nameLength)

            if (pass->Setup(_renderGraphBuilder))
            {
                _executingPasses.Insert(pass);
            }
        }
    }

    void RenderGraph::Execute()
    {

        ZoneScopedNC("RenderGraph::Execute", tracy::Color::Red2)
        
        CommandList commandList(_renderer, _desc.allocator);
        // TODO: Parallel_for this
        commandList.PushMarker("RenderGraph", Color(0.0f, 0.0f, 0.4f));
        for (IRenderPass* pass : _executingPasses)
        {
            ZoneScopedC(tracy::Color::Red2)
            ZoneName(pass->_name, pass->_nameLength)

            pass->Execute(commandList);
        }
        commandList.PopMarker();
        
        {
            ZoneScopedNC("CommandList::Execute", tracy::Color::Red2)
            commandList.Execute();
        }
    }

    void RenderGraph::InitializePipelineDesc(GraphicsPipelineDesc& desc) const
    {
        desc.ResourceToImageID = [&](RenderPassResource resource) 
        {
            return _renderGraphBuilder->GetImage(resource);
        };
        desc.ResourceToDepthImageID = [&](RenderPassResource resource) 
        {
            return _renderGraphBuilder->GetDepthImage(resource);
        };
        desc.MutableResourceToImageID = [&](RenderPassMutableResource resource) 
        {
            return _renderGraphBuilder->GetImage(resource);
        };
        desc.MutableResourceToDepthImageID = [&](RenderPassMutableResource resource) 
        {
            return _renderGraphBuilder->GetDepthImage(resource);
        };
    }
}