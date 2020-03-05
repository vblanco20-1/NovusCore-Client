#pragma once
#include <NovusTypes.h>
#include "UIWidget.h"
#include "../../../UI/Widget/Panel.h"

class UIPanel : public UIWidget
{
public:
    UIPanel(f32 posX, f32 posY, f32 width, f32 height);
    static void RegisterType();

    std::string GetTypeName() override;
    void SetColor(f32 r, f32 g, f32 b, f32 a);

    void SetTexture(std::string& texture);

private:
    static UIPanel* CreatePanel(f32 posX, f32 posY, f32 width, f32 height);

private:
    UI::Panel _panel;
};