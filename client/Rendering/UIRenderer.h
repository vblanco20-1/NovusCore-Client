#pragma once
#include <NovusTypes.h>

#include <Renderer/Descriptors/ImageDesc.h>
#include <Renderer/Descriptors/DepthImageDesc.h>
#include <Renderer/Descriptors/TextureDesc.h>
#include <Renderer/Descriptors/ModelDesc.h>
#include <Renderer/Descriptors/SamplerDesc.h>
#include <Renderer/ConstantBuffer.h>
#include <Renderer/InstanceData.h>

namespace Renderer
{
    class RenderGraph;
    class Renderer;
}

class Window;
class InputBinding;
class UIRenderer
{
public:
    UIRenderer(Renderer::Renderer* renderer);

    void Update(f32 deltaTime);
    void AddUIPass(Renderer::RenderGraph* renderGraph, Renderer::ImageID renderTarget, u8 frameIndex);
    void OnMouseClick(Window* window, std::shared_ptr<InputBinding> binding);
    void OnMousePositionUpdate(Window* window, f32 x, f32 y);

private:
    void CreatePermanentResources();

    // Helper functions
    Renderer::TextureID ReloadTexture(std::string& texturePath);
    void CalculateVertices(const vec3& pos, const vec2& size, std::vector<Renderer::Vertex>& vertices);

private:
    Renderer::Renderer* _renderer;

    Renderer::SamplerID _linearSampler;
};