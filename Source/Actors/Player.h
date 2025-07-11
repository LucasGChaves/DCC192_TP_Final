#pragma once
#include "Actor.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/ColliderComponents/AABBColliderComponent.h"
#include "../Components/RigidBodyComponent.h"
#include <SDL_mixer.h>
#include <algorithm>
#include "../Math.h"
#include "Attack.h"
#include "Dog.h"
#include "Boss.h"
#include "InvisibleWall.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../UIElements/UIGameOver.h"
#include <string>
#include <vector>

class Player : public Actor
{
public:
    explicit Player(Game* game, Vector2 pos, float forwardSpeed = 300.0f);

    void OnProcessInput(const Uint8* keyState) override;
    void OnUpdate(float deltaTime) override;
    void OnHandleKeyPress(const int key, const bool isPressed) override;

    void OnHorizontalCollision(const float minOverlap, class AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, class AABBColliderComponent* other) override;

    void Kill() override;
    void EnableCollision(bool enabled);
    void Win(class AABBColliderComponent *poleCollider);

    void Hit();
    int GetHearts() const { return mHearts; }
    int GetScore() const { return mScore; }
    void AddScore(int value) { mScore += value; }

private:
    float mDeathTimer = -1.0f;
    void ManageAnimations();
    std::vector<int> GetAnimationFramesByNamePrefix(const std::string& prefix, int frameCount);

    float mForwardSpeed;
    bool mIsRunning;
    bool mIsDying;
    std::string mLastDirection;

    int mHearts;
    int mScore = 0;
    float mInvincibleTime;

    float mBlinkTimer = 0.0f;
    bool mIsBlinkVisible = true;

    bool mTriggeredAnimation = false;

    class RigidBodyComponent* mRigidBodyComponent;
    class DrawAnimatedComponent* mDrawComponent;
    class AABBColliderComponent* mColliderComponent;
};