#pragma once
#include "Widget.h"
#include <Renderer/Descriptors/TextureDesc.h>

namespace UI
{
    class Panel : Widget
    {
    public:
        Panel() : Widget() { }


    private:
        Renderer::TextureID GetTexture();
        void SetTexture(Renderer::TextureID texture);

        Vector4 GetColor();
        void SetColor(Vector4 color);

    private:
        Renderer::TextureID _texture;
        Vector4 _color;


        friend class ::UIPanel;
    };
}