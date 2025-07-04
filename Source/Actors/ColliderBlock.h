//
// Created by Lucas on 26/06/2025.
//

#pragma once
#include "../Game.h"
#include "Actor.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"

class ColliderBlock : public Actor {
    public:
        explicit ColliderBlock(Game* game,  Vector2 pos, Vector2 srcPos, int width, int height,
            SDL_Texture* texture, float angle = Math::ToRadians(0), SDL_RendererFlip flip = SDL_FLIP_NONE);

    private:
        class AABBColliderComponent* mColliderComponent;
};
