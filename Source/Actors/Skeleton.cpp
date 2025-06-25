#include "Skeleton.h"
#include "Player.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/ColliderComponents/AABBColliderComponent.h"
#include "../Components/RigidBodyComponent.h"

Skeleton::Skeleton(Game* game, Player* target)
        : Actor(game)
        , mTarget(target)
        , mSpeed(120.0f)
        , mIsDying(false)
{
    SetScale(4.0f);

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

    // Colisor
    mColliderComponent = new AABBColliderComponent(this, 0, 0, 32, 32, ColliderLayer::Enemy, false);
}

void Skeleton::OnUpdate(float deltaTime)
{
    if (mIsDying || !mTarget) return;

    Vector2 toPlayer = mTarget->GetPosition() - GetPosition();
    if (toPlayer.Length() > 1.0f)
    {
        toPlayer.Normalize();
        SetPosition(GetPosition() + toPlayer * mSpeed * deltaTime);

        if (std::abs(toPlayer.x) > std::abs(toPlayer.y))
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
    }
    else
    {
        mDrawComponent->SetAnimation("IdleDown");
    }
}

void Skeleton::OnHorizontalCollision(float, AABBColliderComponent* other)
{
    if (mIsDying) return;

    if (other->GetLayer() == ColliderLayer::PlayerAttack)
    {
        SDL_Log("entrou horizontal");
        Die();
    }
    else if (other->GetLayer() == ColliderLayer::Player)
    {
        mTarget->Kill();
    }
}

void Skeleton::OnVerticalCollision(float, AABBColliderComponent* other)
{
    OnHorizontalCollision(0.0f, other);
}

void Skeleton::Die()
{
    if (mIsDying) return;
    mIsDying = true;

    mDrawComponent->SetAnimation("Dead");
    mColliderComponent->SetEnabled(true);

    GetGame()->AddActor(new Actor(GetGame()));
    SetState(ActorState::Destroy);
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