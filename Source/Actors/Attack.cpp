//
// Created by Felipe Gonçalves on 24/06/2025.
//

#include "Attack.h"

#include <SDL_log.h>

#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/ColliderComponents/AABBColliderComponent.h"
#include "../Game.h"
#include "Skeleton.h"
#include "Boss.h"

Attack::Attack(Game *game, Vector2 base, Vector2 forward, ColliderLayer ownerLayer) : Actor(game) {
    const float w = (forward.y != 0 ? 2.f : 1.f) * 16.f * Game::SCALE;
    const float h = (forward.x != 0 ? 2.f : 1.f) * 16.f * Game::SCALE;

    const auto position = base + Vector2(forward.x, forward.y);
    
    SetPosition(position);

    // std::vector<Vector2> positions = {
    //     Vector2(0, 0),
    //     Vector2(w, 0),
    //     Vector2(w, h),
    //     Vector2(0, h),
    // };
    //
    // new DrawPolygonComponent(this, positions);

    mColliderComponent = new AABBColliderComponent(this, 0, 0, w, h, ColliderLayer::PlayerAttack, false);

    for (const auto actor : game->GetNearbyActors(GetPosition(), 2)) {
        if (ownerLayer == ColliderLayer::Player) {
            if (auto* enemy = dynamic_cast<Skeleton*>(actor)) {
                if (enemy->GetColliderComponent() && mColliderComponent->Intersect(*enemy->GetColliderComponent())) {
                    enemy->Die();
                }
            }
            else if (auto* boss = dynamic_cast<Boss*>(actor)) {
                auto aabb = boss->GetComponent<AABBColliderComponent>();
                if (aabb && mColliderComponent->Intersect(*aabb) &&
                    (!boss->IsAtSpAttackPos() || (boss->IsAtSpAttackPos() && boss->GetSpAttackTimer() <= 0.f))) {
                    boss->Hit();
                    }
            }
        }

        if (ownerLayer == ColliderLayer::Boss) {
            if (auto* player = dynamic_cast<Player*>(actor)) {
                const auto aabb = player->GetComponent<AABBColliderComponent>();
                if (aabb && mColliderComponent->Intersect(*aabb)) {
                    player->Hit();
                }
            }
        }
    }
}
