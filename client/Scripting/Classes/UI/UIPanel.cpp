#include "UIPanel.h"
#include "../../ScriptEngine.h"

std::vector<UIPanel*> UIPanel::_panels;

void UIPanel::RegisterType()
{
    i32 r = ScriptEngine::RegisterScriptClass("UIPanel", 0, asOBJ_REF | asOBJ_NOCOUNT);
    assert(r >= 0);
    {
        r = ScriptEngine::RegisterScriptInheritance<UIWidget, UIPanel>("UIWidget");
        r = ScriptEngine::RegisterScriptFunction("UIPanel@ CreatePanel()", asFUNCTION(UIPanel::Create)); assert(r >= 0);
        r = ScriptEngine::RegisterScriptClassFunction("void SetColor(float r, float g, float b, float a)", asMETHOD(UIPanel, SetColor)); assert(r >= 0);
    }
}

std::string UIPanel::GetTypeName()
{
    return "UIPanel";
}

void UIPanel::SetColor(f32 r, f32 g, f32 b, f32 a)
{
    _panel.SetColor(Vector4(r, g, b, a));
}
