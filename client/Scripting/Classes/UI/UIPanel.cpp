#include "UIPanel.h"
#include "../../ScriptEngine.h"

std::vector<UIPanel*> UIPanel::_panels;

UIPanel::UIPanel(f32 posX, f32 posY, f32 width, f32 height)
    : _panel(posX, posY, width, height), UIWidget(&_panel), _onClickCallback(nullptr)
{
    _panels.push_back(this);
}

void UIPanel::RegisterType()
{
    i32 r = ScriptEngine::RegisterScriptClass("UIPanel", 0, asOBJ_REF | asOBJ_NOCOUNT);
    assert(r >= 0);
    {
        r = ScriptEngine::RegisterScriptInheritance<UIWidget, UIPanel>("UIWidget");
        r = ScriptEngine::RegisterScriptFunction("UIPanel@ CreatePanel(float xPos = 0, float yPos = 0, float width = 100, float height = 100, bool clickable = false)", asFUNCTION(UIPanel::CreatePanel)); assert(r >= 0);
        r = ScriptEngine::RegisterScriptClassFunction("void SetColor(float r, float g, float b, float a)", asMETHOD(UIPanel, SetColor)); assert(r >= 0);
        r = ScriptEngine::RegisterScriptClassFunction("void SetTexture(string texture)", asMETHOD(UIPanel, SetTexture)); assert(r >= 0);
        r = ScriptEngine::RegisterScriptClassFunction("void SetClickable(bool value)", asMETHOD(UIPanel, SetClickable)); assert(r >= 0);
        r = ScriptEngine::RegisterScriptClassFunction("bool IsClickable()", asMETHOD(UIPanel, IsDragable)); assert(r >= 0);
        r = ScriptEngine::RegisterScriptClassFunction("void SetDragable(bool value)", asMETHOD(UIPanel, SetDragable)); assert(r >= 0);
        r = ScriptEngine::RegisterScriptClassFunction("bool IsDragable()", asMETHOD(UIPanel, IsDragable)); assert(r >= 0);

        // Callback
        r = ScriptEngine::RegisterScriptFunctionDef("void OnClickCallback(UIPanel@ panel)"); assert(r >= 0);
        r = ScriptEngine::RegisterScriptClassFunction("void OnClick(OnClickCallback @cb)", asMETHOD(UIPanel, SetOnClick)); assert(r >= 0);
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

bool UIPanel::IsClickable()
{
    return _panel.IsClickable();
}
void UIPanel::SetClickable(bool value)
{
    _panel.SetClickable(value);
}

bool UIPanel::IsDragable()
{
    return _panel.IsDragable();
}
void UIPanel::SetDragable(bool value)
{
    _panel.SetDragable(value);
}

void UIPanel::SetOnClick(asIScriptFunction* function)
{
    _onClickCallback = function;
}
void UIPanel::OnClick()
{
    if (!_onClickCallback)
        return;

    asIScriptContext* context = ScriptEngine::GetScriptContext();
    {
        context->Prepare(_onClickCallback);
        {
            context->SetArgObject(0, this);
        }
        context->Execute();
    }
}

UI::Panel* UIPanel::GetInternal()
{
    return &_panel;
}
UIPanel* UIPanel::CreatePanel(f32 posX, f32 posY, f32 width, f32 height, bool clickable)
{
    UIPanel* panel = new UIPanel(posX, posY, width, height);
    panel->SetClickable(clickable);

    return panel;
}