#include "Skeleton.h"
#include "Player.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/ColliderComponents/AABBColliderComponent.h"
#include "../Components/RigidBodyComponent.h"

Skeleton::Skeleton(Game* game, Player* target, Vector2 pos)
        : Actor(game)
        , mTarget(target)
        , mSpeed(50.0f)
        , mIsDying(false)
{
    mIsLocked = false;
    SetPosition(pos);
    SetScale(Game::SCALE);

    // Draw e animações
    mDrawComponent = new DrawAnimatedComponent(this,
                                               "../Assets/Sprites/Skeleton/skeleton.png",
                                               "../Assets/Sprites/Skeleton/skeleton.json");

    mDrawComponent->AddAnimation("IdleDown", GetAnimationFramesByNamePrefix("Idle_down", 6));
    mDrawComponent->AddAnimation("IdleUp", GetAnimationFramesByNamePrefix("Idle_up", 6));
    mDrawComponent->AddAnimation("IdleSide", GetAnimationFramesByNamePrefix("Idle_horizontal", 6));
    mDrawComponent->AddAnimation("WalkDown", GetAnimationFramesByNamePrefix("Walk_down", 6));
    mDrawComponent->AddAnimation("WalkUp", GetAnimationFramesByNamePrefix("Walk_up", 6));
    mDrawComponent->AddAnimation("WalkSide", GetAnimationFramesByNamePrefix("Walk_horizontal", 6));
    mDrawComponent->AddAnimation("Dead", GetAnimationFramesByNamePrefix("Death", 5));

    mDrawComponent->SetAnimation("IdleDown");
    mDrawComponent->SetAnimFPS(10.0f);

    auto [dx, dy, w, h] = ComputeColliderParams(Game::TILE_SIZE * 2, Game::TILE_SIZE * 2);

    mRigidBodyComponent = new RigidBodyComponent(this);

    // Colisor
    mColliderComponent = new AABBColliderComponent(this, dx, dy, w, h, ColliderLayer::Enemy, false);
}

void Skeleton::OnUpdate(float deltaTime)
{
    if (mIsDying) {
        mDeathTimer -= deltaTime;
        if (mDeathTimer <= 0.0f) {
            SetState(ActorState::Destroy);
        }
        return;
    }

    if (mIsDying || !mTarget) return;

    Vector2 toPlayer = mTarget->GetPosition() - GetPosition();
    Vector2 vel = Vector2::Zero;

    if (toPlayer.Length() > 1.0f)
    {
        toPlayer.Normalize();

        vel = toPlayer * mSpeed;

        float diff = std::abs(std::abs(toPlayer.x) - std::abs(toPlayer.y));

        if (diff >= 0.f && diff <= 0.5f) {
            SetRotation(toPlayer.x > 0 ? 0.0f : Math::Pi);
            mDrawComponent->SetAnimation("WalkSide");
        }

        else if (std::abs(toPlayer.x) > std::abs(toPlayer.y))
        {
            SetRotation(toPlayer.x > 0 ? 0.0f : Math::Pi);
            mDrawComponent->SetAnimation("WalkSide");
        }
        else if (toPlayer.y < 0)
        {
            mDrawComponent->SetAnimation("WalkUp");
        }
        else
        {
            mDrawComponent->SetAnimation("WalkDown");
        }

        mStepTimer -= deltaTime;
        if (mStepTimer <= 0.0f)
        {
            mStepTimer = 0.2f;
        }
        mRigidBodyComponent->SetVelocity(vel);
    }
    else
    {
        mDrawComponent->SetAnimation("IdleDown");
    }
}

void Skeleton::OnHorizontalCollision(float overlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Player) {
        if (auto *player = dynamic_cast<Player*>(other->GetOwner())) player->Hit();
    }
}

void Skeleton::OnVerticalCollision(float overlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Player) {
        if (auto *player = dynamic_cast<Player*>(other->GetOwner())) player->Hit();
    }
}

void Skeleton::Die()
{
    if (mIsDying) return;
    mIsDying = true;
    mDrawComponent->SetAnimation("Dead");
    mColliderComponent->SetEnabled(false); 
    mDeathTimer = 0.5f; 

    if (mTarget) {
        mTarget->AddScore(1);
    }
}

std::vector<int> Skeleton::GetAnimationFramesByNamePrefix(const std::string& prefix, int frameCount)
{
    std::vector<int> frames;
    const auto& names = mDrawComponent->GetFrameNames();
    for (size_t i = 0; i < names.size(); ++i)
    {
        if (names[i].find(prefix) != std::string::npos)
            frames.emplace_back(static_cast<int>(i));
    }
    std::sort(frames.begin(), frames.end());
    if (frames.size() > static_cast<size_t>(frameCount))
        frames.resize(frameCount);
    return frames;
}