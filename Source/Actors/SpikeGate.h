//
// Created by Lucas on 26/06/2025.
//

#pragma once
#include "../Game.h"
#include "Actor.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"

class SpikeGate : public Actor {
    public:
        explicit SpikeGate(Game* game,  Vector2 pos, int width, int height);

        void OnUpdate(float deltaTime) override;

    private:
        class DrawAnimatedComponent* mDrawComponent;
        class AABBColliderComponent* mColliderComponent;
};
