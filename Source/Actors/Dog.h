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
        Circle,
        Dying
    };

    explicit Dog(class Game* game, Vector2 pos);

    void OnUpdate(float deltaTime) override;

    // Relacionamento
    void SetOwner(Actor* owner);
    Actor* GetOwner() const;

    // Estado
    void SetState(State newState);
    State GetState() const;

    // Movimento
    void StartCircleAround(Actor* target, float radius = 100.0f, float speed = 2.0f);
    void SetFollowDistance(float distance) { mMaxFollowDistance = distance; }
    void SetSpeed(float speed) { mSpeed = speed; }

    AABBColliderComponent* GetColliderComponent() const { return mColliderComponent; }

private:
    // Componentes principais
    DrawAnimatedComponent* mDrawComponent;
    AABBColliderComponent* mColliderComponent;

    // Estado e dono
    State mState = State::Wander;
    Actor* mOwner = nullptr;

    // Comportamento
    float mSpeed = 100.0f;
    float mMaxFollowDistance = 200.0f;
    bool mIsDying = false;
    float mDeathTimer = -1.0f;

    // Modo circular
    Actor* mCircleTarget = nullptr;
    float mCircleRadius = 100.0f;
    float mCircleAngle = 0.0f;
    float mCircleAngularSpeed = 2.0f;

    // Variação
    float mChangeDirTimer = 0.0f;
    Vector2 mRandomDir = Vector2(0, -1);

    // Comportamentos
    void UpdateWander(float dt);
    void UpdateFollow(float dt);
    void UpdateCircle(float dt);

    // Utilitários
    std::string GetDirectionFromVector(const Vector2& dir) const;
    std::string GetDirectionFromAngle(float degrees) const;
    std::vector<int> GetAnimationFramesByNamePrefix(const std::string& prefix, int frameCount);
};



// #pragma once
// #include "Actor.h"
// #include <string>
// #include <vector>
//
// class DrawAnimatedComponent;
// class AABBColliderComponent;
//
// class Dog : public Actor {
// public:
//     Dog(class Game* game);
//     void OnUpdate(float deltaTime) override;
//     AABBColliderComponent* GetColliderComponent() const { return mColliderComponent; }
//     void SetFollowTarget(Actor* target, float maxDistance = 200.0f);
//     void SetSpeed(float speed) { mSpeed = speed; }
//     void StartCircleAround(Actor* target, float radius = 100.0f, float speed = 2.0f);
//
// private:
//     std::vector<int> GetAnimationFramesByNamePrefix(const std::string& prefix, int frameCount);
//     float mSpeed;
//     bool mIsDying;
//     float mDeathTimer = -1.0f;
//     DrawAnimatedComponent* mDrawComponent;
//     AABBColliderComponent* mColliderComponent;
//     Actor* mFollowTarget = nullptr;
//     float mMaxFollowDistance = 200.0f;
//
//     bool mCircleMode = false;
//     float mCircleRadius = 100.0f;
//     float mCircleAngle = 0.0f;
//     float mCircleAngularSpeed = 2.0f;
// };
