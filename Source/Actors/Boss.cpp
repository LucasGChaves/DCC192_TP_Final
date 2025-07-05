#include "Boss.h"

int count = 0;

Boss::Boss(Game* game, Player* target, Vector2 pos)
        : Actor(game)
        , mTarget(target)
        , mSpeed(200.0f)
        , mIsDying(false)
{
    SetPosition(pos);
    SetScale(Game::SCALE);

    mRunToMiddleTimer = Random::GetIntRange(25, 35);
    mSpAttackTimer = 30.f;

    mDrawComponent = new DrawBossAnimatedComponent(this,
        "../Assets/Sprites/Boss/boss.png", "../Assets/Sprites/Boss/boss.json", 100, 1);
    mDrawComponent->SetUpdateOrder(0);
    mDrawComponent->AddAnimation("IdleDown", {0, 1, 2, 3, 4, 5});
    mDrawComponent->AddAnimation("IdleSide", {8, 9, 10, 11, 12, 13});
    mDrawComponent->AddAnimation("IdleUp", {16, 17, 18, 19, 20, 21});
    mDrawComponent->AddAnimation("WalkDown", {24, 25, 26, 27, 28, 29});
    mDrawComponent->AddAnimation("WalkSide", {32, 33, 34, 35, 36, 37});
    mDrawComponent->AddAnimation("WalkUp", {40, 41, 42, 43, 44, 45});
    mDrawComponent->AddAnimation("StrikeDown", {48, 49, 50, 51, 52, 53, 54, 55});
    mDrawComponent->AddAnimation("StrikeSide", {56, 57, 58, 59, 60, 61, 62, 63});
    mDrawComponent->AddAnimation("StrikeUp", {64, 65, 66, 67, 68, 69, 70, 71});
    mDrawComponent->AddAnimation("HurtDown", {72, 73, 74, 75});
    mDrawComponent->AddAnimation("HurtSide", {80, 81, 82, 83});
    mDrawComponent->AddAnimation("HurtUp", {88, 89, 90, 91});
    mDrawComponent->AddAnimation("Dead", {96, 97, 98, 99});
    mDrawComponent->AddAnimation("GoingToSpAttackPos", {105});
    mDrawComponent->AddAnimation("BeginSpAttack", {104});
    mDrawComponent->AddAnimation("LoopSpAttack", {106, 107, 108, 109});

    mDrawComponent->SetAnimation("IdleDown");
    mDrawComponent->SetAnimFPS(10.0f);

    float fullPx = Game::TILE_SIZE * 4 * Game::SCALE;
    Vector2 centerFull = pos + Vector2{ fullPx * 0.5f, fullPx * 0.5f };
    mDrawComponent->SetDefaultFrameSize(mDrawComponent->GetFrameSize(0));
    mDrawComponent->SetDefaultSpAttackFrameSize(mDrawComponent->GetFrameSize(106));

    Vector2 newPos = centerFull - (mDrawComponent->GetDefaultFrameSize() * 0.5f);
    SetPosition(newPos);


    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 0.0f, false);
    mColliderComponent = new AABBColliderComponent(
        this,0,0, static_cast<int>(std::round(mDrawComponent->GetDefaultFrameSize().x)),
        static_cast<int>(std::round(mDrawComponent->GetDefaultFrameSize().y)),
        ColliderLayer::Boss, false);

}

void Boss::OnUpdate(float deltaTime) {

    if (mLifePoints <= 0) {
        Die();
    }

    if (mIsDying) {
        if (mDeathTimer >= 0.f) {
            mDeathTimer -= deltaTime;
            return;
        }
        SetState(ActorState::Destroy);
        return;
    }

    if (mIsDying || !mTarget) return;

    HandleTimers(deltaTime);

    if (mRunToMiddleTimer >= 0.f) {
        mRunToMiddleTimer -= deltaTime;
    }
    else {
        mChasingPlayer = false;
        mSpeed = 300.f;
        if (mAtSpAttackPos && mSpAttackTimer >= 0.f) {
            mSpAttackTimer -= deltaTime;
            if (mBeginSpAttackTimer >= 0.f) {
                mDrawComponent->SetAnimation("BeginSpAttack");
                mBeginSpAttackTimer -= deltaTime;
            }
            else {
                mDrawComponent->SetAnimation("LoopSpAttack");
            }
        }
        else if (mAtSpAttackPos) {
            mChasingPlayer = true;
            mAtSpAttackPos = false;
            mRunToMiddleTimer = Random::GetIntRange(25, 35);
            mSpeed = 200.f;
            mSpAttackTimer = 30.f;
            mBeginSpAttackTimer = 1.f;
        }
    }

    //Dynamic collision logic
    const std::string& anim = mDrawComponent->GetAnimationName();

    // if (mRigidBodyComponent) {
    //     mRigidBodyComponent->SetVelocity(Vector2{-25.f, 0.f});
    // }

    if (anim == "LoopSpAttack") {
        Vector2 centerFull = mPosition + Vector2{ mDrawComponent->GetDefaultFrameSize().x * 0.5f,
            mDrawComponent->GetDefaultFrameSize().y * 0.5f };

        Vector2 framePos = centerFull - (mDrawComponent->GetSpAttackFrameSize() * 0.5f);

        Vector2 offset =  framePos - mPosition;

        if (mColliderComponent) {
            mColliderComponent->SetOffset(offset);
            mColliderComponent->SetSize(mDrawComponent->GetSpAttackFrameSize());
        }
    }
    else if (mColliderComponent) {
        mColliderComponent->SetOffset(Vector2::Zero);
        mColliderComponent->SetSize(mDrawComponent->GetDefaultFrameSize());
    }

    Vector2 distanceToTarget;

    float threshold = 10.0f;

    if (mChasingPlayer) {
        distanceToTarget = mTarget->GetPosition() - mPosition;
        threshold = 24.f * Game::SCALE;
    }
    else {
        distanceToTarget = mSpAttackPos - mPosition;
    }

    // Move the boss away from the player when hit
    if (mInvincibleTimer > 0.f) {
        distanceToTarget.Normalize();
        distanceToTarget *= -0.5f;
        Vector2 vel = distanceToTarget * mSpeed;
        mRigidBodyComponent->SetVelocity(vel);
        mDrawComponent->ForceSetAnimation("Hurt" + mLastDirection);
    }

    // Decrease attack delay and avoid other interactions
    if (!std::isnan(mAttackDelay) && mAttackDelay > .0f && mInvincibleTimer <= .0f) {
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
        mAttackDelay -= deltaTime;
        return;
    }

    // Do the attack
    if (!std::isnan(mAttackDelay) && mAttackDelay <= .0f && mDelayBetweenAttacks <= 0.0f && mInvincibleTimer <= .0f) {
        float x = 0.f, y = 0.f;

        if (mLastDirection == "Up") y = -4.f * Game::SCALE;
        else if (mLastDirection == "Down") y = 24.f * Game::SCALE;
        else if (mLastDirection == "Side") {
            if (GetRotation() == 0.f) x = 23.f * Game::SCALE;
            else x = -9.f * Game::SCALE;
        }

        new Attack(mGame, GetPosition(), Vector2(x, y), ColliderLayer::Boss);

        std::string animationName = "Strike" + mLastDirection;

        mDrawComponent->SetAnimation(animationName);
        mAttackDelay = NAN;
        mDelayBetweenAttacks = 2.5f;
        mPostAttackAnimationDelay = .5f;
        return;
    }

    if (mAtSpAttackPos && mSpAttackTimer >= 0.f || mInvincibleTimer > .0f) return;

    if (distanceToTarget.Length() > threshold && mInvincibleTimer <= 0.f)
    {
        distanceToTarget.Normalize();

        Vector2 vel = distanceToTarget * mSpeed;

        float diff = std::abs(std::abs(distanceToTarget.x) - std::abs(distanceToTarget.y));

        if (diff >= 0.f && diff <= 0.5f || std::abs(distanceToTarget.x) > std::abs(distanceToTarget.y)) {
            SetRotation(distanceToTarget.x > 0 ? 0.0f : Math::Pi);
            HandleChasingAnimation("HurtSide", "WalkSide");
            mLastDirection = "Side";
        }
        else if (distanceToTarget.y < 0)
        {
            HandleChasingAnimation("HurtUp", "WalkUp");
            mLastDirection = "Up";
        }
        else
        {
            HandleChasingAnimation("HurtDown", "WalkDown");
            mLastDirection = "Down";
        }

        mStepTimer -= deltaTime;
        if (mStepTimer <= 0.0f)
        {
            mStepTimer = 0.2f;
        }
        mRigidBodyComponent->SetVelocity(vel);
    }
    else
    {
        if (mChasingPlayer && mDelayBetweenAttacks <= .0f) {
            mAttackDelay = .5f;
        }

        if (mChasingPlayer && mDelayBetweenAttacks > .0f && std::isnan(mAttackDelay) && mPostAttackAnimationDelay <= .0f && mInvincibleTimer <= .0f) {
            mDrawComponent->SetAnimation("Idle" + mLastDirection);
        }

        if (!mChasingPlayer) {
            mDrawComponent->SetAnimation("GoingToSpAttackPos");
            mRigidBodyComponent->SetVelocity(Vector2::Zero);
            mAtSpAttackPos = true;
        }
    }
}

void Boss::OnHorizontalCollision(float overlap, AABBColliderComponent* other)
{
    // if (other->GetLayer() == ColliderLayer::Player) {
    //     if (auto *player = dynamic_cast<Player*>(other->GetOwner())) player->Hit();
    // }
}

void Boss::OnVerticalCollision(float overlap, AABBColliderComponent* other)
{
    // if (other->GetLayer() == ColliderLayer::Player) {
    //     if (auto *player = dynamic_cast<Player*>(other->GetOwner())) player->Hit();
    // }
}

void Boss::Die()
{
    if (mIsDying) return;

    mDrawComponent->SetAnimation("Dead");
    mDrawComponent->SetAnimFPS(4.0f);
    mIsDying = true;
    mColliderComponent->SetEnabled(false);
}

void Boss::Hit() {
    if (mInvincibleTimer > 0.f) return;

    mLifePoints--;
    mInvincibleTimer = 2.f;
}

void Boss::HandleChasingAnimation(std::string hurtAnimation, std::string walkAnimation) {
    if (mInvincibleTimer > 0.f) {
        mDrawComponent->SetAnimation(hurtAnimation);
        return;
    }

    if (mChasingPlayer) {
        mDrawComponent->SetAnimation(walkAnimation);
        return;
    }

    mDrawComponent->SetAnimation("GoingToSpAttackPos");
}

void Boss::HandleTimers(float deltaTime) {
    if (mInvincibleTimer > 0.f) mInvincibleTimer -= deltaTime;
    if (mDelayBetweenAttacks > 0.f) mDelayBetweenAttacks -= deltaTime;
    if (mPostAttackAnimationDelay > .0f) mPostAttackAnimationDelay -= deltaTime;
}
