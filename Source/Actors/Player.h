#pragma once
#include "Actor.h"
#include <string>
#include <vector>

class Player : public Actor
{
public:
    explicit Player(Game* game, float forwardSpeed = 150.0f);

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

private:
    float mDeathTimer = -1.0f;
    void ManageAnimations();
    std::vector<int> GetAnimationFramesByNamePrefix(const std::string& prefix, int frameCount);

    float mForwardSpeed;
    bool mIsRunning;
    bool mIsDying;
    std::string mLastDirection;

    int mHearts;
    float mInvincibleTime;
    // Blinking effect variables
    float mBlinkTimer = 0.0f;
    bool mIsBlinkVisible = true;

    class RigidBodyComponent* mRigidBodyComponent;
    class DrawAnimatedComponent* mDrawComponent;
    class AABBColliderComponent* mColliderComponent;
};