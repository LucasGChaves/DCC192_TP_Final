//
// Created by Felipe Gonçalves on 24/06/2025.
//

#ifndef PROJECTILE_H
#define PROJECTILE_H
#include "Actor.h"


class Projectile : public Actor {
public:
    explicit Projectile(Game* game, Vector2 pos, float angle, float speed = 300.0f, float lifetime = 2.0f);
    void OnUpdate(float deltaTime) override;

private:
    float mAngle = 0.0f;
    float mSpeed = 300.0f;
    float mLifeTime = 2.0f;
    float mElapsedTime = 0.0f;

    class RigidBodyComponent* mRigidBodyComponent;
    // class AABBColliderComponent* mColliderComponent;
    class DrawAnimatedComponent* mDrawComponent;
};

#endif //PROJECTILE_H
