//
// Created by Lucas N. Ferreira on 08/12/23.
//

#include "HUD.h"
#include "Game.h"
#include "Actors/Player.h"
#include "UIElements/UIText.h"

HUD::HUD(class Game* game, const std::string& fontName)
    :UIScreen(game, fontName)
    , mCurrentHearts(-1)
    , mlastDrawnLives(-1)
{
    hudScale = 2.0f;
    hudPos = Vector2(0.0f, 0.0f);

    Vector2 imageSize = Vector2(HUD_WIDTH, HUD_HEIGHT) * hudScale;
    AddImage(mGame->GetRenderer(), "../Assets/Images/ruffQuestHudSkel.png", hudPos, imageSize);

    Vector2 textSize = Vector2(CHAR_WIDTH, WORD_HEIGHT) * hudScale;
    Vector2 offset = Vector2(85.0f, 32.0f);
    Vector2 textPos = (offset * hudScale) + hudPos;
    mScoreCounter = AddText("00", textPos, textSize, POINT_SIZE);

    // Vector2 textSize = Vector2(CHAR_WIDTH, WORD_HEIGHT) * hudScale;
    // mScoreCounter = AddText("00", Vector2(85.0f, 32.0f) + (hudPos * hudScale), textSize, POINT_SIZE);
    // mScoreCounter = AddText("00", Vector2(WORD _OFFSET, HUD_POS_Y + WORD_HEIGHT), textSize, POINT_SIZE);
}

HUD::~HUD()
{

}

void HUD::Update(float deltaTime)
{
    int hearts = mGame->GetPlayer()->GetHearts();

    if (hearts < 0)  hearts = 0;
    if (hearts > 5) hearts = 5;
    if (hearts != mCurrentHearts) mCurrentHearts = hearts;
}

void HUD::Draw(SDL_Renderer *renderer)
{
    if (mCurrentHearts != mlastDrawnLives)
    {
        mlastDrawnLives = mCurrentHearts;
        UpdateHeartImages(renderer);
    }

    UIScreen::Draw(renderer);
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

void HUD::UpdateHeartImages(SDL_Renderer* renderer)
{
    for (auto* heart : mHearts)
    {
        // Remove o heart de mImages e deleta
        auto it = std::find(mImages.begin(), mImages.end(), heart);
        if (it != mImages.end())
        {
            delete *it;
            mImages.erase(it);
        }
    }
    mHearts.clear();

    for (int i = 0; i < mCurrentHearts; ++i)
    {
        Vector2 pos = GetHeartPosition(i);
        Vector2 size = Vector2(HEART_WIDTH, HEART_HEIGHT) * hudScale;

        if (UIImage* heart = AddImage(renderer, "../Assets/Images/heart.png", pos, size))
        {
            mHearts.push_back(heart);
        }
        else
        {
            SDL_Log("Error creating heart %d", i);
        }
    }
}
