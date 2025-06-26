//
// Created by Lucas N. Ferreira on 08/12/23.
//

#include "HUD.h"
#include "Game.h"
#include "Actors/Player.h"
#include "UIElements/UIText.h"
#include <iomanip>
#include <sstream>

using namespace HUDConstants;

HUD::HUD(class Game* game, const std::string& fontName)
    :UIScreen(game, fontName)
    , hudScale(2.0f)
    , hudPos(Vector2(0.0f, 0.0f))
    , mCurrentHearts(-1)
    , mLastDrawnHearts(-1)
{
    Vector2 textSize = kCharSize * hudScale;
    Vector2 textPos = (kScoreOffset * hudScale) + hudPos;
    mScoreCounter = AddText("00", textPos, textSize, kPointSize);
}

HUD::~HUD()
{

}

void HUD::Update(float deltaTime)
{
    if (mGame->GetGamePlayState() == Game::GamePlayState::Playing)
    {
        int hearts = mGame->GetPlayer()->GetHearts();
        if (hearts < 0)  hearts = 0;
        if (hearts > 5) hearts = 5;

        if (hearts != mCurrentHearts)
        {
            mCurrentHearts = hearts;
        }

        // int score = mGame->GetPlayer()->GetScore(); // aguardando
        // if (score < 0)  score = 0; // just for caution
        static int fakeScore = 0;
        fakeScore += 1;
        SetScore(fakeScore);
    }
}

void HUD::Draw(SDL_Renderer *renderer)
{
    if (!mBaseImageLoaded)
    {
        Vector2 imageSize = kHudSize * hudScale;
        AddImage(renderer, "../Assets/Images/ruffQuestHudSkel.png", hudPos, imageSize);
        mBaseImageLoaded = true;
    }

    if (mCurrentHearts != mLastDrawnHearts)
    {
        mLastDrawnHearts = mCurrentHearts;
        UpdateHeartImages(renderer);
    }

    UIScreen::Draw(renderer);
}

void HUD::SetScore(int score)
{
    if (score < 0) score = 0; // just for caution
    if (score > 99) score = 99; // just for caution

    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << score;
    std::string scoreStr = oss.str();

    mScoreCounter->SetText(scoreStr);
}


Vector2 HUD::GetHeartPosition(int index) const
{
    return hudPos + (kHeartBasePos + static_cast<float>(index) * kHeartPadding) * hudScale;
}

void HUD::UpdateHeartImages(SDL_Renderer* renderer)
{
    for (auto* heart : mHearts)
    {
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
        Vector2 size = kHeartSize * hudScale;

        UIImage* heart = AddImage(renderer, "../Assets/Images/heart.png", pos, size);
        mHearts.push_back(heart);
    }
}
