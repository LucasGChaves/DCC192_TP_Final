#include "Player.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/ColliderComponents/AABBColliderComponent.h"
#include "../Components/RigidBodyComponent.h"
#include <SDL2/SDL_mixer.h>
#include <algorithm>

Player::Player(Game* game, const float forwardSpeed)
        : Actor(game)
        , mIsRunning(false)
        , mIsDying(false)
        , mForwardSpeed(forwardSpeed)
        , mLastDirection("Down")
{
    SetPosition(Vector2(100.0f, 100.0f));
    float scale = mGame->GetWindowHeight() / 240.0f;
    SetScale(scale);

    mRigidBodyComponent = new RigidBodyComponent(this);
    mColliderComponent = new AABBColliderComponent(
            this,
            0, 0,
            32, 32,
            ColliderLayer::Player,
            false,
            100
    );

    mColliderComponent->SetEnabled(false);

    mDrawComponent = new DrawAnimatedComponent(this, "../Assets/Sprites/Player/player.png", "../Assets/Sprites/Player/player.json");

    mDrawComponent->AddAnimation("IdleDown", GetAnimationFramesByNamePrefix("Idle_down", 6));
    mDrawComponent->AddAnimation("IdleDown", GetAnimationFramesByNamePrefix("Idle_down", 6));
    mDrawComponent->AddAnimation("IdleUp", GetAnimationFramesByNamePrefix("Idle_up", 6));
    mDrawComponent->AddAnimation("IdleSide", GetAnimationFramesByNamePrefix("Idle_horizontal", 6));
    mDrawComponent->AddAnimation("WalkDown", GetAnimationFramesByNamePrefix("Walk_down", 6));
    mDrawComponent->AddAnimation("WalkUp", GetAnimationFramesByNamePrefix("Walk_up", 6));
    mDrawComponent->AddAnimation("WalkSide", GetAnimationFramesByNamePrefix("Walk_horizontal", 6));
    mDrawComponent->AddAnimation("StrikeDown", GetAnimationFramesByNamePrefix("Strike_down", 4));
    mDrawComponent->AddAnimation("StrikeUp", GetAnimationFramesByNamePrefix("Strike_up", 4));
    mDrawComponent->AddAnimation("StrikeSide", GetAnimationFramesByNamePrefix("Strike_horizontal", 4));
    mDrawComponent->AddAnimation("Dead", GetAnimationFramesByNamePrefix("Death", 5));

    mDrawComponent->SetAnimation("IdleDown");
    mDrawComponent->SetAnimFPS(10.0f);
    SDL_Log("✅ Player criado em %.1f, %.1f", GetPosition().x, GetPosition().y);
}

void Player::OnProcessInput(const uint8_t* state)
{
    if (mIsDying) return;

    Vector2 velocity = Vector2::Zero;
    mIsRunning = false;

    if (state[SDL_SCANCODE_UP])
    {
        velocity.y -= 1.0f;
        mLastDirection = "Up";
    }
    else if (state[SDL_SCANCODE_DOWN])
    {
        velocity.y += 1.0f;
        mLastDirection = "Down";
    }

    if (state[SDL_SCANCODE_LEFT])
    {
        velocity.x -= 1.0f;
        mLastDirection = "Side";
        SetRotation(Math::Pi);

    }
    else if (state[SDL_SCANCODE_RIGHT])
    {
        velocity.x += 1.0f;
        mLastDirection = "Side";
        SetRotation(0.0f);
    }

    if (!Math::NearZero(velocity.Length()))
    {
        velocity.Normalize();
        velocity *= mForwardSpeed;
        mIsRunning = true;
    }

    mRigidBodyComponent->SetVelocity(velocity);
}

void Player::OnHandleKeyPress(const int key, const bool isPressed)
{
    if (mGame->GetGamePlayState() != Game::GamePlayState::Playing || mIsDying) return;

    if (key == SDLK_SPACE && isPressed)
    {
        mDrawComponent->ForceSetAnimation("Strike" + mLastDirection);
    }
}

void Player::OnUpdate(float deltaTime)
{
    if (mIsDying) return;
    ManageAnimations();
}

void Player::ManageAnimations()
{
    const std::string& currentAnim = mDrawComponent->GetAnimationName();
    if (currentAnim.find("Strike") != std::string::npos)
    {
        if (!mDrawComponent->IsAnimationFinished())
            return;
    }

    if (mIsRunning)
    {
        mDrawComponent->SetAnimation("Walk" + mLastDirection);
    }
    else
    {
        mDrawComponent->SetAnimation("Idle" + mLastDirection);
    }
}


void Player::Kill()
{
    if (mIsDying) return;

    mIsDying = true;
    mGame->SetGamePlayState(Game::GamePlayState::GameOver);

    mDrawComponent->SetAnimation("Dead");
    mRigidBodyComponent->SetEnabled(false);

    mGame->GetAudio()->StopAllSounds();
    mGame->GetAudio()->PlaySound("Assets/Audio/Dead.wav");

    mGame->ResetGameScene(3.5f);
}

void Player::Win(AABBColliderComponent *poleCollider)
{
}

void Player::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Enemy)
    {
        Kill();
    }
}

void Player::EnableCollision(bool enabled)
{
    if (mColliderComponent)
        mColliderComponent->SetEnabled(enabled);
}

void Player::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Enemy)
    {
        Kill();
    }
}

std::vector<int> Player::GetAnimationFramesByNamePrefix(const std::string& prefix, int frameCount)
{
    std::vector<int> frames;
    const auto& frameNames = mDrawComponent->GetFrameNames();
    for (size_t i = 0; i < frameNames.size(); ++i)
    {
        if (frameNames[i].find(prefix) != std::string::npos)
        {
            frames.emplace_back(static_cast<int>(i));
        }
    }

    std::sort(frames.begin(), frames.end());
    if (frames.size() > static_cast<size_t>(frameCount)) {
        frames.resize(frameCount);
    }

    return frames;
}