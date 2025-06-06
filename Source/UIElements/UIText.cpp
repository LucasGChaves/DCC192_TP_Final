//
// Created by Lucas N. Ferreira on 22/05/25.
//

#include "UIText.h"
#include "UIFont.h"

UIText::UIText(const std::string &text, class UIFont* font, int pointSize, const unsigned wrapLength,
               const Vector2 &pos, const Vector2 &size, const Vector3 &color)
   :UIElement(pos, size, color)
   ,mFont(font)
   ,mPointSize(pointSize)
   ,mWrapLength(wrapLength)
   ,mTextTexture(nullptr)
{
    // TODO
}

UIText::~UIText()
{

}

void UIText::SetText(const std::string &text)
{
    // TODO
}

void UIText::Draw(SDL_Renderer *renderer, const Vector2 &screenPos)
{
    // TODO
}