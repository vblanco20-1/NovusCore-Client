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
        Renderer::ModelID GetModelID();
        void SetModelID(Renderer::ModelID modelID);

        std::string& GetTexture();
        void SetTexture(std::string& texture);

        Renderer::TextureID GetTextureID();
        void SetTextureID(Renderer::TextureID textureID);

        Vector4 GetColor();
        void SetColor(Vector4 color);

        bool IsClickable();
        void SetClickable(bool value);

        bool IsDragable();
        void SetDragable(bool value);


        Renderer::ConstantBuffer<PanelConstantBuffer>* GetConstantBuffer() { return _constantBuffer; }
        void SetConstantBuffer(Renderer::ConstantBuffer<PanelConstantBuffer>* constantBuffer) { _constantBuffer = constantBuffer; }

    private:
        Renderer::ModelID _modelID = Renderer::ModelID::Invalid();

        std::string _texture;
        Renderer::TextureID _textureID = Renderer::TextureID::Invalid();
        Vector4 _color;
        bool _clickable;
        bool _dragable;
        bool _isDragging;
        bool _didDrag;
        Vector2 _deltaDragPosition;

        bool IsDragging();
        void BeingDrag(Vector2 deltaDragPosition);
        Vector2 GetDeltaDragPosition();
        bool DidDrag();
        void SetDidDrag();
        void EndDrag();

        Renderer::ConstantBuffer<PanelConstantBuffer>* _constantBuffer = nullptr;

        friend class ::UIPanel;
        friend class UIRenderer;
    };
}