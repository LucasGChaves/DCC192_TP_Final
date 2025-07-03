//
// Created by Felipe Gonçalves on 24/06/2025.
//

#include <SDL_log.h>
#include "../Game.h"
#include "Projectile.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/ColliderComponents/AABBColliderComponent.h"


Projectile::Projectile(Game* game, Vector2 pos, float angle, float speed, float lifetime)
    : Actor(game)
    , mAngle(angle)
    , mSpeed(speed)
    , mLifeTime(lifetime)
{
    SetPosition(pos);

    mDrawComponent = new DrawAnimatedComponent(this, "../Assets/Sprites/Projectile/projectile.png", "../Assets/Sprites/Projectile/projectile.json", 6);
    mDrawComponent->SetAngle(angle);
    mDrawComponent->AddAnimation("Flaming", {0,1,2,3,4,5,6,7});
    mDrawComponent->SetAnimation("Flaming");
    mDrawComponent->SetAnimFPS(10.0f);

    mRigidBodyComponent = new RigidBodyComponent(this);
    // mColliderComponent = new AABBColliderComponent(this, 0, 0, 6, 15, ColliderLayer::EnemyAttack, false);
}

void Projectile::OnUpdate(float deltaTime)
{
    mElapsedTime += deltaTime;
    if (mElapsedTime >= mLifeTime) {
        SetState(ActorState::Destroy);
        return;
    }

    Vector2 velocity(
        Math::Cos(mAngle) * mSpeed,
        Math::Sin(mAngle) * mSpeed
    );

    mRigidBodyComponent->SetVelocity(velocity);
}
