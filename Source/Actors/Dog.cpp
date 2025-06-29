#include "Dog.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/ColliderComponents/AABBColliderComponent.h"
#include "../Components/RigidBodyComponent.h"
#include <SDL_log.h>
#include "../Random.h"

Dog::Dog(Game* game)
    : Actor(game)
    , mSpeed(100.0f)
    , mIsDying(false)
{
    SetScale(4.0f);
    try {
        mDrawComponent = new DrawAnimatedComponent(this,
            "../Assets/Sprites/Dog/dog.png",
            "../Assets/Sprites/Dog/dog.json");
    } catch (const std::exception& e) {
        SDL_Log("[Dog] Failed to load DrawAnimatedComponent or parse texture.json: %s", e.what());
        mDrawComponent = nullptr;
    }
    if (!mDrawComponent) {
        SDL_Log("[Dog] mDrawComponent is null. Dog animations will not be loaded.");
        return;
    }
    try {
        mDrawComponent->AddAnimation("IdleDown", GetAnimationFramesByNamePrefix("idleDown", 4));
        mDrawComponent->AddAnimation("IdleLeft", GetAnimationFramesByNamePrefix("idleLeft", 4));
        mDrawComponent->AddAnimation("IdleRight", GetAnimationFramesByNamePrefix("idleRight", 4));
        mDrawComponent->AddAnimation("IdleUp", GetAnimationFramesByNamePrefix("idleTop", 4));
        mDrawComponent->AddAnimation("WalkDown", GetAnimationFramesByNamePrefix("walkDown", 4));
        mDrawComponent->AddAnimation("WalkLeft", GetAnimationFramesByNamePrefix("walkLeft", 4));
        mDrawComponent->AddAnimation("WalkRight", GetAnimationFramesByNamePrefix("walkRight", 4));
        mDrawComponent->AddAnimation("WalkUp", GetAnimationFramesByNamePrefix("walkUp", 4));
        mDrawComponent->SetAnimation("IdleDown");
        mDrawComponent->SetAnimFPS(10.0f);
    } catch (const std::exception& e) {
        SDL_Log("[Dog] Failed to add animations: %s", e.what());
    }
    mColliderComponent = new AABBColliderComponent(this, 0, 0, 32, 32, ColliderLayer::Enemy, false);
}

void Dog::SetFollowTarget(Actor* target, float maxDistance) {
    mFollowTarget = target;
    mMaxFollowDistance = maxDistance;
}

void Dog::StartCircleAround(Actor* target, float radius, float speed) {
    mFollowTarget = target;
    mCircleMode = true;
    mCircleRadius = radius;
    mCircleAngularSpeed = speed;
    // Start at a random angle
    mCircleAngle = Random::GetFloatRange(0, 2 * Math::Pi);
}

void Dog::OnUpdate(float deltaTime)
{
    if (mIsDying) return;
    if (mCircleMode && mFollowTarget) {
        mCircleAngle += mCircleAngularSpeed * deltaTime;
        if (mCircleAngle > 2 * Math::Pi) mCircleAngle -= 2 * Math::Pi;
        Vector2 center = mFollowTarget->GetPosition();
        Vector2 offset(cos(mCircleAngle) * mCircleRadius, sin(mCircleAngle) * mCircleRadius);
        SetPosition(center + offset);

        float deg = mCircleAngle * 180.0f / Math::Pi;
        std::string dir;
        if (deg > 45 && deg <= 135) dir = "Down";
        else if (deg > 135 && deg <= 225) dir = "Left";
        else if (deg > 225 && deg <= 315) dir = "Up";
        else dir = "Right";
        mDrawComponent->SetAnimation("Walk" + dir);
        return;
    }
    if (mFollowTarget) {

        Vector2 toTarget = mFollowTarget->GetPosition() - GetPosition();
        float dist = toTarget.Length();
        if (dist > mMaxFollowDistance) {
            toTarget.Normalize();
            SetPosition(GetPosition() + toTarget * mSpeed * deltaTime);
            mDrawComponent->SetAnimation("Walk" + std::string((fabs(toTarget.x) > fabs(toTarget.y)) ? (toTarget.x > 0 ? "Right" : "Left") : (toTarget.y > 0 ? "Down" : "Up")));
        } else {

            static float changeDirTimer = 0.0f;
            static Vector2 randomDir(0, -1);
            changeDirTimer -= deltaTime;
            if (changeDirTimer <= 0.0f) {
                float angle = Random::GetFloatRange(0, 2 * Math::Pi);
                randomDir = Vector2(cos(angle), sin(angle));
                changeDirTimer = Random::GetFloatRange(0.5f, 1.5f);
            }
            Vector2 newPos = GetPosition() + randomDir * mSpeed * 0.5f * deltaTime;

            if ((newPos - mFollowTarget->GetPosition()).Length() < mMaxFollowDistance) {
                SetPosition(newPos);
            }
            mDrawComponent->SetAnimation("Walk" + std::string((fabs(randomDir.x) > fabs(randomDir.y)) ? (randomDir.x > 0 ? "Right" : "Left") : (randomDir.y > 0 ? "Down" : "Up")));
        }
        return;
    }
    if (mIsDying) return;
    Vector2 pos = GetPosition();
    pos.y -= mSpeed * deltaTime;
    SetPosition(pos);
    mDrawComponent->SetAnimation("WalkUp");
    if (pos.y + 32 < 0) {
        SetState(ActorState::Destroy);
    }
}

std::vector<int> Dog::GetAnimationFramesByNamePrefix(const std::string& prefix, int frameCount)
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
