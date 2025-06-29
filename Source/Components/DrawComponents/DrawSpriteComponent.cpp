//
// Created by Lucas N. Ferreira on 28/09/23.
//

#include "DrawSpriteComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include <algorithm>

DrawSpriteComponent::DrawSpriteComponent(class Actor* owner, const std::string &texturePath,
    const int width, const int height, const int drawOrder,
    bool hasSrc, Vector2 srcPos, SDL_Texture* texture)
        :DrawComponent(owner, drawOrder)
        ,mWidth(width)
        ,mHeight(height)
        ,mHasSrc(hasSrc)
        ,mSrcPos(srcPos)
        ,mSpriteSheetSurface(texture)
{
    if (!mSpriteSheetSurface) {
        mSpriteSheetSurface = mOwner->GetGame()->LoadTexture(texturePath);
    }
}

DrawSpriteComponent::~DrawSpriteComponent()
{
    DrawComponent::~DrawComponent();

    if (mSpriteSheetSurface) {
        SDL_DestroyTexture(mSpriteSheetSurface);
        mSpriteSheetSurface = nullptr;
    }
}

void DrawSpriteComponent::Draw(SDL_Renderer *renderer, const Vector3 &modColor)
{
    SDL_Rect dstRect = {
        static_cast<int>(mOwner->GetPosition().x - mOwner->GetGame()->GetCameraPos().x),
        static_cast<int>(mOwner->GetPosition().y - mOwner->GetGame()->GetCameraPos().y),
        mWidth,
        mHeight
    };

    SDL_Rect* srcRect = nullptr;

    if (mHasSrc) {
        SDL_Rect srcPos{static_cast<int>(mSrcPos.x), static_cast<int>(mSrcPos.y), mWidth/Game::SCALE, mHeight/Game::SCALE};
        srcRect = &srcPos;
    }

    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if (mOwner->GetRotation() == Math::Pi) {
        flip = SDL_FLIP_HORIZONTAL;
    }

    SDL_SetTextureBlendMode(mSpriteSheetSurface, SDL_BLENDMODE_BLEND);

    SDL_SetTextureColorMod(mSpriteSheetSurface,
                           static_cast<Uint8>(modColor.x),
                           static_cast<Uint8>(modColor.y),
                           static_cast<Uint8>(modColor.z));

    SDL_RenderCopyEx(renderer, mSpriteSheetSurface, srcRect, &dstRect, mOwner->GetRotation(), nullptr, flip);
}