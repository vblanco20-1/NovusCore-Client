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
        Widget(const vec2& pos, const vec2& size);

        const vec3& GetPosition();
        void SetPosition(const vec3& position);

        const vec2& GetSize();
        void SetSize(const vec2& size);

        const vec2& GetAnchor();
        void SetAnchor(const vec2& anchor);

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
        vec3 _position;
        vec2 _size;
        vec2 _anchor;
        Widget* _parent;
        std::vector<Widget*> _children;
        bool _isDirty;

        friend class UIRenderer;
    };
}