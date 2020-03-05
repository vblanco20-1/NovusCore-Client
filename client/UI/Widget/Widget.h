#pragma once
#include <NovusTypes.h>
#include <vector>
#include <assert.h>

class UIRenderer;

namespace UI
{
    class Widget
    {
    public:
        Widget(f32 posX, f32 posY, f32 width, f32 height);

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

        friend class UIRenderer;
    };
}