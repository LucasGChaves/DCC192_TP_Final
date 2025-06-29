#include "Dog.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/ColliderComponents/AABBColliderComponent.h"
#include "../Components/RigidBodyComponent.h"
#include <SDL_log.h>

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

void Dog::OnUpdate(float deltaTime)
{
    if (mIsDying) return;
    // Move up
    Vector2 pos = GetPosition();
    pos.y -= mSpeed * deltaTime;
    SetPosition(pos);
    mDrawComponent->SetAnimation("WalkUp");
    // Optionally: destroy when off screen (above top)
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
