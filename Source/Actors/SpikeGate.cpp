//
// Created by Lucas on 26/06/2025.
//
#include "SpikeGate.h"

#include "Boss.h"
#include "Player.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"


SpikeGate::SpikeGate(Game* game, Vector2 pos, int width, int height, int drawOrder)
    : Actor(game) {

    const int maxFrameHeight = 32; // Altura máxima (em pixels) de um frame da animação
    const int scaledHeight = static_cast<int>(maxFrameHeight * Game::SCALE);

    pos.y -= scaledHeight;
    SetPosition(pos);
    SetScale(Game::SCALE);

    mColliderComponent = new AABBColliderComponent(this, 0, 0, width, height, ColliderLayer::Blocks, true);
    mDrawComponent = new DrawAnimatedComponent(this, "../Assets/Sprites/SpikeGate/texture.png", "../Assets/Sprites/SpikeGate/texture.json", 2+drawOrder);

    mDrawComponent->AddAnimation("Raised", {0});
    mDrawComponent->AddAnimation("Lowered", {47});
    mDrawComponent->AddAnimation("Lowering",
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
            16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
            30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47});

    mDrawComponent->SetAnimation("Raised");
    mDrawComponent->SetAnimFPS(20.0f);
}

void SpikeGate::OnUpdate(float deltaTime)
{
    if (!mGame->GetPlayer()) return;
    if (mGame->GetPlayer()->GetScore() != mGame->GetNumSkeletons()) return;
    if (mGame->GetBoss() && mGame->GetGameScene() == Game::GameScene::Level3 && !mGame->GetBoss()->IsDying()) return;

    std::string animationName = mDrawComponent->GetAnimationName();

    if (animationName != "Lowering" && animationName != "Lowered") {
        mDrawComponent->SetAnimation("Lowering");
    }
    if (animationName == "Lowering" && mDrawComponent->IsAnimationFinished())
    {
        mDrawComponent->SetAnimation("Lowered");
        mColliderComponent->SetEnabled(false);
        mGame->SetSpikeGateLowered(true);
    }
}