#pragma once

#include "Actor.h"
#include <string>
#include <vector>
#include "Player.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawBossAnimatedComponent.h"
#include "../Components/ColliderComponents/AABBColliderComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Random.h"

class Player;
class DrawBossAnimatedComponent;
class AABBColliderComponent;

class Boss : public Actor {

  public:
    Boss(class Game* game, Player* target, Vector2 pos);

    void OnUpdate(float deltaTime) override;
    void OnHorizontalCollision(float overlap, class AABBColliderComponent* other) override;
    void OnVerticalCollision(float overlap, class AABBColliderComponent* other) override;
    void Die();
    void SetSpAttackPos(Vector2 pos) { mSpAttackPos = pos; }
    bool IsDying() const { return mIsDying; }
    void hit() { mLifePoints--; }

  private:
  Player* mTarget;
  float mSpeed;
  bool mIsDying;
  float mDeathTimer = 1.f;
  float mSpAttackTimer = -1.0f;
  float mRunToMiddleTimer = -1.0f;
  Vector2 mSpAttackPos;
  bool mAtSpAttackPos = false;
  bool mChasingPlayer = true;
  float mBeginSpAttackTimer = 1.f;
  int mLifePoints = 10;
  class DrawBossAnimatedComponent* mDrawComponent;
  class AABBColliderComponent* mColliderComponent;
  class RigidBodyComponent* mRigidBodyComponent;

};