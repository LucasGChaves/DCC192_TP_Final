//
// Created by Lucas N. Ferreira on 28/09/23.
//

#include "DrawSpriteComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"

DrawSpriteComponent::DrawSpriteComponent(class Actor* owner, const std::string &texturePath,
    const int width, const int height, const int drawOrder) :
        DrawComponent(owner, drawOrder),
        mWidth(width),
        mHeight(height) {
            //TODO
        }

void DrawSpriteComponent::Draw(SDL_Renderer *renderer) {
    //TODO
}