//
// Created by Lucas on 26/06/2025.
//

#include "ColliderBlock.h"

ColliderBlock::ColliderBlock(Game* game, Vector2 pos, Vector2 srcPos, SDL_Texture* texture)
    : Actor(game) {

    SetPosition(pos);
    SetScale(Game::SCALE);
    new DrawSpriteComponent(this, "", Game::TILE_SIZE, Game::TILE_SIZE, 10, true, srcPos, texture);
    //mColliderComponent = new AABBColliderComponent(this, 0, 0, Game::TILE_SIZE, Game::TILE_SIZE, ColliderLayer::Blocks, true);
}

