//
// Created by Lucas N. Ferreira on 03/08/23.
//

#include "Player.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"

Player::Player(Game* game, const float forwardSpeed, const float jumpSpeed)
        : Actor(game)
        , mIsRunning(false)
        , mIsDying(false)
        , mForwardSpeed(forwardSpeed)
{

    // TODO

    //mRigidBodyComponent = new RigidBodyComponent(...);
    //mColliderComponent = new AABBColliderComponent(...);
    //mDrawComponent = new DrawAnimatedComponent(...);

    //mDrawComponent->AddAnimation(...);

    //mDrawComponent->SetAnimation(...);
    //mDrawComponent->SetAnimFPS(...);
}

void Player::OnProcessInput(const uint8_t* state)
{
    // TODO
}

void Player::OnHandleKeyPress(const int key, const bool isPressed)
{
    if(mGame->GetGamePlayState() != Game::GamePlayState::Playing) return;

    // TODO
}

void Player::OnUpdate(float deltaTime)
{

    // TODO

    ManageAnimations();
}

void Player::ManageAnimations()
{
    // TODO
    //    if(...)
    //    {
    //        mDrawComponent->SetAnimation(...);
    //    }
}

void Player::Kill()
{
    //mIsDying = true;
    //mGame->SetGamePlayState(Game::GamePlayState::GameOver);
    //mDrawComponent->SetAnimation("Dead");

    // Disable collider and rigid body
    //mRigidBodyComponent->SetEnabled(false);
    //mColliderComponent->SetEnabled(false);

    // TODO

    //Pare todos os sons com StopAllSounds() e toque o som "Dead.wav".

    //mGame->ResetGameScene(3.5f); // Reset the game scene after 3 seconds
}

void Player::Win(AABBColliderComponent *poleCollider)
{
    // TODO
}

void Player::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Enemy)
    {
        // TODO
    }
    // TODO
}

void Player::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
    // TODO
}