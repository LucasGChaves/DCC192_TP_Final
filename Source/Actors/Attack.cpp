//
// Created by Felipe Gonçalves on 24/06/2025.
//

#include "Attack.h"

#include <SDL_log.h>

#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/ColliderComponents/AABBColliderComponent.h"
#include "../Game.h"
#include "Skeleton.h"

Attack::Attack(Game *game, Vector2 base, Vector2 forward) : Actor(game) {
    float w = (std::abs(forward.y) == 1 ? 2.f : 1.f) * 16.f * Game::SCALE;
    float h = (std::abs(forward.x) == 1 ? 2.f : 1.f) * 16.f * Game::SCALE;

    float forwardX = 16.f * (forward.x == 1.f ? 2.f : forward.x);
    float forwardY = 16.f * (forward.y == 1.f ? 2.f : forward.y);

    auto position = base + Vector2(forwardX, forwardY);
    
    SetPosition(position);

    mColliderComponent = new AABBColliderComponent(this, 0, 0, w, h, ColliderLayer::PlayerAttack, false);

    for (auto actor : game->GetNearbyActors(GetPosition(), 2)) {
        if (auto* enemy = dynamic_cast<Skeleton*>(actor)) {
            if (enemy->GetColliderComponent() && mColliderComponent->Intersect(*enemy->GetColliderComponent())) {
                enemy->Die();
            }
        }
    }
}
