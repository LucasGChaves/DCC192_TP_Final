#include "UIGameOver.h"
#include "UIButton.h"
#include "UIText.h"
#include "UIImage.h"
#include "../Game.h"

UIGameOver::UIGameOver(Game* game)
    : UIScreen(game, "../Assets/Fonts/PeaberryBase.ttf"), mAlpha(0.0f), mFadeSpeed(0.7f)
{
    mPos = Vector2(0, 0);
    mSize = Vector2(game->GetWindowWidth(), game->GetWindowHeight());
    auto t = AddText("Game Over", Vector2(game->GetWindowWidth()/2 - 200, game->GetWindowHeight()/2 - 120), Vector2(400, 80), 64);

    Vector2 sz = t->GetSize();
    float centeredX = (game->GetWindowWidth() - sz.x) * 0.5f;
    float centeredY = float(game->GetWindowHeight())/2.0f - sz.y/2.0f - 60.f;
    t->SetPosition(Vector2{ centeredX, centeredY - 100});

    auto btn = AddButton("Main Menu", Vector2(game->GetWindowWidth()/2 - 200, game->GetWindowHeight()/2 + 20), Vector2(400, 80), [game, this]() {
        this->Close();
        game->SetGameScene(Game::GameScene::MainMenu);
    });

    Vector2 bSz = btn->GetSize();
    btn->SetPosition(Vector2{(game->GetWindowWidth() - bSz.x) * 0.5f, (game->GetWindowHeight() * 0.5f) - 20.0f});

    game->GetAudio()->PlaySound("SpinalLaugh.wav", false);
}

void UIGameOver::Update(float deltaTime)
{
    if (mAlpha < 1.0f) {
        mAlpha += mFadeSpeed * deltaTime;
        if (mAlpha > 1.0f) mAlpha = 1.0f;
    }
    UIScreen::Update(deltaTime);
}

void UIGameOver::Draw(SDL_Renderer* renderer)
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, static_cast<Uint8>(180 * mAlpha));
    SDL_Rect overlay = {0, 0, static_cast<int>(mSize.x), static_cast<int>(mSize.y)};
    SDL_RenderFillRect(renderer, &overlay);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    UIScreen::Draw(renderer);
}

void UIGameOver::Open() {
    mAlpha = 0.0f;
}
