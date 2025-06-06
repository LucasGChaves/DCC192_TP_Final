//
// Created by Lucas on 06/06/2025.
//

#include "Spawner.h"
#include "../Game.h"

Spawner::Spawner(Game* game, float spawnDistance)
        :Actor(game)
        ,mSpawnDistance(spawnDistance)
{

}

void Spawner::OnUpdate(float deltaTime)
{
    // TODO
}