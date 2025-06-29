//
// Created by Lucas on 29/06/2025.
//

#include "InvisibleWall.h"

InvisibleWall::InvisibleWall(Game* game, Vector2 pos, int width, int height, std::string sceneString)
    : Actor(game) {

    if (sceneString == "scene1") {
         mScene = Game::GameScene::Level1;
    }
    else if (sceneString == "scene2") {
        mScene = Game::GameScene::Level2;
    }

    SetPosition(pos);
    SetScale(Game::SCALE);
    mColliderComponent = new AABBColliderComponent(this, 0, 0, width, height, ColliderLayer::InvisibleWall, true);
    }