//
// Created by Felipe Gonçalves on 24/06/2025.
//

#include "Attack.h"

#include <SDL_log.h>

#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/ColliderComponents/AABBColliderComponent.h"
#include "../Game.h"
#include "Skeleton.h"

Attack::Attack(Game *game, const std::vector<Vector2>& polygon) : Actor(game) {
    // Draw the polygon for visualization
    mDrawPolygonComponent = new DrawPolygonComponent(this, const_cast<std::vector<Vector2>&>(polygon));
    // Use the bounding box of the polygon for collision
    float minX = polygon[0].x, minY = polygon[0].y, maxX = polygon[0].x, maxY = polygon[0].y;
    for (const auto& v : polygon) {
        if (v.x < minX) minX = v.x;
        if (v.x > maxX) maxX = v.x;
        if (v.y < minY) minY = v.y;
        if (v.y > maxY) maxY = v.y;
    }
    float w = maxX - minX;
    float h = maxY - minY;
    SetPosition(Vector2(minX, minY));
    mColliderComponent = new AABBColliderComponent(this, 0, 0, w, h, ColliderLayer::PlayerAttack, false);
    // Use GetNearbyActors to find enemies
    for (auto actor : game->GetNearbyActors(GetPosition(), 2)) {
        if (auto* enemy = dynamic_cast<Skeleton*>(actor)) {
            if (enemy->GetColliderComponent() && mColliderComponent->Intersect(*enemy->GetColliderComponent())) {
                enemy->Die();
            }
        }
    }
}
