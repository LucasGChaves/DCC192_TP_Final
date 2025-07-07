#include "Player.h"


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

    // int dx = (Game::TILE_SIZE * Game::SCALE * 2) / 3;
    // int dy = (Game::TILE_SIZE * Game::SCALE) / 4;
    // int width = (Game::TILE_SIZE * Game::SCALE * 2) / 3;
    // int height = (Game::TILE_SIZE * Game::SCALE) / 2;


    auto [dx, dy, w, h] = ComputeColliderParams(Game::TILE_SIZE * 2, Game::TILE_SIZE * 2);
    mRigidBodyComponent = new RigidBodyComponent(this);
    mColliderComponent = new AABBColliderComponent(this,dx, dy, w, h, ColliderLayer::Player, false, 100);

    //   std::vector<Vector2> vertices = {
    //       Vector2(dx, dy),                     // Top-left
    //       Vector2(dx + w, dy),            // Top-right
    //       Vector2(dx + w, dy + h),   // Bottom-right
    //       Vector2(dx, dy + h)            // Bottom-left
    //   };
    // new DrawPolygonComponent(this, vertices);
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

        float x = 0.f, y = 0.f;

        if (mLastDirection == "Up") y = -4.f * Game::SCALE;
        else if (mLastDirection == "Down") y = 24.f * Game::SCALE;
        else if (mLastDirection == "Side") {
            if (GetRotation() == 0.f) x = 23.f * Game::SCALE;
            else x = -9.f * Game::SCALE;
        }

        new Attack(mGame, GetPosition(), Vector2(x, y), ColliderLayer::Player);
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
        mGame->GetDog() && mGame->GetDog()->GetState() == Dog::State::Dying &&
        mIsLocked && mGame->GetTopInvisibleWall()->GetComponent<AABBColliderComponent>()->IsEnabled()) {
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

    if (mGame->GetTopInvisibleWall() && mGame->GetGamePlayState() == Game::GamePlayState::Leaving
        && mPosition.y <= mGame->GetTopInvisibleWall()->GetPosition().y &&
        mGame->GetGameScene() != Game::GameScene::MainMenu) {
        mState = ActorState::Destroy;
        mGame->SetGameScene(static_cast<Game::GameScene>(static_cast<int>(mGame->GetGameScene())+1), 1.5f);
    }
    else if (mGame->GetBottomInvisibleWall() && mGame->GetGamePlayState() == Game::GamePlayState::EnteringMap &&
        mPosition.y <= (mGame->GetBottomInvisibleWall()->GetPosition().y - ((Game::TILE_SIZE + Game::TILE_SIZE/2) * Game::SCALE)) &&
        mGame->GetGameScene() != Game::GameScene::MainMenu) {
        mIsLocked = false;
        mGame->SetGamePlayState(Game::GamePlayState::Playing);
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
        mGame->GetBottomInvisibleWall()->GetComponent<AABBColliderComponent>()->SetEnabled(true);
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
    mGame->GetAudio()->PlaySound("PlayerDead.wav");

    // Show Game Over menu
    mGame->SetGamePlayState(Game::GamePlayState::GameOver);
    auto* gameOverScreen = new UIGameOver(mGame);
    gameOverScreen->Open();
}

void Player::Win(AABBColliderComponent *poleCollider)
{
}

void Player::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Boss) {
        auto boss = dynamic_cast<Boss*>(other->GetOwner());
        if (boss->IsAtSpAttackPos() && boss->GetSpAttackTimer() > 0.0f) {
            Hit();
        }
    }

    else if (other->GetLayer() == ColliderLayer::InvisibleWall && !mIsLocked) {
        mIsLocked = true;
        mTriggeredAnimation = true;
        mTargetPos = other->GetOwner()->GetPosition();
        other->SetEnabled(false);
        InvisibleWall* wall = dynamic_cast<InvisibleWall*>(other->GetOwner());
        wall->SetColliding(true);
        if (wall->GetType() == InvisibleWall::Type::Top) {
            mGame->SetGamePlayState(Game::GamePlayState::Leaving);
        }
    }

    else if (other->GetLayer() == ColliderLayer::Projectile) {
        Hit();
        other->SetEnabled(false);
        auto otherRigidBody = other->GetOwner()->GetComponent<RigidBodyComponent>();
        if (otherRigidBody) otherRigidBody->SetEnabled(false);
        other->GetOwner()->SetState(ActorState::Destroy);
    }
}

void Player::EnableCollision(bool enabled)
{
    if (mColliderComponent)
        mColliderComponent->SetEnabled(enabled);
}

void Player::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Boss) {
        auto boss = dynamic_cast<Boss*>(other->GetOwner());
        if (boss->IsAtSpAttackPos() && boss->GetSpAttackTimer() > 0.0f) {
            Hit();
        }
    }

    else if (minOverlap < 0 && other->GetLayer() == ColliderLayer::InvisibleWall && !mIsLocked) {
        mIsLocked = true;
        mTriggeredAnimation = true;
        mTargetPos = other->GetOwner()->GetPosition();
        other->SetEnabled(false);
        InvisibleWall* wall = dynamic_cast<InvisibleWall*>(other->GetOwner());
        wall->SetColliding(true);
        if (wall->GetType() == InvisibleWall::Type::Top) {
            mGame->SetGamePlayState(Game::GamePlayState::Leaving);
        }
    }

    else if (other->GetLayer() == ColliderLayer::Projectile) {
        Hit();
        other->SetEnabled(false);
        auto otherRigidBody = other->GetOwner()->GetComponent<RigidBodyComponent>();
        if (otherRigidBody) otherRigidBody->SetEnabled(false);
        other->GetOwner()->SetState(ActorState::Destroy);
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