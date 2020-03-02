#include "Panel.h"

namespace UI
{
    // Public


    // Private
    Renderer::TextureID Panel::GetTexture() { return _texture; }
    void Panel::SetTexture(Renderer::TextureID texture) { _texture = texture; }

    Vector4 Panel::GetColor() { return _color; }
    void Panel::SetColor(Vector4 color) { _color = color; }
}