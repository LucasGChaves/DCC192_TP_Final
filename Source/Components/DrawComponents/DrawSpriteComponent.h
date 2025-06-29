//
// Created by Lucas N. Ferreira on 28/09/23.
//

#pragma once
#include "DrawComponent.h"
#include <string>

class DrawSpriteComponent : public DrawComponent
{
public:
    // (Lower draw order corresponds with further back)
    DrawSpriteComponent(class Actor* owner, const std::string &texturePath,
        int width, int height, int drawOrder,
        bool hasSrc=false, Vector2 srcPos=Vector2::Zero, SDL_Texture* texture=nullptr);

    ~DrawSpriteComponent() override;

    void Draw(SDL_Renderer* renderer, const Vector3 &modColor = Color::White) override;

protected:
    // Map of textures loaded
    SDL_Texture* mSpriteSheetSurface;

    int mWidth;
    int mHeight;
    bool mHasSrc;
    Vector2 mSrcPos;
};