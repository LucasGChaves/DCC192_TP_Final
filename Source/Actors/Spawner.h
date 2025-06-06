//
// Created by Lucas on 06/06/2025.
//

#pragma once

#include "Actor.h"

class Spawner : public Actor
{
public:
    explicit Spawner(Game* game, float spawnDistance);

    void OnUpdate(float deltaTime) override;
private:
    float mSpawnDistance;
};