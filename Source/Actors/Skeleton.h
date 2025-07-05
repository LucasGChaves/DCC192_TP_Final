#pragma once

#include "Math.h"
#include "../AStar.h"
#include <numeric>
#include "Actor.h"
#include <string>
#include <vector>

class Player;
class DrawAnimatedComponent;
class AABBColliderComponent;

class Skeleton : public Actor
{
public:
    enum class State {
        Wander,
        Chasing,
        Dying
    };

    Skeleton(class Game* game, Player* target, Vector2 pos);

    void OnUpdate(float deltaTime) override;
    void OnHorizontalCollision(float overlap, class AABBColliderComponent* other) override;
    void OnVerticalCollision(float overlap, class AABBColliderComponent* other) override;

    AABBColliderComponent* GetColliderComponent() const { return mColliderComponent; }
    void Die();



private:
    std::vector<int> GetAnimationFramesByNamePrefix(const std::string& prefix, int frameCount);
    bool InsideFOV(Vector2 dir, Vector2 toPlayer);
    bool HasLineOfSight();
    void Wander(float deltatime);
    void Chase(float deltatime, Vector2 toPlayer);
    void InitWander();

    Player* mTarget;
    float mSpeed;
    bool mIsDying;
    float mDeathTimer = -1.0f;
    std::vector<Vector2> mPath;
    size_t mNextWaypoint = 0;
    Vector2 mLastTargetTile;
    float mFOVAngle = Math::ToRadians(180.f);
    Vector2 mWanderDir = Vector2::Zero;
    Vector2 mCurrentDir = Vector2::Zero;
    float mWanderTimer = 0.0f;
    float mWanderInterval = 1.0f;  // trocar de direção a cada ~1s
    State mState = State::Wander;

    class DrawAnimatedComponent* mDrawComponent;
    class AABBColliderComponent* mColliderComponent;
    class RigidBodyComponent* mRigidBodyComponent;
};