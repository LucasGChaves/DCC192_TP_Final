#include "AABBColliderComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include <algorithm>

// Mapeamento de camadas que devem ignorar colisões entre si
const std::map<ColliderLayer, const std::set<ColliderLayer>> ColliderIgnoreMap = {
        { ColliderLayer::Player, {} },
        { ColliderLayer::Enemy, {} },
        { ColliderLayer::Blocks, { ColliderLayer::Blocks } },
        { ColliderLayer::Pole, {} },
        { ColliderLayer::PlayerAttack, { ColliderLayer::Player, ColliderLayer::Blocks, ColliderLayer::Pole } } // ✅
};

AABBColliderComponent::AABBColliderComponent(
        class Actor* owner, int dx, int dy, int w, int h,
        ColliderLayer layer, bool isStatic, int updateOrder)
        : Component(owner, updateOrder)
        , mOffset(Vector2((float)dx, (float)dy))
        , mIsStatic(isStatic)
        , mWidth(w)
        , mHeight(h)
        , mLayer(layer)
        , mIsEnabled(true)
{
}

AABBColliderComponent::~AABBColliderComponent() {}

Vector2 AABBColliderComponent::GetMin() const
{
    return mOwner->GetPosition() + mOffset;
}

Vector2 AABBColliderComponent::GetMax() const
{
    return GetMin() + Vector2((float)mWidth, (float)mHeight);
}

Vector2 AABBColliderComponent::GetCenter() const
{
    return GetMin() + Vector2((float)mWidth / 2.0f, (float)mHeight / 2.0f);
}

bool AABBColliderComponent::Intersect(const AABBColliderComponent& b) const
{
    return (GetMin().x < b.GetMax().x && GetMax().x > b.GetMin().x &&
            GetMin().y < b.GetMax().y && GetMax().y > b.GetMin().y);
}

float AABBColliderComponent::GetMinHorizontalOverlap(AABBColliderComponent* b) const
{
    float right = GetMax().x - b->GetMin().x;
    float left = GetMin().x - b->GetMax().x;
    return (Math::Abs(left) < Math::Abs(right)) ? left : right;
}

float AABBColliderComponent::GetMinVerticalOverlap(AABBColliderComponent* b) const
{
    float top = GetMin().y - b->GetMax().y;
    float down = GetMax().y - b->GetMin().y;
    return (Math::Abs(top) < Math::Abs(down)) ? top : down;
}

float AABBColliderComponent::DetectHorizontalCollision(RigidBodyComponent* rigidBody)
{
    if (!mIsEnabled) return 0.0f;

    auto colliders = mOwner->GetGame()->GetNearbyColliders(mOwner->GetPosition());

    std::sort(colliders.begin(), colliders.end(), [this](AABBColliderComponent* a, AABBColliderComponent* b) {
        return (a->GetCenter() - GetCenter()).LengthSq() < (b->GetCenter() - GetCenter()).LengthSq();
    });

    for (auto& collider : colliders)
    {
        if (collider == this || !collider->IsEnabled()) continue;

        if (ColliderIgnoreMap.at(mLayer).count(collider->GetLayer()) > 0)
            continue;

        if (Intersect(*collider))
        {
            float overlap = GetMinHorizontalOverlap(collider);
            if (!mIsStatic && rigidBody && collider->GetLayer() != ColliderLayer::PlayerAttack) {
                ResolveHorizontalCollisions(rigidBody, overlap);
            }

            mOwner->OnHorizontalCollision(overlap, collider);
            return overlap;
        }
    }

    return 0.0f;
}

float AABBColliderComponent::DetectVertialCollision(RigidBodyComponent* rigidBody)
{
    if (!mIsEnabled) return 0.0f;

    auto colliders = mOwner->GetGame()->GetNearbyColliders(mOwner->GetPosition());

    std::sort(colliders.begin(), colliders.end(), [this](AABBColliderComponent* a, AABBColliderComponent* b) {
        return (a->GetCenter() - GetCenter()).LengthSq() < (b->GetCenter() - GetCenter()).LengthSq();
    });

    for (auto& collider : colliders)
    {
        if (collider == this || !collider->IsEnabled()) continue;

        if (ColliderIgnoreMap.at(mLayer).count(collider->GetLayer()) > 0)
            continue;

        if (Intersect(*collider))
        {
            float overlap = GetMinVerticalOverlap(collider);
            if (!mIsStatic && rigidBody && collider->GetLayer() != ColliderLayer::PlayerAttack) {
                ResolveVerticalCollisions(rigidBody, overlap);
            }

            mOwner->OnVerticalCollision(overlap, collider);
            return overlap;
        }
    }

    return 0.0f;
}

void AABBColliderComponent::ResolveHorizontalCollisions(RigidBodyComponent* rigidBody, float overlap)
{
    if (!rigidBody) return;
    mOwner->SetPosition(mOwner->GetPosition() - Vector2(overlap, 0.0f));
    rigidBody->SetVelocity(Vector2(0.0f, rigidBody->GetVelocity().y));
}

void AABBColliderComponent::ResolveVerticalCollisions(RigidBodyComponent* rigidBody, float overlap)
{
    if (!rigidBody) return;
    mOwner->SetPosition(mOwner->GetPosition() - Vector2(0.0f, overlap));
    rigidBody->SetVelocity(Vector2(rigidBody->GetVelocity().x, 0.0f));

    if (overlap > 0.0f)
    {
        mOwner->SetOnGround();
    }
}

void AABBColliderComponent::SetEnabled(bool enabled)
{
    mIsEnabled = enabled;
}

bool AABBColliderComponent::IsEnabled() const
{
    return mIsEnabled;
}

ColliderLayer AABBColliderComponent::GetLayer() const
{
    return mLayer;
}