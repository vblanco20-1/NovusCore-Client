#pragma once
#include <NovusTypes.h>
#include "UIWidget.h"
#include "../../../UI/Widget/Panel.h"

class UIPanel : public UIWidget
{
public:
    UIPanel() : UIWidget(), _panel() {}
    static void RegisterType();

    std::string GetTypeName() override;
    void asd(f32 r, f32 g, f32 b, f32 a);

private:
    static UIPanel* Create()
    {
        UIPanel* panel = new UIPanel();
        _panels.push_back(panel);

        return panel;
    }

private:
    UI::Panel _panel;
    static std::vector<UIPanel*> _panels;
};