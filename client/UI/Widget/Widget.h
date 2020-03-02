#pragma once
#include <NovusTypes.h>
#include <vector>
#include <assert.h>

namespace UI
{
    class Widget
    {
    public:
        Widget() : _position(0, 0, 0), _size(0, 0), _parent(nullptr), _children(16), _isDirty(false) { }

        Vector3 GetPosition();
        void SetPosition(Vector3 position);

        Vector2 GetSize();
        void SetSize(Vector2 size);

        Vector2 GetAnchor();
        void SetAnchor(Vector2 anchor);

        Widget* GetParent();
        void SetParent(Widget* widget);

        const std::vector<Widget*>& GetChildren();

        bool IsDirty();
        void SetDirty();

    private:
        void AddChild(Widget* child);
        void RemoveChild(Widget* child);
        void ResetDirty();

    private:
        Vector3 _position;
        Vector2 _size;
        Vector2 _anchor;
        Widget* _parent;
        std::vector<Widget*> _children;
        bool _isDirty;
    };
}