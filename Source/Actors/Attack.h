//
// Created by Felipe Gonçalves on 24/06/2025.
//

#ifndef ATTACK_H
#define ATTACK_H
#include "Actor.h"


class Attack : public Actor {
public:
    explicit Attack(Game* game, Vector2 position);

private:
    class AABBColliderComponent* mColliderComponent;
    class DrawPolygonComponent *mDrawPolygonComponent;
};



#endif //ATTACK_H
