#include "Skeleton.h"
#include "Player.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/ColliderComponents/AABBColliderComponent.h"
#include "../Components/RigidBodyComponent.h"

Skeleton::Skeleton(Game* game, Player* target, Vector2 pos)
        : Actor(game)
        , mTarget(target)
        , mSpeed(200.0f)
        , mIsDying(false)
        , mLastTargetTile(Vector2{-1.f, -1.f})
{
    mIsLocked = false;
    SetPosition(pos);
    SetScale(Game::SCALE);

    mDrawComponent = new DrawAnimatedComponent(this,
                                               "../Assets/Sprites/Skeleton/skeleton.png",
                                               "../Assets/Sprites/Skeleton/skeleton.json");

    mDrawComponent->AddAnimation("IdleDown", GetAnimationFramesByNamePrefix("Idle_down", 6));
    mDrawComponent->AddAnimation("IdleUp", GetAnimationFramesByNamePrefix("Idle_up", 6));
    mDrawComponent->AddAnimation("IdleSide", GetAnimationFramesByNamePrefix("Idle_horizontal", 6));
    mDrawComponent->AddAnimation("WalkDown", GetAnimationFramesByNamePrefix("Walk_down", 6));
    mDrawComponent->AddAnimation("WalkUp", GetAnimationFramesByNamePrefix("Walk_up", 6));
    mDrawComponent->AddAnimation("WalkSide", GetAnimationFramesByNamePrefix("Walk_horizontal", 6));
    mDrawComponent->AddAnimation("Dead", GetAnimationFramesByNamePrefix("Death", 5));

    mDrawComponent->SetAnimation("IdleDown");
    mDrawComponent->SetAnimFPS(10.0f);

    auto [dx, dy, w, h] = ComputeColliderParams(Game::TILE_SIZE * 2, Game::TILE_SIZE * 2);

    mRigidBodyComponent = new RigidBodyComponent(this);

    mColliderComponent = new AABBColliderComponent(this, dx, dy, w, h, ColliderLayer::Enemy, false);
}

void Skeleton::OnUpdate(float deltaTime)
{
    if (mState == State::Dying) {
        mDeathTimer -= deltaTime;
        if (mDeathTimer <= 0.0f) {
            SetState(ActorState::Destroy);
        }
        return;
    }

    if (mState == State::Dying || !mTarget) return;

    Vector2 toPlayer = mTarget ? (mTarget->GetPosition() - mPosition): Vector2::Zero;

    bool seesPlayer = (mState == State::Wander)
               && InsideFOV(toPlayer)
               && HasLineOfSight();

    if (seesPlayer)
    {
        mState = State::Chasing;
        mLastTargetTile = Vector2{-1.f, -1.f};
    }

    switch (mState)
    {
        case State::Wander:
            Wander(deltaTime);
        break;

        case State::Chasing:
            Chase(deltaTime);
        break;
    }
}

void Skeleton::OnHorizontalCollision(float overlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Player) {
        if (auto *player = dynamic_cast<Player*>(other->GetOwner())) player->Hit();
    }
}

void Skeleton::OnVerticalCollision(float overlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Player) {
        if (auto *player = dynamic_cast<Player*>(other->GetOwner())) player->Hit();
    }
}

void Skeleton::Die()
{
    if (mIsDying || mState == State::Dying) return;
    mIsDying = true;
    mState = State::Dying;
    mDrawComponent->SetAnimation("Dead");
    mColliderComponent->SetEnabled(false); 
    mDeathTimer = 0.5f; 

    if (mTarget) {
        mTarget->AddScore(1);
    }
}

std::vector<int> Skeleton::GetAnimationFramesByNamePrefix(const std::string& prefix, int frameCount)
{
    std::vector<int> frames;
    const auto& names = mDrawComponent->GetFrameNames();
    for (size_t i = 0; i < names.size(); ++i)
    {
        if (names[i].find(prefix) != std::string::npos)
            frames.emplace_back(static_cast<int>(i));
    }
    std::sort(frames.begin(), frames.end());
    if (frames.size() > static_cast<size_t>(frameCount))
        frames.resize(frameCount);
    return frames;
}

bool Skeleton::InsideFOV(Vector2 toPlayer) {
    if (mState == State::Chasing) {
        return true;
    }

    mCurrentDir.Normalize();
    toPlayer.Normalize();
    float dot = toPlayer.x * mCurrentDir.x + toPlayer.y * mCurrentDir.y;
    return dot >= Math::Cos(mFOVAngle/2.f);
}

bool Skeleton::HasLineOfSight()
{
    const float tilePx = Game::TILE_SIZE * Game::SCALE;
    //auto& passable     = mGame->GetPassableVector();
    int H              = (int)mGame->GetPassableVector().size();
    int W              = (int)mGame->GetPassableVector()[0].size();

    Vector2 center  = mColliderComponent->GetCenter();
    auto* bb1 = mTarget->GetComponent<AABBColliderComponent>();
    Vector2 playerCenter   = bb1->GetCenter();

    int x0 = int(center.x / tilePx);
    int y0 = int(center.y / tilePx);
    int x1 = int(playerCenter.x  / tilePx);
    int y1 = int(playerCenter.y  / tilePx);

    if (x1 < 0 || x1 >= W || y1 < 0 || y1 >= H)
        return false;

    // 4) Setup de Bresenham
    int dx =  abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    while (true)
    {
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }

        if (x0 == x1 && y0 == y1)
            return true;

        if (x0 < 0 || x0 >= W || y0 < 0 || y0 >= H)
            return false;

        if (!mGame->GetPassableVector()[y0][x0])
            return false;
    }
}

void Skeleton::InitWander()
{
    srand(static_cast<unsigned int>(time(nullptr)));

    int direction = rand() % 4;

    switch(direction) {
        case 0: mWanderDir = Vector2{ 1, 0 }; break;
        case 1: mWanderDir = Vector2{-1, 0 }; break;
        case 2: mWanderDir = Vector2{ 0, 1 }; break;
        default:mWanderDir = Vector2{ 0,-1 }; break;
    }
    mWanderTimer = 0.0f;
    mCurrentDir = mWanderDir;
}

void Skeleton::Wander(float deltaTime)
{
    mWanderTimer += deltaTime;
    if (mWanderTimer >= 1.0f) {
        InitWander();
    }

    mRigidBodyComponent->SetVelocity(mWanderDir * mSpeed);

    if (std::abs(mWanderDir.x) >= std::abs(mWanderDir.y)) {
        SetRotation(mWanderDir.x > 0 ? 0.0f : Math::Pi);
        mDrawComponent->SetAnimation("WalkSide");
    }
    else if (mWanderDir.y < 0) {
        mDrawComponent->SetAnimation("WalkUp");
    }
    else {
        mDrawComponent->SetAnimation("WalkDown");
    }
}

void Skeleton::Chase(float deltaTime)
{
    Vector2 lastPlayerCenter = Vector2::Zero;

    const float tilePx = Game::TILE_SIZE * Game::SCALE;

    Vector2 myCenter     = mColliderComponent->GetCenter();
    auto* playerBB       = mTarget->GetComponent<AABBColliderComponent>();
    Vector2 playerCenter = playerBB->GetCenter();

    int r0 = int(myCenter.y / tilePx);
    int c0 = int(myCenter.x / tilePx);
    int r1 = int(playerCenter.y / tilePx);
    int c1 = int(playerCenter.x / tilePx);

    mRepathTimer -= deltaTime;

    float distMoved = (playerCenter - lastPlayerCenter).Length();

    bool tileChanged    = (r1 != int(mLastTargetTile.x) || c1 != int(mLastTargetTile.y));
    bool movedFarEnough = (distMoved > tilePx * 0.5f);
    bool timeToRepath   = (mRepathTimer <= 0.0f);

    auto [goalR, goalC] = GetGoalTile(
    mTarget->GetComponent<AABBColliderComponent>(),
    mGame->GetPassable2x2Vector(),
    (int)mGame->GetPassable2x2Vector().size(),
    (int)mGame->GetPassable2x2Vector()[0].size()
);
    if (tileChanged || movedFarEnough || timeToRepath)
    {
        mPath = FindPathAStar(mGame->GetPassable2x2Vector(),r0, c0, goalR, goalC);

        if (!mPath.empty())
        {
            Vector2 startTileCenter{ (c0 + 0.5f) * tilePx, (r0 + 0.5f) * tilePx };
            if ((mPath.front() - startTileCenter).Length() < 1e-3f)
                mPath.erase(mPath.begin());
        }

        mLastTargetTile = Vector2{ float(r1), float(c1) };
        mNextWaypoint = 0;
        mRepathTimer = 0.5f;
    }

    const float reachRad = tilePx * 0.10f;
    while (mNextWaypoint < mPath.size())
    {
        if ((mPath[mNextWaypoint] - myCenter).Length() < reachRad)
            ++mNextWaypoint;
        else
            break;
    }

    Vector2 desiredVel = Vector2::Zero;
    Vector2 toPlayer = mTarget->GetPosition() - mPosition;

    if (mNextWaypoint < mPath.size() && toPlayer.Length() > Game::TILE_SIZE * Game::SCALE )
    {
        Vector2 toWP = mPath[mNextWaypoint] - myCenter;
        mCurrentDir = toWP;
        toWP.Normalize();
        desiredVel  = toWP * mSpeed;
    }
    else {

        if (toPlayer.Length() > 1.f) {
            mCurrentDir = toPlayer;
            toPlayer.Normalize();
            desiredVel = toPlayer * mSpeed;
        }
        else {
            desiredVel = Vector2::Zero;
        }
    }

    mRigidBodyComponent->SetVelocity(desiredVel);

    float diff = std::abs(std::abs(mCurrentDir.x) - std::abs(mCurrentDir.y));
    if (diff <= 0.5f) {
        SetRotation(mCurrentDir.x > 0 ? 0.0f : Math::Pi);
        mDrawComponent->SetAnimation("WalkSide");
    }
    else if (std::abs(mCurrentDir.x) > std::abs(mCurrentDir.y)) {
        SetRotation(mCurrentDir.x > 0 ? 0.0f : Math::Pi);
        mDrawComponent->SetAnimation("WalkSide");
    }
    else if (mCurrentDir.y < 0) {
        mDrawComponent->SetAnimation("WalkUp");
    }
    else {
        mDrawComponent->SetAnimation("WalkDown");
    }
}

std::pair<int,int> Skeleton::GetGoalTile(
    const AABBColliderComponent* playerBB,
    const std::vector<std::vector<bool>>& passable2x2,
    int maxRows, int maxCols)
{
    Vector2 min = playerBB->GetMin();
    Vector2 max = playerBB->GetMax();
    float tilePx = Game::TILE_SIZE * Game::SCALE;

    int r0 = int(min.y / tilePx);
    int c0 = int(min.x / tilePx);
    int r1 = int((max.y - 1) / tilePx);
    int c1 = int((max.x - 1) / tilePx);

    struct Node { int r,c; float dist; };
    std::vector<Node> candidates;

    Vector2 skCenter = mColliderComponent->GetCenter();
    int sr = int(skCenter.y / tilePx), sc = int(skCenter.x / tilePx);

    for (int r = r0; r <= r1; ++r) {
      for (int c = c0; c <= c1; ++c) {
        if (r>=0 && r<maxRows && c>=0 && c<maxCols && passable2x2[r][c]) {
          float dr = float(r - sr), dc = float(c - sc);
          candidates.push_back({r,c, dr*dr + dc*dc});
        }
      }
    }

    if (!candidates.empty()) {
      auto best = std::min_element(
        candidates.begin(), candidates.end(),
        [](auto &a, auto &b){ return a.dist < b.dist; }
      );
      return {best->r, best->c};
    }

    const int drs[8] = {-1,-1,-1, 0, 0, 1, 1, 1};
    const int dcs[8] = {-1, 0, 1,-1, 1,-1, 0, 1};
    candidates.clear();

    for (int r = r0-1; r <= r1+1; ++r) {
      for (int c = c0-1; c <= c1+1; ++c) {
        if (r>=0 && r<maxRows && c>=0 && c<maxCols && passable2x2[r][c]) {
          float dr = float(r - sr), dc = float(c - sc);
          candidates.push_back({r,c, dr*dr + dc*dc});
        }
      }
    }
    if (!candidates.empty()) {
      auto best = std::min_element(
        candidates.begin(), candidates.end(),
        [](auto &a, auto &b){ return a.dist < b.dist; }
      );
      return {best->r, best->c};
    }

    int fr = int(skCenter.y / tilePx), fc = int(skCenter.x / tilePx);
    return {fr, fc};
}
