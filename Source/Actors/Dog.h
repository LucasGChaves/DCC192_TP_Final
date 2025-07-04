#pragma once
#include "Actor.h"
#include <string>
#include <vector>

class DrawAnimatedComponent;
class AABBColliderComponent;

class Dog : public Actor {
public:
    enum class State {
        Wander,
        Follow,
        Idle,
        Dying
    };

    explicit Dog(class Game* game, Vector2 pos);

    void OnUpdate(float deltaTime) override;

    void SetOwner(Actor* owner);
    Actor* GetOwner() const;

    void SetState(State newState);
    State GetState() const;

    void SetFollowDistance(float distance) { mMaxFollowDistance = distance; }
    void SetSpeed(float speed) { mSpeed = speed; }

    float GetDistanceWithOwner() const { return mDistanceWithOwner; }

    //AABBColliderComponent* GetColliderComponent() const { return mColliderComponent; }

private:
    DrawAnimatedComponent* mDrawComponent;
    AABBColliderComponent* mColliderComponent;

    Actor* mOwner = nullptr;
    State mState = State::Wander;

    float mSpeed = 100.0f;
    float mMaxFollowDistance = 20.0f;
    bool mIsDying = false;
    float mDeathTimer = -1.0f;

    float mChangeDirTimer = 0.0f;
    Vector2 mRandomDir = Vector2(0, -1);

    float mDistanceWithOwner = 0.f;

    void UpdateWander(float dt);
    void UpdateFollow(float dt);
    void UpdateIdle(float dt);

    std::string GetDirectionFromVector(const Vector2& dir) const;
    std::vector<int> GetAnimationFramesByNamePrefix(const std::string& prefix, int frameCount);
};