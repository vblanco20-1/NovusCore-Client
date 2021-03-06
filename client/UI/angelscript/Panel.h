#pragma once
#include <NovusTypes.h>

#include "../ECS/Components/TransformEvents.h"
#include "../ECS/Components/Image.h"
#include "BaseElement.h"

namespace UIScripting
{
    class Panel : public BaseElement
    {
    public:
        Panel();

        static void RegisterType();

        // TransformEvents Functions
        const bool IsClickable() const;
        const bool IsDraggable() const;
        const bool IsFocusable() const;
        void SetEventFlag(const UI::UITransformEventsFlags flags);
        void UnsetEventFlag(const UI::UITransformEventsFlags flags);
        void SetOnClickCallback(asIScriptFunction* callback);
        void SetOnDragCallback(asIScriptFunction* callback);
        void SetOnFocusCallback(asIScriptFunction* callback);

        // Renderable Functions
        const std::string& GetTexture() const;
        void SetTexture(const std::string& texture);

        const Color GetColor() const;
        void SetColor(const Color& color);

        static Panel* CreatePanel();
    };
}