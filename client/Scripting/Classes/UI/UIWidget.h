#pragma once
#include <NovusTypes.h>
#include "../../ScriptEngine.h"
#include "../../../UI/Widget/Widget.h"

class UIWidget
{
public:
    UIWidget(UI::Widget* widget)
    {
        _widget = widget;
    }
    static void RegisterType();

    template <class T>
    static void RegisterBase()
    {
        i32 r = ScriptEngine::RegisterScriptClassFunction("string GetTypeName()", asMETHOD(T, GetTypeName)); assert(r >= 0);
        r = ScriptEngine::RegisterScriptClassFunction("void SetPosition(float x, float y, float depth)", asMETHOD(T, SetPosition)); assert(r >= 0);
        r = ScriptEngine::RegisterScriptClassFunction("void SetSize(float width, float height)", asMETHOD(T, SetSize)); assert(r >= 0);
        r = ScriptEngine::RegisterScriptClassFunction("float GetPositionX()", asMETHOD(T, GetPositionX)); assert(r >= 0);
        r = ScriptEngine::RegisterScriptClassFunction("float GetPositionY()", asMETHOD(T, GetPositionY)); assert(r >= 0);
        r = ScriptEngine::RegisterScriptClassFunction("float GetDepth()", asMETHOD(T, GetDepth)); assert(r >= 0);
    }

    virtual std::string GetTypeName();
    virtual void SetPosition(f32 x, f32 y, f32 depth);
    virtual void SetSize(f32 width, f32 height);
    virtual f32 GetPositionX();
    virtual f32 GetPositionY();
    virtual f32 GetDepth();

private:
    UI::Widget* _widget;

    static std::vector<UIWidget*> _widgets;
};