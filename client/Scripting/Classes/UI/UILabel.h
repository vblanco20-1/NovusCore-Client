#pragma once
#include <NovusTypes.h>
#include "UIWidget.h"
#include "../../../UI/Widget/Label.h"

class UILabel : public UIWidget
{
public:
    UILabel(f32 posX, f32 posY, f32 width, f32 height);
    static void RegisterType();

    std::string GetTypeName() override;
    void SetColor(f32 r, f32 g, f32 b);
    void SetOutlineWidth(f32 width);
    void SetOutlineColor(f32 r, f32 g, f32 b);

    void SetText(std::string& text);
    void SetFont(std::string& fontPath, f32 fontSize);

    UI::Label* GetInternal() { return &_label; }
    static std::vector<UILabel*> _labels;
private:
    static UILabel* CreateLabel(f32 posX, f32 posY, f32 width, f32 height);

private:
    UI::Label _label;
};