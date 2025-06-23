#pragma once

#include "Actor.h"
#include <string>
#include <vector>

class Player;
class DrawAnimatedComponent;
class AABBColliderComponent;

class Skeleton : public Actor
{
public:
    Skeleton(class Game* game, Player* target);

    void OnUpdate(float deltaTime) override;
    void OnHorizontalCollision(float, class AABBColliderComponent* other) override;
    void OnVerticalCollision(float, class AABBColliderComponent* other) override;

private:
    std::vector<int> GetAnimationFramesByNamePrefix(const std::string& prefix, int frameCount);
    void Die();

    Player* mTarget;
    float mSpeed;
    bool mIsDying;

    class DrawAnimatedComponent* mDrawComponent;
    class AABBColliderComponent* mColliderComponent;
};