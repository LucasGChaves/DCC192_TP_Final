//
// Created by Lucas N. Ferreira on 28/05/25.
//

#include "UIImage.h"

UIImage::UIImage(const std::string &imagePath, const Vector2 &pos, const Vector2 &size, const Vector3 &color)
    : UIElement(pos, size, color),
    mTexture(nullptr)
{
    // TODO
}

UIImage::~UIImage()
{
    // TODO
}

void UIImage::Draw(SDL_Renderer* renderer, const Vector2 &screenPos)
{
    // TODO
}