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
    //------

    if (mAtSpAttackPos && mSpAttackTimer >= 0.f) return;

    Vector2 distanceToTarget = Vector2::Zero;

    float threshold = 1.0f;

    if (mChasingPlayer) {
        distanceToTarget = mTarget->GetPosition() - mPosition;
        //threshold = 1.0f;
    }
    else {
        distanceToTarget = mSpAttackPos - mPosition;
    }

    if (distanceToTarget.Length() > threshold)
    {
        distanceToTarget.Normalize();

        Vector2 vel = distanceToTarget * mSpeed;

        float diff = std::abs(std::abs(distanceToTarget.x) - std::abs(distanceToTarget.y));

        if (diff >= 0.f && diff <= 0.5f) {
            SetRotation(distanceToTarget.x > 0 ? 0.0f : Math::Pi);
            if (mInvincibleTimer > 0.f) mDrawComponent->SetAnimation("HurtSide");
            else {
                if (mChasingPlayer) mDrawComponent->SetAnimation("WalkSide");
                else mDrawComponent->SetAnimation("GoingToSpAttackPos");
            }
        }

        else if (std::abs(distanceToTarget.x) > std::abs(distanceToTarget.y))
        {
            SetRotation(distanceToTarget.x > 0 ? 0.0f : Math::Pi);
            if (mInvincibleTimer > 0.f) mDrawComponent->SetAnimation("HurtSide");
            else {
                if (mChasingPlayer) mDrawComponent->SetAnimation("WalkSide");
                else mDrawComponent->SetAnimation("GoingToSpAttackPos");
            }
        }
        else if (distanceToTarget.y < 0)
        {
            if (mInvincibleTimer > 0.f) mDrawComponent->SetAnimation("HurtUp");
            else {
                if (mChasingPlayer) mDrawComponent->SetAnimation("WalkUp");
                else mDrawComponent->SetAnimation("GoingToSpAttackPos");
            }
        }
        else
        {
            if (mInvincibleTimer > 0.f) mDrawComponent->SetAnimation("HurtDown");
            else {
                if (mChasingPlayer) mDrawComponent->SetAnimation("WalkDown");
                else mDrawComponent->SetAnimation("GoingToSpAttackPos");
            }
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
        if (mInvincibleTimer > 0.f) mDrawComponent->SetAnimation("HurtDown");
        else {
            if (mChasingPlayer) mDrawComponent->SetAnimation("IdleDown");
            else mDrawComponent->SetAnimation("GoingToSpAttackPos");
        }
        if (!mChasingPlayer) {
            mRigidBodyComponent->SetVelocity(Vector2::Zero);
            mAtSpAttackPos = true;
        }
    }

    if (mInvincibleTimer > 0.f) {
        mInvincibleTimer -= deltaTime;
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
    SDL_Log("DECREASING LIFEPOINTS");
    mLifePoints--;
    mInvincibleTimer = 2.f;
}