//
// Created by Lucas on 29/06/2025.
//

#include "InvisibleWall.h"

#include "Player.h"

InvisibleWall::InvisibleWall(Game* game, Vector2 pos, int width, int height, std::string sceneString, std::string topOrBottom)
    : Actor(game) {

    if (sceneString == "scene1") {
         mScene = Game::GameScene::Level1;
    }
    else if (sceneString == "scene2") {
        mScene = Game::GameScene::Level2;
    }

    if (topOrBottom == "top") {
        mType = Type::Top;
    }
    else if (topOrBottom == "bottom") {
        mType = Type::Bottom;
    }

    SetPosition(pos);
    SetScale(Game::SCALE);
    mColliderComponent = new AABBColliderComponent(this, 0, 0, width, height,
        ColliderLayer::InvisibleWall, true);
}

void InvisibleWall::OnUpdate(float deltaTime) {
    if (mGame->GetPlayer() && (mGame->GetPlayer()->GetScore() == mGame->GetNumSkeletons() && !mIsColliding)) {
        mColliderComponent->SetEnabled(true);
    }
}