//
// Created by Lucas N. Ferreira on 08/12/23.
//

#include "HUD.h"
#include "Game.h"
#include "UIElements/UIText.h"

HUD::HUD(class Game* game, const std::string& fontName)
    :UIScreen(game, fontName)
{
    hudScale = 2.0f;
    hudPos = Vector2(0.0f, 0.0f);

    Vector2 imageSize = Vector2(HUD_WIDTH, HUD_HEIGHT) * hudScale;
    AddImage(mGame->GetRenderer(), "../Assets/Images/ruffQuestHudSkel.png", hudPos, imageSize);

    SetLives(5);
    Vector2 textSize = Vector2(CHAR_WIDTH, WORD_HEIGHT) * hudScale;
    mScoreCounter = AddText("00", Vector2(85.0f, 32.0f) + (hudPos * hudScale), textSize, POINT_SIZE);
    // mScoreCounter = AddText("00", Vector2(WORD _OFFSET, HUD_POS_Y + WORD_HEIGHT), textSize, POINT_SIZE);
}

HUD::~HUD()
{

}

void HUD::SetTime(int time)
{
    // TODO
}

void HUD::SetLevelName(const std::string &levelName)
{
    // TODO
}

Vector2 HUD::GetHeartPosition(int index) const
{
    Vector2 padding(HEART_PADDING_X, HEART_PADDING_Y);
    Vector2 baseOffset(HEART_POS_X, HEART_POS_Y);
    return hudPos + (baseOffset + static_cast<float>(index) * padding) * hudScale;
}


void HUD::SetLives(int lives)
{
    for (auto* heart : mHearts)
    {
        if (auto it = std::find(mImages.begin(), mImages.end(), heart); it != mImages.end())
        {
            delete *it;
            mImages.erase(it);
        }
    }
    mHearts.clear();

    for (int i = 0; i < lives; ++i)
    {
        Vector2 pos = GetHeartPosition(i);
        Vector2 size = Vector2(HEART_WIDTH, HEART_HEIGHT) * hudScale;

        if (UIImage* heart = AddImage(mGame->GetRenderer(), "../Assets/Images/heart.png", pos, size))
        {
            mHearts.push_back(heart);
        }
        else
        {
            SDL_Log("Error creating heart %d", i);
        }
    }
}

