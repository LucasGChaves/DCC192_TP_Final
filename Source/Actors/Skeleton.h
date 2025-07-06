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

    std::pair<int,int> GetGoalTile(
    const AABBColliderComponent* playerBB,
    const std::vector<std::vector<bool>>& passable2x2,
    int maxRows, int maxCols);



private:
    std::vector<int> GetAnimationFramesByNamePrefix(const std::string& prefix, int frameCount);
    bool InsideFOV(Vector2 toPlayer);
    bool HasLineOfSight();
    void Wander(float deltatime);
    void Chase(float deltatime);
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
    float mRepathTimer = 0.0f;
    State mState = State::Wander;

    class DrawAnimatedComponent* mDrawComponent;
    class AABBColliderComponent* mColliderComponent;
    class RigidBodyComponent* mRigidBodyComponent;
};