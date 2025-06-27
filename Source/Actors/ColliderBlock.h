//
// Created by Lucas on 26/06/2025.
//

#pragma once
#include "../Game.h"
#include "Actor.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"

class ColliderBlock : public Actor {
    public:
        explicit ColliderBlock(Game* game,  Vector2 pos, Vector2 srcPos, SDL_Texture* texture);

    private:
        class AABBColliderComponent* mColliderComponent;
};
