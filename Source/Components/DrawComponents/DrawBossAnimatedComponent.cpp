//
// Created by Lucas on 01/07/2025.
//

#include "DrawBossAnimatedComponent.h"

DrawBossAnimatedComponent::DrawBossAnimatedComponent(class Actor* owner, const std::string& spriteSheetPath,
                              const std::string& spriteSheetData, int drawOrder, int updateOrder)
    : DrawAnimatedComponent(owner, spriteSheetPath, spriteSheetData, drawOrder) {}

DrawBossAnimatedComponent::~DrawBossAnimatedComponent() {}

void DrawBossAnimatedComponent::Draw(SDL_Renderer* renderer, const Vector3& modColor)
{
    if (!mIsVisible) return;
    if (mSpriteSheetData.empty() || mAnimations.find(mAnimName) == mAnimations.end()) return;

    int frameIndex = static_cast<int>(mAnimTimer);
    const auto& frames = mAnimations[mAnimName];
    if (frameIndex >= static_cast<int>(frames.size())) return;

    SDL_Rect* srcRect = mSpriteSheetData[frames[frameIndex]];
    SDL_Rect* dstRect = nullptr;

    if (mAnimName == "LoopSpAttack") {
        Vector2 frameSizeScaled {static_cast<float>(srcRect->w * Game::SCALE),
        static_cast<float>(srcRect->h * Game::SCALE)};


        Vector2 center{mOwner->GetPosition().x + (mDefaultFrameSize.x * 0.5f), mOwner->GetPosition().y + (mDefaultFrameSize.y * 0.5f)};

        Vector2 framePos = center - (frameSizeScaled * 0.5f);

        SDL_Rect rect = {
            static_cast<int>(framePos.x - mOwner->GetGame()->GetCameraPos().x),
            static_cast<int>(framePos.y - mOwner->GetGame()->GetCameraPos().y),
            static_cast<int>(frameSizeScaled.x),
            static_cast<int>(frameSizeScaled.y)
        };
        dstRect = &rect;
    }
    else {
        SDL_Rect rect = {
            static_cast<int>(mOwner->GetPosition().x - mOwner->GetGame()->GetCameraPos().x),
            static_cast<int>(mOwner->GetPosition().y - mOwner->GetGame()->GetCameraPos().y),
            srcRect->w * Game::SCALE,
            srcRect->h * Game::SCALE
        };
        dstRect = &rect;
    }

    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if (mOwner->GetRotation() == Math::Pi)
        flip = SDL_FLIP_HORIZONTAL;

    SDL_SetTextureBlendMode(mSpriteSheetSurface, SDL_BLENDMODE_BLEND);
    SDL_SetTextureColorMod(mSpriteSheetSurface,
                           static_cast<Uint8>(modColor.x),
                           static_cast<Uint8>(modColor.y),
                           static_cast<Uint8>(modColor.z));

    SDL_RenderCopyEx(renderer, mSpriteSheetSurface, srcRect, dstRect, 0.0, nullptr, flip);
}

Vector2 DrawBossAnimatedComponent::GetFrameSize(int frameIdx) {
    SDL_Rect* srcRect = mSpriteSheetData[frameIdx];
    if (!srcRect) return Vector2::Zero;

    return Vector2{static_cast<float>(srcRect->w) * Game::SCALE, static_cast<float>(srcRect->h) * Game::SCALE};
}