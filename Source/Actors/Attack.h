//
// Created by Felipe Gonçalves on 24/06/2025.
//

#ifndef ATTACK_H
#define ATTACK_H
#include "Actor.h"


class Attack : public Actor {
public:
    explicit Attack(Game* game, Vector2 base, Vector2 forward, ColliderLayer ownerLayer);

private:
    class AABBColliderComponent* mColliderComponent;
};



#endif //ATTACK_H
