#include "UILabel.h"
#include "../../ScriptEngine.h"

std::vector<UILabel*> UILabel::_labels;

UILabel::UILabel(f32 posX, f32 posY, f32 width, f32 height)
    : _label(posX, posY, width, height)
    , UIWidget(&_label)
{
    _labels.push_back(this);
}

void UILabel::RegisterType()
{
    i32 r = ScriptEngine::RegisterScriptClass("UILabel", 0, asOBJ_REF | asOBJ_NOCOUNT);
    assert(r >= 0);
    {
        r = ScriptEngine::RegisterScriptInheritance<UIWidget, UILabel>("UIWidget");
        r = ScriptEngine::RegisterScriptFunction("UILabel@ CreateLabel(float xPos = 0, float yPos = 0, float width = 100, float height = 100)", asFUNCTION(UILabel::CreateLabel)); assert(r >= 0);
        r = ScriptEngine::RegisterScriptClassFunction("void SetColor(float r, float g, float b)", asMETHOD(UILabel, SetColor)); assert(r >= 0);
        r = ScriptEngine::RegisterScriptClassFunction("void SetOutlineWidth(float width)", asMETHOD(UILabel, SetOutlineWidth)); assert(r >= 0);
        r = ScriptEngine::RegisterScriptClassFunction("void SetOutlineColor(float r, float g, float b)", asMETHOD(UILabel, SetOutlineColor)); assert(r >= 0);
        r = ScriptEngine::RegisterScriptClassFunction("void SetText(string texture)", asMETHOD(UILabel, SetText)); assert(r >= 0);
        r = ScriptEngine::RegisterScriptClassFunction("void SetFont(string fontPath, float fontSize)", asMETHOD(UILabel, SetFont)); assert(r >= 0);
    }
}

std::string UILabel::GetTypeName()
{
    return "UILabel";
}

void UILabel::SetColor(f32 r, f32 g, f32 b)
{
    _label.SetColor(Color(r, g, b, 1));
}

void UILabel::SetOutlineWidth(f32 width)
{
    _label.SetOutlineWidth(width);
}

void UILabel::SetOutlineColor(f32 r, f32 g, f32 b)
{
    _label.SetOutlineColor(Color(r, g, b, 1));
}

void UILabel::SetText(std::string& text)
{
    _label.SetText(text);
}

void UILabel::SetFont(std::string& fontPath, f32 fontSize)
{
    _label.SetFont(fontPath, fontSize);
}

UILabel* UILabel::CreateLabel(f32 posX, f32 posY, f32 width, f32 height)
{
    UILabel* label = new UILabel(posX, posY, width, height);

    return label;
}