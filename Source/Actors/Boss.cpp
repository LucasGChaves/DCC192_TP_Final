#include "Boss.h"

Boss::Boss(Game* game, Player* target, Vector2 pos)
        : Actor(game)
        , mTarget(target)
        , mSpeed(120.0f)
        , mIsDying(false)
{
    SetPosition(pos);
    SetScale(Game::SCALE);

    mDrawComponent = new DrawBossAnimatedComponent(this,
        "../Assets/Sprites/Boss/boss.png", "../Assets/Sprites/Boss/boss.json",
        Vector2(Game::TILE_SIZE * Game::SCALE * 4, Game::TILE_SIZE * Game::SCALE * 4));

    mDrawComponent->AddAnimation("IdleDown", {0, 1, 2, 3, 4, 5});
    mDrawComponent->AddAnimation("IdleSide", {8, 9, 10, 11, 12, 13});
    mDrawComponent->AddAnimation("IdleUp", {16, 17, 18, 19, 20, 21});
    mDrawComponent->AddAnimation("WalkDown", {24, 25, 26, 27, 28, 29});
    mDrawComponent->AddAnimation("WalkSide", {32, 33, 34, 35, 36, 37});
    mDrawComponent->AddAnimation("WalkUp", {40, 41, 42, 43, 44, 45});
    mDrawComponent->AddAnimation("StrikeDown", {48, 49, 50, 51, 52, 53, 54, 55});
    mDrawComponent->AddAnimation("StrikeSide", {56, 57, 58, 59, 60, 61, 62, 63});
    mDrawComponent->AddAnimation("StrikeUp", {64, 65, 66, 67, 68, 69, 70, 71});
    mDrawComponent->AddAnimation("HurtDown", {72, 73, 74, 75});
    mDrawComponent->AddAnimation("HurtSide", {80, 81, 82, 83});
    mDrawComponent->AddAnimation("HurtUp", {88, 89, 90, 91});
    mDrawComponent->AddAnimation("Dead", {96, 97, 98, 99});
    mDrawComponent->AddAnimation("BeginSpAttack", {104});
    mDrawComponent->AddAnimation("LooSpAttack", {106, 107, 108, 109});

    mDrawComponent->SetAnimation("IdleDown");
    mDrawComponent->SetAnimFPS(10.0f);

    Vector2 InitialFrameSizeScaled = mDrawComponent->GetFrameSize(0);
    Vector2 offset{
        ((Game::TILE_SIZE * Game::SCALE * 4) - InitialFrameSizeScaled.x) * 0.5f,
        ((Game::TILE_SIZE * Game::SCALE * 4) - InitialFrameSizeScaled.y) * 0.5f
    };

    mColliderComponent = new AABBColliderComponent(this, offset.x, offset.y,
        InitialFrameSizeScaled.x, InitialFrameSizeScaled.y, ColliderLayer::Enemy, false, 1);
}

void Boss::OnUpdate(float deltaTime) {
    // if (mIsDying) {
    //     mDeathTimer -= deltaTime;
    //     if (mDeathTimer <= 0.0f) {
    //         SetState(ActorState::Destroy);
    //     }
    //     return;
    // }
    //
    // if (mIsDying || !mTarget) return;
}

void Boss::OnHorizontalCollision(float overlap, class AABBColliderComponent* other) {};
void Boss::OnVerticalCollision(float overlap, class AABBColliderComponent* other) {};
