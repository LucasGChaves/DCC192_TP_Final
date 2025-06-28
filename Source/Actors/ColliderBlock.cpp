//
// Created by Lucas on 26/06/2025.
//

#include "ColliderBlock.h"

ColliderBlock::ColliderBlock(Game* game, Vector2 pos, Vector2 srcPos, int width, int height, SDL_Texture* texture)
    : Actor(game) {

    SetPosition(pos);
    SetScale(Game::SCALE);

    new DrawSpriteComponent(this, "", width, height, 10, true, srcPos, texture);
    mColliderComponent = new AABBColliderComponent(this, 0, 0, width, height, ColliderLayer::Blocks, true);
}

