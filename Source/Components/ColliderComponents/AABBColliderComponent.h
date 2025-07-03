#pragma once

#include "../Component.h"
#include "../../Math.h"
#include "../RigidBodyComponent.h"
#include <vector>
#include <map>
#include <set>
#include <algorithm>

enum class ColliderLayer
{
    Player,
    Enemy,
    Boss,
    Blocks,
    InvisibleWall,
    PlayerAttack, // ✅ Adicionado
    EnemyAttack
};

// Define os pares de camadas que se ignoram
extern const std::map<ColliderLayer, const std::set<ColliderLayer>> ColliderIgnoreMap;

class AABBColliderComponent : public Component
{
public:
    AABBColliderComponent(class Actor* owner, int dx, int dy, int w, int h,
                          ColliderLayer layer, bool isStatic = false, int updateOrder = 10);
    ~AABBColliderComponent() override;

    // Colisão AABB básica
    bool Intersect(const AABBColliderComponent& b) const;

    // Checagem de colisões em ambos os eixos
    float DetectHorizontalCollision(class RigidBodyComponent* rigidBody);
    float DetectVertialCollision(class RigidBodyComponent* rigidBody);

    // Acessores utilitários
    void SetEnabled(bool enabled);
    bool IsEnabled() const;
    void SetStatic(bool isStatic) { mIsStatic = isStatic; }
    ColliderLayer GetLayer() const;
    void SetSize(Vector2 pos) { mWidth = pos.x; mHeight = pos.y; } //Public?
    void SetOffset(Vector2 offset) {mOffset.x = offset.x; mOffset.y = offset.y; }

    // AABB básico
    Vector2 GetMin() const;
    Vector2 GetMax() const;
    Vector2 GetCenter() const;

private:
    float GetMinHorizontalOverlap(AABBColliderComponent* b) const;
    float GetMinVerticalOverlap(AABBColliderComponent* b) const;

    void ResolveHorizontalCollisions(class RigidBodyComponent* rigidBody, float minXOverlap);
    void ResolveVerticalCollisions(class RigidBodyComponent* rigidBody, float minYOverlap);

    Vector2 mOffset;
    int mWidth;
    int mHeight;
    bool mIsStatic;
    bool mIsEnabled;

    ColliderLayer mLayer;
};