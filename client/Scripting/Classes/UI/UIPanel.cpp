#include "UIPanel.h"
#include "../../ScriptEngine.h"

UIPanel::UIPanel(f32 posX, f32 posY, f32 width, f32 height)
    : UIWidget()
    , _panel(posX, posY, width, height)
{

}

void UIPanel::RegisterType()
{
    i32 r = ScriptEngine::RegisterScriptClass("UIPanel", 0, asOBJ_REF | asOBJ_NOCOUNT);
    assert(r >= 0);
    {
        r = ScriptEngine::RegisterScriptInheritance<UIWidget, UIPanel>("UIWidget");
        r = ScriptEngine::RegisterScriptFunction("UIPanel@ CreatePanel(float xPos = 0, float yPos = 0, float width = 100, float height = 100)", asFUNCTION(UIPanel::CreatePanel)); assert(r >= 0);
        r = ScriptEngine::RegisterScriptClassFunction("void SetColor(float r, float g, float b, float a)", asMETHOD(UIPanel, SetColor)); assert(r >= 0);
        r = ScriptEngine::RegisterScriptClassFunction("void SetTexture(string texture)", asMETHOD(UIPanel, SetTexture)); assert(r >= 0);
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

void UIPanel::SetTexture(std::string& texture)
{
    _panel.SetTexture(texture);
}

UIPanel* UIPanel::CreatePanel(f32 posX, f32 posY, f32 width, f32 height)
{
    UIPanel* panel = new UIPanel(posX, posY, width, height);

    return panel;
}