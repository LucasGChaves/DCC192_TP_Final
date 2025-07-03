//
// Created by Lucas on 01/07/2025.
//

#include "DrawBossAnimatedComponent.h"

DrawBossAnimatedComponent::DrawBossAnimatedComponent(class Actor* owner, const std::string& spriteSheetPath,
                              const std::string& spriteSheetData, Vector2 originalSize, int drawOrder)
    : DrawAnimatedComponent(owner, spriteSheetPath, spriteSheetData, drawOrder), mOriginalSize(originalSize) {

        // already scaled
        mOriginalFrameCenter = Vector2{ mOwner->GetPosition().x + (mOriginalSize.x/2.f),
            mOwner->GetPosition().y + (mOriginalSize.y/2.f)};
    }

DrawBossAnimatedComponent::~DrawBossAnimatedComponent() {}

void DrawBossAnimatedComponent::Draw(SDL_Renderer* renderer, const Vector3& modColor)
{
    if (!mIsVisible) return;
    if (mSpriteSheetData.empty() || mAnimations.find(mAnimName) == mAnimations.end()) return;

    int frameIndex = static_cast<int>(mAnimTimer);
    const auto& frames = mAnimations[mAnimName];
    if (frameIndex >= static_cast<int>(frames.size())) return;

    SDL_Rect* srcRect = mSpriteSheetData[frames[frameIndex]];

    if (auto bb = mOwner->GetComponent<AABBColliderComponent>()) {
        bb->SetSize(Vector2{
            static_cast<float>(srcRect->w) * Game::SCALE,
            static_cast<float>(srcRect->h) * Game::SCALE
        });
    }

    // 2) monta o dstRect partindo de mOwner->GetPosition() (canto sup-esq)
    Vector2 worldPos = mOwner->GetPosition();
    Vector2 cam      = mOwner->GetGame()->GetCameraPos();
    SDL_Rect dstRect = {
        int(worldPos.x - cam.x),
        int(worldPos.y - cam.y),
        srcRect->w * Game::SCALE,
        srcRect->h * Game::SCALE
    };

    // SDL_Rect dstRect = {
    //     static_cast<int>(mOwner->GetPosition().x - mOwner->GetGame()->GetCameraPos().x),
    //     static_cast<int>(mOwner->GetPosition().y - mOwner->GetGame()->GetCameraPos().y),
    //     srcRect->w * Game::SCALE,
    //     srcRect->h * Game::SCALE
    // };

    auto ownerBoundingBox = mOwner->GetComponent<AABBColliderComponent>();
    if (ownerBoundingBox) {
        //Colocar código aqui com o SetSize
    }

    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if (mOwner->GetRotation() == Math::Pi)
        flip = SDL_FLIP_HORIZONTAL;

    SDL_SetTextureBlendMode(mSpriteSheetSurface, SDL_BLENDMODE_BLEND);
    SDL_SetTextureColorMod(mSpriteSheetSurface,
                           static_cast<Uint8>(modColor.x),
                           static_cast<Uint8>(modColor.y),
                           static_cast<Uint8>(modColor.z));

    SDL_RenderCopyEx(renderer, mSpriteSheetSurface, srcRect, &dstRect, 0.0, nullptr, flip);
}

Vector2 DrawBossAnimatedComponent::GetFrameSize(int frameIdx) {
    SDL_Rect* srcRect = mSpriteSheetData[frameIdx];
    if (!srcRect) return Vector2::Zero;

    return Vector2{static_cast<float>(srcRect->w) * Game::SCALE, static_cast<float>(srcRect->h) * Game::SCALE};
}