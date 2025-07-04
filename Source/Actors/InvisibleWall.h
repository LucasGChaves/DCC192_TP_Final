//
// Created by Lucas on 29/06/2025.
//

#pragma once
#include "../Game.h"
#include "Actor.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"

class InvisibleWall : public Actor {
public:
    enum class Type {
        Top,
        Bottom
    };

    explicit InvisibleWall(Game* game,  Vector2 pos, int width, int height, std::string sceneString, std::string topOrBottom);

    Game::GameScene GetInvisibleWallScene() {return mScene;}
    void SetType(Type type) { mType = type; }
    Type GetType() { return mType; }
    void SetColliding(bool colliding) {mIsColliding = colliding;}
    bool IsColliding() {return mIsColliding; }
    void OnUpdate(float deltaTime) override;
private:
    class AABBColliderComponent* mColliderComponent;
    Game::GameScene mScene;
    Type mType;
    bool mIsColliding = false;

};
