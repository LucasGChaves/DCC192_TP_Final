// FireballSpawner.h
#ifndef FIREBALLSPAWNER_H
#define FIREBALLSPAWNER_H

#include "Actor.h"
#include <vector>

class FireballSpawner : public Actor {
public:
    FireballSpawner(Game* game, Vector2 center, int count, float delay, float speed = 300.0f, float lifetime = 2.0f);
    // void OnUpdate(float deltaTime) override;

private:
    Vector2 mCenter;
    int mTotalCount;
    int mSpawnedCount = 0;
    float mSpeed;
    float mLifetime;
    // float mDelay;
    // float mTimeSinceLastSpawn = 0.0f;
};

#endif
