// FireballSpawner.cpp
#include "FireballSpawner.h"

#include <SDL_log.h>

#include "Projectile.h"
#include "Math.h"

FireballSpawner::FireballSpawner(Game* game, Vector2 center, int count, float delay, float speed, float lifetime)
    : Actor(game)
    , mCenter(center)
    , mTotalCount(count)
    , mSpeed(speed)
    , mLifetime(lifetime)
    // , mDelay(delay)
{
    float angleStep = Math::TwoPi / mTotalCount;

    for (int i = 0; i < mTotalCount; ++i)
    {
        float angle = i * angleStep;

        float radius = 80.0f;
        Vector2 offset(Math::Cos(angle) * radius, Math::Sin(angle) * radius);
        Vector2 spawnPos = mCenter + offset;

        new Projectile(GetGame(), spawnPos, angle, mSpeed, mLifetime);
    }

    SetState(ActorState::Destroy);

    // while (mSpawnedCount < mTotalCount /*&& mTimeSinceLastSpawn >= mDelay*/)
    // {
    //     SDL_Log("Spawn fireball");
    //     float angleStep = Math::TwoPi / mTotalCount;
    //     float angle = mSpawnedCount * angleStep;
    //
    //     float radius = 80.0f;
    //     Vector2 offset(Math::Cos(angle) * radius, Math::Sin(angle) * radius);
    //     Vector2 spawnPos = mCenter + offset;
    //
    //     new Projectile(GetGame(), spawnPos, angle, mSpeed, mLifetime);
    //
    //     mSpawnedCount++;
    //     // mTimeSinceLastSpawn -= mDelay;
    // }

    // if (mSpawnedCount >= mTotalCount)
    // {
    //     SetState(ActorState::Destroy);
    // }
}

// void FireballSpawner::OnUpdate(float deltaTime)
// {
//     // mTimeSinceLastSpawn += deltaTime;
//     //
//     // while (mSpawnedCount < mTotalCount && mTimeSinceLastSpawn >= mDelay)
//     // {
//     //     SDL_Log("Spawn fireball");
//     //     float angleStep = Math::TwoPi / mTotalCount;
//     //     float angle = mSpawnedCount * angleStep;
//     //
//     //     float radius = 80.0f;
//     //     Vector2 offset(Math::Cos(angle) * radius, Math::Sin(angle) * radius);
//     //     Vector2 spawnPos = mCenter + offset;
//     //
//     //     new Projectile(GetGame(), spawnPos, angle, mSpeed, mLifetime);
//     //
//     //     mSpawnedCount++;
//     //     mTimeSinceLastSpawn -= mDelay;
//     // }
//     //
//     // if (mSpawnedCount >= mTotalCount)
//     // {
//     //     SetState(ActorState::Destroy);
//     // }
// }
