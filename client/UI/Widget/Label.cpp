#include "Label.h"

#include <algorithm>

namespace UI
{
    Label::Label(const vec2& pos, const vec2& size)
        : Widget(pos, size)
        , _color(1.0f, 1.0f, 1.0f, 1.0f)
    {

    }

    // Private
    std::string& Label::GetText()
    { 
        return _text; 
    }

    void Label::SetText(std::string& text)
    { 
        _text = text;
        SetDirty();
        _glyphCount = static_cast<u32>(std::count_if(text.begin(), text.end(), [](char c)
        {
            return c != ' ';
        }));
    }

    u32 Label::GetTextLength()
    {
        return static_cast<u32>(_text.length());
    }

    u32 Label::GetGlyphCount()
    {
        return _glyphCount;
    }

    const Color& Label::GetColor()
    { 
        return _color;
    }

    void Label::SetColor(const Color& color)
    { 
        _color = color;
        SetDirty();
    }

    f32 Label::GetOutlineWidth()
    {
        return _outlineWidth;
    }

    void Label::SetOutlineWidth(f32 width)
    {
        _outlineWidth = width;
    }

    const Color& Label::GetOutlineColor()
    {
        return _outlineColor;
    }

    void Label::SetOutlineColor(const Color& color)
    {
        _outlineColor = color;
        SetDirty();
    }

    void Label::SetFont(std::string& fontPath, f32 fontSize)
    {
        _fontPath = fontPath;
        _fontSize = fontSize;
        SetDirty();
    }

    std::string& Label::GetFontPath()
    {
        return _fontPath;
    }

    f32 Label::GetFontSize()
    {
        return _fontSize;
    }
}
