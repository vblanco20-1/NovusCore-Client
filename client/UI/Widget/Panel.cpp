#include "Panel.h"

namespace UI
{
    // Public
    Panel::Panel(f32 posX, f32 posY, f32 width, f32 height)
        : Widget(posX, posY, width, height)
        , _color(1.0f, 1.0f, 1.0f, 1.0f), _clickable(true), _dragable(false), _isDragging(false), _deltaDragPosition(0, 0), _didDrag(false)
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

    bool Panel::IsDragable()
    {
        return _dragable;
    }
    void Panel::SetDragable(bool value)
    {
        _dragable = value;
    }
    bool Panel::IsDragging()
    {
        return _isDragging;
    }
    void Panel::BeingDrag(Vector2 deltaDragPosition)
    {
        _deltaDragPosition = deltaDragPosition;
        _isDragging = true;
    }
    Vector2 Panel::GetDeltaDragPosition()
    {
        return _deltaDragPosition;
    }
    bool Panel::DidDrag()
    {
        return _didDrag;
    }
    void Panel::SetDidDrag()
    {
        _didDrag = true;
    }
    void Panel::EndDrag()
    {
        _deltaDragPosition = Vector2(0, 0);
        _didDrag = false;
        _isDragging = false;
    }
}