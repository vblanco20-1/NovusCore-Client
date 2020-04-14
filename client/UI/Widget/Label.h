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
            Color textColor; // 16 bytes
            Color outlineColor; // 16 bytes
            f32 outlineWidth; // 4 bytes

            u8 padding[220] = {};
        };

    public:
        Label(f32 posX, f32 posY, f32 width, f32 height);

    private:
        std::string& GetText();
        void SetText(std::string& text);

        u32 GetTextLength();

        const Color& GetColor();
        void SetColor(const Color& color);

        f32 GetOutlineWidth();
        void SetOutlineWidth(f32 width);

        const Color& GetOutlineColor();
        void SetOutlineColor(const Color& color);

        void SetFont(std::string& fontPath, f32 fontSize);
        std::string& GetFontPath();
        f32 GetFontSize();

        Renderer::ConstantBuffer<LabelConstantBuffer>* GetConstantBuffer() { return _constantBuffer; }
        void SetConstantBuffer(Renderer::ConstantBuffer<LabelConstantBuffer>* constantBuffer) { _constantBuffer = constantBuffer; }

    private:
        std::string _text;
        Color _color = Color(1,1,1,1);
        Color _outlineColor = Color(0, 0, 0, 1);
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