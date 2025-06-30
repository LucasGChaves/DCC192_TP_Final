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
    explicit InvisibleWall(Game* game,  Vector2 pos, int width, int height, std::string sceneString);

    Game::GameScene GetInvisibleWallScene() {return mScene;}
private:
    class AABBColliderComponent* mColliderComponent;
    Game::GameScene mScene;
};
