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

    new DrawSpriteComponent(this, "", width, height, 100, true, srcPos, texture);
    mColliderComponent = new AABBColliderComponent(this, 0, 0, width, height, ColliderLayer::Blocks, true);
    // std::vector<Vector2> vertices= {
    //     Vector2(0, 0), Vector2(static_cast<float>(width), 0),
    //     Vector2(static_cast<float>(width), static_cast<float>(height)), Vector2(0, static_cast<float>(height))
    // };
    // new DrawPolygonComponent(this, vertices);
}

