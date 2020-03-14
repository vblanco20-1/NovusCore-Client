#include "Panel.h"

namespace UI
{
    // Public
    Panel::Panel(f32 posX, f32 posY, f32 width, f32 height, bool clickable /*= false*/)
        : Widget(posX, posY, width, height)
        , _color(1.0f, 1.0f, 1.0f, 1.0f), _clickable(clickable)
    {
    }

    // Private
    Renderer::ModelID Panel::GetModelID()
    {
        return _modelID;
    }
    void Panel::SetModelID(Renderer::ModelID modelID)
    {
        _modelID = modelID;
    }
    
    std::string& Panel::GetTexture() 
    { 
        return _texture; 
    }
    void Panel::SetTexture(std::string& texture) 
    { 
        _texture = texture;
        SetDirty();
    }

    Renderer::TextureID Panel::GetTextureID()
    {
        return _textureID;
    }
    void Panel::SetTextureID(Renderer::TextureID textureID)
    {
        _textureID = textureID;
    }

    Vector4 Panel::GetColor() 
    { 
        return _color; 
    }
    void Panel::SetColor(Vector4 color) 
    { 
        _color = color;
        SetDirty();
    }

    bool Panel::IsClickable()
    {
        return _clickable;
    }
    void Panel::SetClickable(bool value)
    {
        _clickable = value;
    }
}