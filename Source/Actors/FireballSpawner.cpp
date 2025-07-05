// FireballSpawner.cpp
#include "FireballSpawner.h"

#include <SDL_log.h>

#include "Projectile.h"
#include "Math.h"

FireballSpawner::FireballSpawner(Game* game, Vector2 center, int count, float delay, float speed, float lifetime)
    : Actor(game)
    , mCenter(center)
    , mTotalCount(count)
    , mDelay(delay)
    , mSpeed(speed)
    , mLifetime(lifetime)
{
    SDL_Log("Spawner criado com %d fireballs", count);

}

void FireballSpawner::OnUpdate(float deltaTime)
{
    mTimeSinceLastSpawn += deltaTime;

    while (mSpawnedCount < mTotalCount && mTimeSinceLastSpawn >= mDelay)
    {
        float angleStep = Math::TwoPi / mTotalCount;
        float angle = mSpawnedCount * angleStep;

        float radius = 10.0f;
        Vector2 offset(Math::Cos(angle) * radius, Math::Sin(angle) * radius);
        Vector2 spawnPos = mCenter + offset;

        SDL_Log("Criando fireball %d em (%.2f, %.2f) com ângulo %.2f", mSpawnedCount, spawnPos.x, spawnPos.y, angle);

        new Projectile(GetGame(), spawnPos, angle, mSpeed, mLifetime);

        mSpawnedCount++;
        mTimeSinceLastSpawn -= mDelay;
    }

    if (mSpawnedCount >= mTotalCount)
    {
        SetState(ActorState::Destroy);
    }
}


// void FireballSpawner::OnUpdate(float deltaTime)
// {
//     SDL_Log("Spawner ativo: %d/%d", mSpawnedCount, mTotalCount);
//
//     mTimeSinceLastSpawn += deltaTime;
//
//     if (mSpawnedCount < mTotalCount && mTimeSinceLastSpawn >= mDelay)
//     {
//         float angleStep = Math::TwoPi / mTotalCount;
//         float angle = mSpawnedCount * angleStep;
//
//         float radius = 10.0f;
//         Vector2 offset(Math::Cos(angle) * radius, Math::Sin(angle) * radius);
//         Vector2 spawnPos = mCenter + offset;
//
//         new Projectile(GetGame(), spawnPos, angle, mSpeed, mLifetime);
//
//         mSpawnedCount++;
//         mTimeSinceLastSpawn = 0.0f;
//     }
//
//     if (mSpawnedCount >= mTotalCount)
//     {
//         SetState(ActorState::Destroy); // Destrói o spawner quando terminar
//     }
// }
