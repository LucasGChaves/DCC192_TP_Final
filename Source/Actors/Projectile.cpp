//
// Created by Felipe Gonçalves on 24/06/2025.
//

#include <SDL_log.h>
#include "../Game.h"
#include "Projectile.h"

#include <vector>
#include <vector>

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

    float visualAngle = Math::ToDegrees(mAngle) + 90.0f + 180.0f;
    mDrawComponent = new DrawAnimatedComponent(this, "../Assets/Sprites/Projectile/projectile.png", "../Assets/Sprites/Projectile/projectile.json", 6);
    mDrawComponent->SetAngle(visualAngle);
    mDrawComponent->AddAnimation("Flaming", {0,1,2,3,4,5,6,7});
    mDrawComponent->SetAnimation("Flaming");
    mDrawComponent->SetAnimFPS(10.0f);

    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 0.0f, false, 1);
    mColliderComponent = new AABBColliderComponent(this, 0, 0, 15, 15, ColliderLayer::Projectile, false, 1);
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

void Projectile::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    auto player = dynamic_cast<Player*> (other->GetOwner());
    if (player)
    {
        player->Hit();
    }
    mColliderComponent->SetEnabled(false);
    mRigidBodyComponent->SetEnabled(false);
    SetState(ActorState::Destroy);
}

void Projectile::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) {
    auto player = dynamic_cast<Player*> (other->GetOwner());
    if (player)
    {
        player->Hit();
    }
    mColliderComponent->SetEnabled(false);
    mRigidBodyComponent->SetEnabled(false);
    SetState(ActorState::Destroy);
}