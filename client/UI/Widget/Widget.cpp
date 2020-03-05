#include "Widget.h"

namespace UI
{
    // Public
    Widget::Widget(f32 posX, f32 posY, f32 width, f32 height)
        : _position(posX, posY, 0)
        , _size(width, height)
        , _parent(nullptr)
        , _children(16)
        , _isDirty(true) 
    {

    }

    Vector3 Widget::GetPosition() { return _position; }
    void Widget::SetPosition(Vector3 position) { _position = position; }
    Vector2 Widget::GetSize() { return _size; }
    void Widget::SetSize(Vector2 size) { _size = size; }
    Vector2 Widget::GetAnchor() { return _anchor; }
    void Widget::SetAnchor(Vector2 anchor) { _anchor = anchor; }

    Widget* Widget::GetParent() { return _parent; }
    void Widget::SetParent(Widget* widget)
    {
        if (_parent)
        {
            _parent->RemoveChild(this);
        }

        _parent = widget;
        _parent->AddChild(_parent);
        _parent->SetDirty();
    }

    const std::vector<Widget*>& Widget::GetChildren() { return _children; }

    bool Widget::IsDirty()
    {
        return _isDirty;
    }
    void Widget::SetDirty()
    {
        _isDirty = true;
    }

    // Private
    void Widget::AddChild(Widget* child)
    {
        _children.push_back(child);
    }
    void Widget::RemoveChild(Widget* child)
    {
        auto iterator = std::find(_children.begin(), _children.end(), child);
        assert(iterator != _children.end());

        _children.erase(iterator);
    }
    void Widget::ResetDirty()
    {
        _isDirty = false;
    }
}