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

private:
    std::vector<int> GetAnimationFramesByNamePrefix(const std::string& prefix, int frameCount);
    float mSpeed;
    bool mIsDying;
    float mDeathTimer = -1.0f;
    DrawAnimatedComponent* mDrawComponent;
    AABBColliderComponent* mColliderComponent;
};
