// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
//
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Actor.h"
#include "../Game.h"
#include "../Components/Component.h"
#include <algorithm>

Actor::Actor(Game* game)
        : mState(ActorState::Active)
        , mPosition(Vector2::Zero)
        , mScale(1.0f)
        , mRotation(0.0f)
        , mGame(game)
        , mIsOnGround(false)
        , mStepTimer(0.0f)
        , mTargetPos(Vector2::Zero)
        , mIsLocked(false)
{
    mGame->AddActor(this);
}

Actor::~Actor()
{
    mGame->RemoveActor(this);

    for(auto component : mComponents)
    {
        delete component;
    }
    mComponents.clear();
}

void Actor::SetPosition(const Vector2& pos)
{
    mPosition = pos;
    mGame->Reinsert(this);
}

void Actor::Update(float deltaTime)
{
    if (mState == ActorState::Active)
    {
        for (auto comp : mComponents)
        {
            if(comp->IsEnabled())
            {
                comp->Update(deltaTime);
            }
        }

        OnUpdate(deltaTime);
    }
}

void Actor::OnUpdate(float deltaTime)
{

}

void Actor::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) {

}

void Actor::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) {

}

void Actor::Kill()
{

}

void Actor::ProcessInput(const Uint8* keyState)
{
    if (mState == ActorState::Active)
    {
        for (auto comp : mComponents)
        {
            comp->ProcessInput(keyState);
        }

        OnProcessInput(keyState);
    }
}

void Actor::HandleKeyPress(const int key, const bool isPressed)
{
    if (mState == ActorState::Active)
    {
        for (auto comp : mComponents)
        {
            comp->HandleKeyPress(key, isPressed);
        }

        // Call actor-specific key press handling
        OnHandleKeyPress(key, isPressed);
    }

}

void Actor::OnProcessInput(const Uint8* keyState)
{

}

void Actor::OnHandleKeyPress(const int key, const bool isPressed)
{

}

void Actor::AddComponent(Component* c)
{
    mComponents.emplace_back(c);
    std::sort(mComponents.begin(), mComponents.end(), [](Component* a, Component* b) {
        return a->GetUpdateOrder() < b->GetUpdateOrder();
    });
}

bool Actor::IsVisibleOnCamera() const
{
    // Get the camera's position and dimensions
    Vector2 cameraPosition = mGame->GetCameraPos();
    float screenWidth = mGame->GetWindowWidth();
    float screenHeight = mGame->GetWindowHeight();

    // Check if the actor's position is within the camera's view
    return (mPosition.x >= cameraPosition.x && mPosition.x <= cameraPosition.x + screenWidth &&
            mPosition.y >= cameraPosition.y && mPosition.y <= cameraPosition.y + screenHeight);
}

std::tuple<int, int, int, int> Actor::ComputeColliderParams(
    int spriteWidthOriginal,
    int spriteHeightOriginal
) {

    //Approximate values obtained manually via Paint
    constexpr float px0 = 316.0f, py0 = 677.0f;
    constexpr float px1 = 570.0f, py1 = 930.0f;

    //Approximate values obtained manually via Paint
    constexpr float ux0 = 385.0f, uy0 = 715.0f;
    constexpr float ux1 = 489.0f, uy1 = 858.0f;

    float fullW_p = px1 - px0;
    float fullH_p = py1 - py0;

    float offX_p  = ux0 - px0;
    float offY_p  = uy0 - py0;

    float utilW_p = ux1 - ux0;
    float utilH_p = uy1 - uy0;

    float scaleX = float(spriteWidthOriginal)  / fullW_p;
    float scaleY = float(spriteHeightOriginal) / fullH_p;

    float dx_u = offX_p  * scaleX;
    float dy_u = offY_p  * scaleY;
    float w_u  = utilW_p * scaleX;
    float h_u  = utilH_p * scaleY;

    int dx = int(std::round(dx_u * Game::SCALE));
    int dy = int(std::round(dy_u * Game::SCALE));
    int w  = int(std::round(w_u  * Game::SCALE));
    int h  = int(std::round(h_u  * Game::SCALE));

    return { dx, dy + (h/2), w, h - (h/2) };
}