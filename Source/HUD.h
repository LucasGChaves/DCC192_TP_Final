//
// Created by Lucas N. Ferreira on 08/12/23.
//

#pragma once

#include <string>

#include "UIElements/UIScreen.h"

class HUD : public UIScreen
{
public:
    const int POINT_SIZE = 48;
    const int HUD_POS_Y = 10.0f;
    const float WORD_HEIGHT = 24.0f;
    const float CHAR_WIDTH = 20.0f;
    const float WORD_OFFSET = 85.0f;

    const float HUD_WIDTH = 144.0f;
    const float HUD_HEIGHT = 64.0f;
    const float HEART_WIDTH = 14.0f;
    const float HEART_HEIGHT = 15.0f;
    const float HEART_POS_X = 57.0f;
    const float HEART_POS_Y = 16.0f;
    const float HEART_PADDING_X = 15.0f;
    const float HEART_PADDING_Y = 0.0f;

    HUD(class Game* game, const std::string& fontName);
    ~HUD();

    // Reinsert the HUD elements
    void SetTime(int time);

    void SetLevelName(const std::string& levelName);

    Vector2 GetHeartPosition(int index) const;
    void SetLives(int lives);

private:
    // HUD elements
    UIText* mScoreCounter;
    UIText* mLevelName;
    UIText* mLivesCount;
    UIText* mTimeText;

    float hudScale;
    Vector2 hudPos;
    std::vector<UIImage*> mHearts;
};