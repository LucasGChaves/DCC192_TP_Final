#include "Player.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/ColliderComponents/AABBColliderComponent.h"
#include "../Components/RigidBodyComponent.h"
#include <SDL_mixer.h>
#include <algorithm>
#include "../Math.h"

#include "Attack.h"
#include "Dog.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../UIElements/UIGameOver.h"

Player::Player(Game* game, Vector2 pos, const float forwardSpeed)
        : Actor(game)
        , mIsRunning(false)
        , mIsDying(false)
        , mForwardSpeed(forwardSpeed)
        , mLastDirection("Down")
        , mHearts(5)
        , mInvincibleTime(.0f)
        , mBlinkTimer(0.0f)
        , mIsBlinkVisible(true)
        , mScore(0)
{
    SetPosition(pos);
    SetScale(Game::SCALE);


    auto [dx, dy, w, h] = ComputeColliderParams(Game::TILE_SIZE * 2, Game::TILE_SIZE * 2);
    mRigidBodyComponent = new RigidBodyComponent(this);
    mColliderComponent = new AABBColliderComponent(this,dx, dy, w, h, ColliderLayer::Player, false, 100);

    mDrawComponent = new DrawAnimatedComponent(this, "../Assets/Sprites/Player/player.png", "../Assets/Sprites/Player/player.json", 6);

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
    auto deadFrames = GetAnimationFramesByNamePrefix("Death", 5);
    mDrawComponent->AddAnimation("Dead", deadFrames);

    mDrawComponent->SetAnimation("IdleDown");
    mDrawComponent->SetAnimFPS(10.0f);
}

void Player::OnProcessInput(const uint8_t* state)
{
    if (mIsDying) return;

    Vector2 velocity = Vector2::Zero;
    mIsRunning = false;

    if (!mIsLocked) {
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
    }
    else if (mTriggeredAnimation){
        if (Math::NearZero(Math::Abs(mTargetPos.x-mPosition.x), 5.f)) {
            velocity.y -= 1.0f;
            mLastDirection = "Up";
        }
        else if ((mPosition.x > mTargetPos.x) && !Math::NearZero(Math::Abs(mTargetPos.x-mPosition.x), 5.f))
        {
            velocity.x -= 1.0f;
            mLastDirection = "Side";
            SetRotation(Math::Pi);

        }
        else if ((mPosition.x < mTargetPos.x) && !Math::NearZero(Math::Abs(mTargetPos.x-mPosition.x), 5.f))
        {
            velocity.x += 1.0f;
            mLastDirection = "Side";
            SetRotation(0.0f);
        }
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
        const std::string& currentAnim = mDrawComponent->GetAnimationName();
        if (currentAnim.find("Strike") == std::string::npos)
        {
            mGame->GetAudio()->PlaySound("PlayerAttack.wav");
        }

        mDrawComponent->ForceSetAnimation("Strike" + mLastDirection);

        Vector2 base = GetPosition();
        float w = 48.0f, h = 48.0f, offset = -8.0f;
        std::vector<Vector2> poly;
        if (mLastDirection == "Up") {
            poly = { Vector2(base.x, base.y - offset), Vector2(base.x + w, base.y - offset), Vector2(base.x + w, base.y - offset + h), Vector2(base.x, base.y - offset + h) };
        } else if (mLastDirection == "Down") {
            poly = { Vector2(base.x, base.y + offset), Vector2(base.x + w, base.y + offset), Vector2(base.x + w, base.y + offset + h), Vector2(base.x, base.y + offset + h) };
        } else if (mLastDirection == "Side") {
            if (GetRotation() == 0.0f) {
                poly = { Vector2(base.x + offset, base.y), Vector2(base.x + offset + w, base.y), Vector2(base.x + offset + w, base.y + h), Vector2(base.x + offset, base.y + h) };
            } else {
                poly = { Vector2(base.x - offset - w, base.y), Vector2(base.x - offset, base.y), Vector2(base.x - offset, base.y + h), Vector2(base.x - offset - w, base.y + h) };
            }
        }
        new Attack(mGame, poly);
    }
}

void Player::Hit() {
    if (mInvincibleTime >= .0f) return;

    mHearts -= 1;
    mInvincibleTime = 2.f;
    mBlinkTimer = 0.0f;
    mIsBlinkVisible = true;
    if (mDrawComponent) mDrawComponent->SetIsVisible(true);
}


void Player::OnUpdate(float deltaTime)
{
    if (mHearts <= 0) Kill();

    if (mGame->GetGameScene() == Game::GameScene::Level1 &&
        mGame->GetDog()->GetState() == Dog::State::Dying &&
        mIsLocked) {
        mIsLocked = false;
    }

    if (mInvincibleTime >= .0f) {
        mInvincibleTime -= deltaTime;
        // Blinking logic: toggle every 0.1s
        mBlinkTimer += deltaTime;
        if (mBlinkTimer >= 0.1f) {
            mIsBlinkVisible = !mIsBlinkVisible;
            mBlinkTimer = 0.0f;
        }
        if (mDrawComponent) mDrawComponent->SetIsVisible(mIsBlinkVisible);
    } else {
        // Ensure visible when not invincible
        if (mDrawComponent) mDrawComponent->SetIsVisible(true);
        mIsBlinkVisible = true;
        mBlinkTimer = 0.0f;
    }

    if (mIsDying)
    {
        mDeathTimer -= deltaTime;
        if (mDeathTimer <= 0.0f)
        {
            mGame->SetGamePlayState(Game::GamePlayState::GameOver);
            mGame->ResetGameScene(0.0f);
        }
    }
    else if (mIsRunning)
    {
        mStepTimer -= deltaTime;
    }

    if (mGame->GetGamePlayState() == Game::GamePlayState::Leaving && mPosition.y <= mTargetPos.y &&
        mGame->GetGameScene() == Game::GameScene::Level1) {
        mState = ActorState::Destroy;
        mGame->SetGameScene(Game::GameScene::Level2, 1.5f);
        mTarget->GetComponent<AABBColliderComponent>()->SetEnabled(true);
    }
    else if (mGame->GetGamePlayState() == Game::GamePlayState::EnteringMap &&
        mPosition.y <= (mTargetPos.y - ((Game::TILE_SIZE + Game::TILE_SIZE/2) * Game::SCALE)) &&
        mGame->GetGameScene() == Game::GameScene::Level2) {
        mIsLocked = false;
        mGame->SetGamePlayState(Game::GamePlayState::Playing);
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
        mTarget->GetComponent<AABBColliderComponent>()->SetEnabled(true);
    }
    ManageAnimations();
}

void Player::ManageAnimations()
{
    if (mIsDying)
    {
        if (mDrawComponent->GetAnimationName() != "Dead")
        {
            mDrawComponent->ForceSetAnimation("Dead");
        }
        return;
    }

    const std::string& currentAnim = mDrawComponent->GetAnimationName();
    if (currentAnim.find("Strike") != std::string::npos)
    {
        if (!mDrawComponent->IsAnimationFinished())
        {
            return;
        }
    }

    if (mIsRunning)
    {
        if (mStepTimer <= 0.0f)
        {
            mGame->GetAudio()->PlaySound("PlayerWalk.wav");
            mStepTimer = 0.2f;
        }
        mDrawComponent->SetAnimation("Walk" + mLastDirection);
    }
    else
    {
        mDrawComponent->SetAnimation("Idle" + mLastDirection);
    }
}


void Player::Kill()
{
    mIsDying = true;
    mDeathTimer = 1.0f;

    mDrawComponent->ForceSetAnimation("Dead");
    mRigidBodyComponent->SetVelocity(Vector2::Zero);
    mRigidBodyComponent->SetEnabled(false);
    mColliderComponent->SetEnabled(false);

    mGame->GetAudio()->StopAllSounds();

    mGame->SetGamePlayState(Game::GamePlayState::GameOver);
    auto* gameOverScreen = new UIGameOver(mGame);
    gameOverScreen->Open();
}

void Player::Win(AABBColliderComponent *poleCollider)
{
}

void Player::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Enemy)
    {
        Hit();
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
        Hit();
    }

    if (minOverlap < 0 && other->GetLayer() == ColliderLayer::InvisibleWall && !mIsLocked) {
        mIsLocked = true;
        mTriggeredAnimation = true;
        other->SetEnabled(false);
        mTargetPos = other->GetOwner()->GetPosition();
        mTarget = other->GetOwner();

        if (mGame->GetGameScene() == Game::GameScene::Level1) {
            mGame->SetGamePlayState(Game::GamePlayState::Leaving);
        }
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