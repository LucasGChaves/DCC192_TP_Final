#pragma once
#include "UIScreen.h"
#include <SDL.h>

class UIGameOver : public UIScreen {
public:
    UIGameOver(class Game* game);
    void Update(float deltaTime) override;
    void Draw(SDL_Renderer* renderer) override;
    void Open();
    bool IsFullyVisible() const { return mAlpha >= 1.0f; }
private:
    float mAlpha; // 0.0 (invisible) to 1.0 (fully visible)
    float mFadeSpeed; // How fast the overlay appears
};
