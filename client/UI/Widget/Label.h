#pragma once
#include "Widget.h"
#include <Renderer/Descriptors/ModelDesc.h>
#include <Renderer/Descriptors/TextureDesc.h>
#include <Renderer/Descriptors/FontDesc.h>
#include <Renderer/ConstantBuffer.h>

class UILabel;
class UIRenderer;

namespace Renderer
{
    struct Font;
}

namespace UI
{
    class Label : public Widget
    {
    public:
        struct LabelConstantBuffer
        {
            Vector4 textColor; // 16 bytes
            Vector4 outlineColor; // 16 bytes
            f32 outlineWidth; // 4 bytes

            u8 padding[220] = {};
        };

    public:
        Label(f32 posX, f32 posY, f32 width, f32 height);

    private:
        std::string& GetText();
        void SetText(std::string& text);

        u32 GetTextLength();

        Vector4 GetColor();
        void SetColor(Vector4 color);

        f32 GetOutlineWidth();
        void SetOutlineWidth(f32 width);

        Vector4 GetOutlineColor();
        void SetOutlineColor(Vector4 color);

        void SetFont(std::string& fontPath, f32 fontSize);
        std::string& GetFontPath();
        f32 GetFontSize();

        Renderer::ConstantBuffer<LabelConstantBuffer>* GetConstantBuffer() { return _constantBuffer; }
        void SetConstantBuffer(Renderer::ConstantBuffer<LabelConstantBuffer>* constantBuffer) { _constantBuffer = constantBuffer; }

    private:
        std::string _text;
        Vector4 _color = Vector4(1,1,1,1);
        Vector4 _outlineColor = Vector4(0, 0, 0, 1);
        f32 _outlineWidth = 0.0f;

        std::string _fontPath;
        f32 _fontSize;
        Renderer::Font* _font;

        std::vector<Renderer::ModelID> _models;
        std::vector<Renderer::TextureID> _textures;

        Renderer::ConstantBuffer<LabelConstantBuffer>* _constantBuffer = nullptr;

        friend class ::UILabel;
        friend class UIRenderer;
    };
}