#include "Skeleton.h"
#include "Player.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/ColliderComponents/AABBColliderComponent.h"
#include "../Components/RigidBodyComponent.h"

Skeleton::Skeleton(Game* game, Player* target, Vector2 pos)
        : Actor(game)
        , mTarget(target)
        , mSpeed(50.0f)
        , mIsDying(false)
        , mLastTargetTile(Vector2{-1.f, -1.f})
{
    mIsLocked = false;
    SetPosition(pos);
    SetScale(Game::SCALE);

    // Draw e animações
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

    // Colisor
    mColliderComponent = new AABBColliderComponent(this, dx, dy, w, h, ColliderLayer::Enemy, false);
}

void Skeleton::OnUpdate(float deltaTime)
{
    // 1) Estado Dying (permanece até destruir)
    if (mState == State::Dying) {
        mDeathTimer -= deltaTime;
        if (mDeathTimer <= 0.0f) {
            SetState(ActorState::Destroy);
        }
        return;
    }

    if (mState == State::Dying || !mTarget) return;

    // 2) Calcula vetor e visibilidade
    Vector2 toPlayer = mTarget ? (mTarget->GetPosition() - mPosition): Vector2::Zero;

    bool canSee = (mTarget != nullptr)
               && InsideFOV(mCurrentDir, toPlayer) //InsideFOV funcionando
               && HasLineOfSight()
               && toPlayer.Length() > 1.0f;

    SDL_Log("InsideFOV: %d", InsideFOV(mCurrentDir, toPlayer));
    SDL_Log("HasLineOfSight: %d", HasLineOfSight());
    SDL_Log("toPlayer.Length() > 1.0f: %d", toPlayer.Length() > 1.0f);
    SDL_Log("canSee: %d", canSee);

    //SDL_Log("angle: %f %f", GetForward().x, GetForward().y);
    //SDL_Log("rotation: %f", Math::ToDegrees(GetRotation()));

    // 3) State machine
    switch (mState)
    {
        case State::Wander:
            if (canSee) {
                // transita para perseguir
                mState = State::Chasing;
                // já reseta last target tile pra forçar recálculo de path
                mLastTargetTile = Vector2{-1.f, -1.f};
            }
            else {
                Wander(deltaTime);
            }
        break;

        case State::Chasing:
            if (!canSee) {
                // perdeu visão, volta a vagar
                mState = State::Wander;
                // zera a velocidade
                mRigidBodyComponent->SetVelocity(Vector2::Zero);
            }
            else {
                // permanece em chase: recálculo e seguimento de path
                Chase(deltaTime, toPlayer);
            }
        break;

        default:
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

bool Skeleton::InsideFOV(Vector2 dir, Vector2 toPlayer) {
    //float angle = Math::ToDegrees(Math::Atan2(-dir.y, dir.x));
    //Vector2 rotation{Math::Cos(angle), -Math::Sin(angle)};
    dir.Normalize();
    toPlayer.Normalize();
    float dot = toPlayer.x * dir.x + toPlayer.y * dir.y;
    return dot >= Math::Cos(mFOVAngle/2.f);
}

bool Skeleton::HasLineOfSight()
{
    //SDL_Log("[0][0]: %d", static_cast<int>(mGame->GetPassableVector()[0][0]));
    // 1) Pega o tamanho de um tile em pixels
    const float tilePx = Game::TILE_SIZE * Game::SCALE;
    //auto& passable     = mGame->GetPassableVector();
    int H              = (int)mGame->GetPassableVector().size();
    int W              = (int)mGame->GetPassableVector()[0].size();

    auto [x, y, w, h] = ComputeColliderParams(Game::TILE_SIZE * 2, Game::TILE_SIZE * 2);

    // 2) Calcula as coordenadas de tile (coluna, linha) do centro do esqueleto e do player
    Vector2 myCenter    = GetPosition() + Vector2{ static_cast<float>(w), static_cast<float>(h) } * 0.5f;
    Vector2 targetCenter = mTarget->GetPosition() + Vector2{ static_cast<float>(w),
                                                            static_cast<float>(h) } * 0.5f;

    int x0 = int(myCenter.x / tilePx);
    int y0 = int(myCenter.y / tilePx);
    int x1 = int(targetCenter.x / tilePx);
    int y1 = int(targetCenter.y / tilePx);

    // 3) Se o destino estiver fora dos limites, sem LOS
    if (x1 < 0 || x1 >= W || y1 < 0 || y1 >= H)
        return false;

    // 4) Setup de Bresenham
    int dx =  abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    // 5) Percorre a linha, pulando o primeiro tile (o próprio esqueleto)
    while (true)
    {
        // Avança para o próximo tile
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }

        // Se chegamos ao destino, LOS livre
        if (x0 == x1 && y0 == y1)
            return true;

        // Se saiu dos limites, bloqueado
        if (x0 < 0 || x0 >= W || y0 < 0 || y0 >= H)
            return false;

        // Se o tile não for atravessável, LOS bloqueada
        if (!mGame->GetPassableVector()[y0][x0])
            return false;
    }
}

// Não apagar por enquanto
// bool Skeleton::HasLineOfSight(Vector2 toPlayer) {
//     // 1) converte para direção unitária e distância
//     float dist = toPlayer.Length();
//     toPlayer.Normalize();
//
//     // 2) número de passos = quantos tiles atravessa
//     float tilePx = Game::TILE_SIZE * Game::SCALE;
//     int steps = int(dist / tilePx) + 1;
//
//     // 3) percorre um passo em cada tile
//     for (int i = 1; i < steps; ++i) {
//         Vector2 sample = mPosition + toPlayer * (i * tilePx);
//
//         // 4) converte amostra pra índices de tile
//         int c = int(sample.x / tilePx);
//         int r = int(sample.y / tilePx);
//
//         // 5) checa se está dentro da matriz
//         //auto& passable = mGame->GetPassableVector();
//         if (r < 0 || r >= mGame->GetPassableVector().size() ||
//             c < 0 || c >= mGame->GetPassableVector()[0].size())
//             return false; // fora do mapa é bloqueio
//
//         // 6) se o tile não for atravessável, linha de visão bloqueada
//         if (!mGame->GetPassableVector()[r][c])
//             return false;
//     }
//     return true;
// }

void Skeleton::InitWander()
{
    // Semente randômica (pode ser static)
    static std::mt19937 rng{ std::random_device{}() };

    // Gera um intervalo aleatório entre 0.5s e 2.0s
    std::uniform_real_distribution<float> distInterval(0.5f, 2.0f);
    mWanderInterval = distInterval(rng);

    // Gera direção aleatória em 4 direções cardeais
    std::uniform_int_distribution<int> distDir(0,3);
    int d = distDir(rng);
    switch(d) {
        case 0: mWanderDir = Vector2{ 1, 0 }; break;  // direita
        case 1: mWanderDir = Vector2{-1, 0 }; break;  // esquerda
        case 2: mWanderDir = Vector2{ 0, 1 }; break;  // baixo
        default:mWanderDir = Vector2{ 0,-1 }; break;  // cima
    }
    mWanderTimer = 0.0f;
    mCurrentDir = mWanderDir;
}

// Função de wander usando RigidBody
void Skeleton::Wander(float deltaTime)
{
    // 1) Atualiza timer
    mWanderTimer += deltaTime;
    if (mWanderTimer >= mWanderInterval) {
        // troca direção e reinicia timer
        InitWander();
    }

    // 2) Aplica velocidade na direção escolhida
    mRigidBodyComponent->SetVelocity(mWanderDir * mSpeed);

    //SDL_Log("angle: %f", Math::ToDegrees(Math::Atan2(-mWanderDir.y, mWanderDir.x)));

    // 3) Escolhe animação conforme mWanderDir
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

void Skeleton::Chase(float deltatime, Vector2 toPlayer) {
    if (toPlayer.Length() <= 1.f) return;

    // 3.1) Índices de tile do Skeleton e do Player
    float tilePx = Game::TILE_SIZE * Game::SCALE;
    int myR = int(mPosition.y / tilePx);
    int myC = int(mPosition.x / tilePx);
    int pR  = int(mTarget->GetPosition().y / tilePx);
    int pC  = int(mTarget->GetPosition().x / tilePx);

    // 3.2) Recalcula rota A* se o player mudou de tile
    if (pR != static_cast<int>(mLastTargetTile.x) || pC != static_cast<int>(mLastTargetTile.y)) {
        mPath = FindPathAStar(
            mGame->GetPassableVector(),
            myR, myC, pR, pC
        );
        mNextWaypoint = 0;
        mLastTargetTile = Vector2{static_cast<float>(pR), static_cast<float>(pC)};
    }

    // 3.3) Segue o próximo waypoint (se houver)
    if (mNextWaypoint < mPath.size()) {
        Vector2 wp   = mPath[mNextWaypoint];
        Vector2 toWP = wp - mPosition;

        // chegou no waypoint?
        if (toWP.Length() < tilePx * 0.1f) {
            ++mNextWaypoint;
        } else {
            // normaliza e aplica velocidade
            mCurrentDir = toWP;
            toWP.Normalize();
            Vector2 vel = toWP * mSpeed;
            mRigidBodyComponent->SetVelocity(vel);

            // 4) MESMA lógica de animação do seu código original,
            //    porém usando toWP em vez de toPlayer
            float diff = std::abs(std::abs(toWP.x) - std::abs(toWP.y));
            if (diff <= 0.5f) {
                SetRotation(toWP.x > 0 ? 0.0f : Math::Pi);
                mDrawComponent->SetAnimation("WalkSide");
            }
            else if (std::abs(toWP.x) > std::abs(toWP.y)) {
                SetRotation(toWP.x > 0 ? 0.0f : Math::Pi);
                mDrawComponent->SetAnimation("WalkSide");
            }
            else if (toWP.y < 0) {
                mDrawComponent->SetAnimation("WalkUp");
            }
            else {
                mDrawComponent->SetAnimation("WalkDown");
            }
        }
    }
}