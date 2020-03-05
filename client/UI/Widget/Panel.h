#pragma once
#include "Widget.h"
#include <Renderer/Descriptors/ModelDesc.h>
#include <Renderer/Descriptors/TextureDesc.h>
#include <Renderer/ConstantBuffer.h>

class UIPanel;
class UIRenderer;

namespace UI
{
    class Panel : public Widget
    {
    public:
        struct PanelConstantBuffer
        {
            Vector4 color; // 16 bytes

            u8 padding[240] = {};
        };

    public:
        Panel(f32 posX, f32 posY, f32 width, f32 height);

    private:
        std::string& GetTexture();
        void SetTexture(std::string& texture);

        Vector4 GetColor();
        void SetColor(Vector4 color);

        Renderer::TextureID GetTextureID();
        void SetTextureID(Renderer::TextureID textureID);

        Renderer::ModelID GetModelID();
        void SetModelID(Renderer::ModelID modelID);

        Renderer::ConstantBuffer<PanelConstantBuffer>* GetConstantBuffer() { return _constantBuffer; }
        void SetConstantBuffer(Renderer::ConstantBuffer<PanelConstantBuffer>* constantBuffer) { _constantBuffer = constantBuffer; }

    private:
        Renderer::ModelID _modelID = Renderer::ModelID::Invalid();

        std::string _texture;
        Renderer::TextureID _textureID = Renderer::TextureID::Invalid();
        Vector4 _color;

        Renderer::ConstantBuffer<PanelConstantBuffer>* _constantBuffer = nullptr;

        static std::vector<Panel*> _panels;

        friend class ::UIPanel;
        friend class UIRenderer;
    };
}