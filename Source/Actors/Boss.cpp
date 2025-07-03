#include "Boss.h"

Boss::Boss(Game* game, Player* target, Vector2 pos)
        : Actor(game)
        , mTarget(target)
        , mSpeed(120.0f)
        , mIsDying(false)
{
    SetPosition(pos);
    SetScale(Game::SCALE);

    SDL_Log("Position Before: %f %f", mPosition.x, mPosition.y);
    float fullPx = Game::TILE_SIZE * 4 * Game::SCALE;
    Vector2 centerFull = pos + Vector2{ fullPx * 0.5f, fullPx * 0.5f };

    mDrawComponent = new DrawBossAnimatedComponent(this,
        "../Assets/Sprites/Boss/boss.png", "../Assets/Sprites/Boss/boss.json",
        Vector2(Game::TILE_SIZE * Game::SCALE * 4, Game::TILE_SIZE * Game::SCALE * 4), 0);
    mDrawComponent->SetUpdateOrder(0);
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

    mDrawComponent->SetAnimation("LooSpAttack");
    mDrawComponent->SetAnimFPS(10.0f);

    Vector2 frameInitSz = mDrawComponent->GetFrameSize(106);
    // e recua mPosition para alinhar o canto do trim ao centerFull
    Vector2 newPos = centerFull - (frameInitSz * 0.5f);
    SetPosition(newPos);

    // --- passo 4: cria RigidBody e AABB sem offset interno (dx=dy=0) ---
    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 0.0f, false, 1);
    mColliderComponent = new AABBColliderComponent(
        this,0,0, static_cast<int>(std::round(frameInitSz.x)), static_cast<int>(std::round(frameInitSz.y)),
        ColliderLayer::Enemy, false,2);

}

void Boss::OnUpdate(float deltaTime) {
    //SDL_Log("TEST");
    //mRigidBodyComponent->SetVelocity(Vector2{-25.f, 0.f});
    //SetPosition(GetPosition() + Vector2{-1.f, 0.f});
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
