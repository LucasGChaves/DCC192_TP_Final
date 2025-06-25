//
// Created by Felipe Gonçalves on 24/06/2025.
//

#include "Attack.h"

#include <SDL_log.h>

#include "../Components/DrawComponents/DrawPolygonComponent.h"

Attack::Attack(Game *game, Vector2 position) : Actor(game) {
    SetPosition(position);

    mColliderComponent = new AABBColliderComponent(this, 0, 0, 32, 32, ColliderLayer::PlayerAttack, false);
    std::vector<Vector2> a = {
        position,
        Vector2(position.x + 32, position.y),
        Vector2(position.x + 32, position.y + 32),
        Vector2(position.x, position.y + 32),
    };
    mDrawPolygonComponent = new DrawPolygonComponent(this, a);
}
