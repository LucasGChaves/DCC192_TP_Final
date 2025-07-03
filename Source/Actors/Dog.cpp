#include "Dog.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/ColliderComponents/AABBColliderComponent.h"
#include "../Random.h"
#include <SDL_log.h>
#include <cmath>

#include "Player.h"

Dog::Dog(Game* game, Vector2 pos)
    : Actor(game)
{
    SetPosition(pos);
    SetScale(Game::SCALE);

    mDrawComponent = new DrawAnimatedComponent(this, "../Assets/Sprites/Dog/dog.png", "../Assets/Sprites/Dog/dog.json", 6);
    mDrawComponent->AddAnimation("IdleDown", GetAnimationFramesByNamePrefix("idleDown", 4));
    mDrawComponent->AddAnimation("IdleLeft", GetAnimationFramesByNamePrefix("idleLeft", 4));
    mDrawComponent->AddAnimation("IdleRight", GetAnimationFramesByNamePrefix("idleRight", 4));
    mDrawComponent->AddAnimation("IdleUp", GetAnimationFramesByNamePrefix("idleUp", 4));
    mDrawComponent->AddAnimation("WalkDown", GetAnimationFramesByNamePrefix("walkDown", 4));
    mDrawComponent->AddAnimation("WalkLeft", GetAnimationFramesByNamePrefix("walkLeft", 4));
    mDrawComponent->AddAnimation("WalkRight", GetAnimationFramesByNamePrefix("walkRight", 4));
    mDrawComponent->AddAnimation("WalkUp", GetAnimationFramesByNamePrefix("walkUp", 4));

    mDrawComponent->SetAnimation("IdleDown");
    mDrawComponent->SetAnimFPS(10.0f);
}

void Dog::SetOwner(Actor* owner)
{
    mOwner = owner;
}

Actor* Dog::GetOwner() const
{
    return mOwner;
}

void Dog::SetState(State newState)
{
    mState = newState;
}

Dog::State Dog::GetState() const
{
    return mState;
}

void Dog::OnUpdate(float dt)
{
    if (mIsDying) return;

    if (mGame->GetSpikeGateLowered() && mGame->GetGameScene() != Game::GameScene::Level1) {
        SetState(State::Follow);
    }

    switch (mState) {
        case State::Wander: UpdateWander(dt); break;
        case State::Follow: UpdateFollow(dt); break;
        case State::Idle: UpdateIdle(dt); break;
        default: break;
    }
}

void Dog::UpdateWander(float dt)
{
    Vector2 pos = GetPosition();
    pos.y -= mSpeed * dt;
    SetPosition(pos);
    mDrawComponent->SetAnimation("WalkUp");

    if (pos.y + 32 < 0) {
        Actor::SetState(ActorState::Destroy);
        SetState(State::Dying);
        SDL_Log("[Dog] Destroyed (wander exit)");
    }
}

void Dog::UpdateFollow(float dt)
{
    if (!mOwner) {
        mState = State::Wander;
        return;
    }

    Vector2 toOwner = mOwner->GetPosition() - GetPosition();
    mDistanceWithOwner = toOwner.Length();

    if (mDistanceWithOwner > mMaxFollowDistance) {
        toOwner.Normalize();
        SetPosition(GetPosition() + toOwner * mSpeed * dt);
        mDrawComponent->SetAnimation("Walk" + GetDirectionFromVector(toOwner));
    }
    else {
        mChangeDirTimer -= dt;
        if (mChangeDirTimer <= 0.0f) {
            float angle = Random::GetFloatRange(0.0f, 2 * Math::Pi);
            mRandomDir = Vector2(std::cos(angle), std::sin(angle));
            mChangeDirTimer = Random::GetFloatRange(0.5f, 1.5f);
        }

        Vector2 newPos = GetPosition() + mRandomDir * mSpeed * 0.5f * dt;

        if ((newPos - mOwner->GetPosition()).Length() < mMaxFollowDistance)
            SetPosition(newPos);

        mDrawComponent->SetAnimation("Walk" + GetDirectionFromVector(mRandomDir));
    }
}

void Dog::UpdateIdle(float dt)
{
    mDrawComponent->SetAnimation("IdleDown");
    // Nada de movimento, ele permanece na posição atual
}

std::string Dog::GetDirectionFromVector(const Vector2& dir) const
{
    if (std::fabs(dir.x) > std::fabs(dir.y))
        return dir.x > 0 ? "Right" : "Left";
    else
        return dir.y > 0 ? "Down" : "Up";
}

std::vector<int> Dog::GetAnimationFramesByNamePrefix(const std::string& prefix, int count)
{
    std::vector<int> frames;
    const auto& all = mDrawComponent->GetFrameNames();
    for (size_t i = 0; i < all.size(); ++i) {
        if (all[i].find(prefix) != std::string::npos)
            frames.emplace_back(static_cast<int>(i));
    }
    std::sort(frames.begin(), frames.end());
    if ((int)frames.size() > count) frames.resize(count);
    return frames;
}
