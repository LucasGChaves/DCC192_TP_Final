#pragma once
#include "Actor.h"
#include <string>
#include <vector>

class DrawAnimatedComponent;
class AABBColliderComponent;

class Dog : public Actor {
public:
    Dog(class Game* game);
    void OnUpdate(float deltaTime) override;
    AABBColliderComponent* GetColliderComponent() const { return mColliderComponent; }
    void SetFollowTarget(Actor* target, float maxDistance = 200.0f);
    void SetSpeed(float speed) { mSpeed = speed; }
    void StartCircleAround(Actor* target, float radius = 100.0f, float speed = 2.0f);

private:
    std::vector<int> GetAnimationFramesByNamePrefix(const std::string& prefix, int frameCount);
    float mSpeed;
    bool mIsDying;
    float mDeathTimer = -1.0f;
    DrawAnimatedComponent* mDrawComponent;
    AABBColliderComponent* mColliderComponent;
    Actor* mFollowTarget = nullptr;
    float mMaxFollowDistance = 200.0f;

    bool mCircleMode = false;
    float mCircleRadius = 100.0f;
    float mCircleAngle = 0.0f;
    float mCircleAngularSpeed = 2.0f;
};
