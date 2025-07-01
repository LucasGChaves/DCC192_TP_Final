//
// Created by Lucas on 26/06/2025.
//

#include "ColliderBlock.h"

ColliderBlock::ColliderBlock(Game* game, Vector2 pos, Vector2 srcPos, int width, int height, SDL_Texture* texture,
    float angle, SDL_RendererFlip flip)
    : Actor(game) {

    SetPosition(pos);
    SetScale(Game::SCALE);
    SetRotation(angle);
    SetFlip(flip);

    new DrawSpriteComponent(this, "", width, height, 1, true, srcPos, texture);
    mColliderComponent = new AABBColliderComponent(this, 0, 0, width, height, ColliderLayer::Blocks, true);
}

