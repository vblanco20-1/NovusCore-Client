#pragma once
#include <NovusTypes.h>
#include "../../ScriptEngine.h"
#include "../../../UI/Widget/Widget.h"

class UIWidget
{
public:
    UIWidget() : _widget() {}
    static void RegisterType();

    template <class T>
    static void RegisterBase()
    {
        i32 r = ScriptEngine::RegisterScriptClassFunction("string GetTypeName()", asMETHOD(T, GetTypeName)); assert(r >= 0);
        r = ScriptEngine::RegisterScriptClassFunction("void SetPosition(float x, float y, float depth)", asMETHOD(T, SetPosition)); assert(r >= 0);
        r = ScriptEngine::RegisterScriptClassFunction("void SetSize(float width, float height)", asMETHOD(T, SetSize)); assert(r >= 0);
    }

    virtual std::string GetTypeName();
    virtual void SetPosition(f32 x, f32 y, f32 depth);
    virtual void SetSize(f32 width, f32 height);

private:
    static UIWidget* Create() 
    {
        UIWidget* widget = new UIWidget();
        _widgets.push_back(widget);

        return widget;
    }

private:
    UI::Widget _widget;

    static std::vector<UIWidget*> _widgets;
};