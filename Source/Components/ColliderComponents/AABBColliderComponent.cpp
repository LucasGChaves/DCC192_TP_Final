//
// Created by Lucas N. Ferreira on 28/09/23.
//

#include "AABBColliderComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"

AABBColliderComponent::AABBColliderComponent(class Actor* owner, int dx, int dy, int w, int h,
    ColliderLayer layer, bool isStatic, int updateOrder) :
    Component(owner, updateOrder),
    mOffset(Vector2((float)dx, (float)dy)),
    mIsStatic(isStatic),
    mWidth(w),
    mHeight(h),
    mLayer(layer) {
        mOwner->GetGame()->AddCollider(this);
}

AABBColliderComponent::~AABBColliderComponent() {
    mOwner->GetGame()->RemoveCollider(this);
}

Vector2 AABBColliderComponent::GetMin() const {
    //TODO
}

Vector2 AABBColliderComponent::GetMax() const {
    //TODO
}

bool AABBColliderComponent::Intersect(const AABBColliderComponent& b) const {
    //TODO
}

float AABBColliderComponent::GetMinVerticalOverlap(AABBColliderComponent* b) const {
    //TODO
}

float AABBColliderComponent::GetMinHorizontalOverlap(AABBColliderComponent* b) const {
    //TODO
}

float AABBColliderComponent::DetectHorizontalCollision(RigidBodyComponent *rigidBody) {
    //TODO

    if (mIsStatic) return false;

    auto colliders = mOwner->GetGame()->GetColliders();

    //TODO

    return 0.0f;
}

float AABBColliderComponent::DetectVertialCollision(RigidBodyComponent *rigidBody) {
    //TODO

    if (mIsStatic) return false;

    auto colliders = mOwner->GetGame()->GetColliders();

    //TODO

    return 0.0f;
}

void AABBColliderComponent::ResolveHorizontalCollisions(RigidBodyComponent *rigidBody, const float minXOverlap) {
    //TODO
}

void AABBColliderComponent::ResolveVerticalCollisions(RigidBodyComponent *rigidBody, const float minYOverlap) {
    //TODO
}