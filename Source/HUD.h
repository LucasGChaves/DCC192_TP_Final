//
// Created by Lucas N. Ferreira on 08/12/23.
//

#pragma once

#include <string>
#include "UIElements/UIScreen.h"

namespace HUDConstants
{
    constexpr float kHudWidth = 144.0f;
    constexpr float kHudHeight = 64.0f;
    inline const Vector2 kHudSize = Vector2(kHudWidth, kHudHeight);

    constexpr float kCharWidth = 20.0f;
    constexpr float kWordHeight = 20.0f;
    constexpr int   kPointSize = 34;
    inline const Vector2 kCharSize = Vector2(kCharWidth, kWordHeight);
    inline const Vector2 kScoreOffset = Vector2(85.0f, 34.0f);

    constexpr float kHeartWidth = 14.0f;
    constexpr float kHeartHeight = 15.0f;
    constexpr float kHeartPosX = 57.0f;
    constexpr float kHeartPosY = 16.0f;
    constexpr float kHeartPaddingX = 15.0f;
    constexpr float kHeartPaddingY = 0.0f;
    constexpr int   kMaxHearts = 5;

    inline const Vector2 kHeartSize = Vector2(kHeartWidth, kHeartHeight);
    inline const Vector2 kHeartBasePos = Vector2(kHeartPosX, kHeartPosY);
    inline const Vector2 kHeartPadding = Vector2(kHeartPaddingX, kHeartPaddingY);
}


class HUD : public UIScreen
{
public:
    HUD(class Game* game, const std::string& fontName);
    ~HUD();

    void Update(float deltaTime) override;
    void Draw(class SDL_Renderer *renderer) override;

    void SetScore(int score);

private:
    void UpdateHeartImages(SDL_Renderer* renderer);
    Vector2 GetHeartPosition(int index) const;

    float hudScale;
    Vector2 hudPos;

    int mCurrentHearts;
    int mLastDrawnHearts;

    std::vector<UIImage*> mHearts;
    UIText* mScoreCounter;

    bool mBaseImageLoaded = false;
};