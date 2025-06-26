// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
//
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "UIScreen.h"
#include "../Game.h"
#include "UIFont.h"

UIScreen::UIScreen(Game* game, const std::string& fontName)
        :mGame(game)
        ,mPos(0.f, 0.f)
        ,mSize(0.f, 0.f)
        ,mState(UIState::Active)
        ,mSelectedButtonIndex(-1)
{
    SDL_Log("Construtor UIScreen: Objeto %p está se auto-adicionando à pilha.", this);
    mGame->PushUI(this);

    mFont = mGame->LoadFont(fontName);
}

UIScreen::~UIScreen()
{
    for(auto ptr: mTexts){
        delete ptr;
    }
    mTexts.clear();

    for(auto ptr: mButtons){
        delete ptr;
    }
    mButtons.clear();

    for(auto ptr: mImages){
        delete ptr;
    }
    mImages.clear();
}

void UIScreen::Update(float deltaTime)
{

}

void UIScreen::Draw(SDL_Renderer *renderer)
{
    for(auto ptr: mImages){
        ptr->Draw(renderer, mPos);
    }

    for(auto ptr: mTexts){
        ptr->Draw(renderer, mPos);
    }

    for(auto ptr: mButtons){
        ptr->Draw(renderer, mPos);
    }



}

void UIScreen::ProcessInput(const uint8_t* keys)
{

}

void UIScreen::HandleKeyPress(int key)
{
    if (mState != UIState::Active)
        return;

    if (mButtons.empty())
        return;
    if (mSelectedButtonIndex >= 0 && mSelectedButtonIndex < static_cast<int>(mButtons.size())){
        mButtons[mSelectedButtonIndex]->SetHighlighted(false);
    }
    if (key == SDLK_w){
        mSelectedButtonIndex--;
        if (mSelectedButtonIndex < 0){
            mSelectedButtonIndex = static_cast<int>(mButtons.size()) - 1;
        }
    }else if (key == SDLK_s){
        mSelectedButtonIndex++;
        if (mSelectedButtonIndex >= static_cast<int>(mButtons.size())){
            mSelectedButtonIndex = 0;
        }
    }else if (key == SDLK_RETURN){
        if (mSelectedButtonIndex >= 0 && mSelectedButtonIndex < static_cast<int>(mButtons.size())){
            mButtons[mSelectedButtonIndex]->OnClick();
        }
        return;
    }

    if (mSelectedButtonIndex >= 0 && mSelectedButtonIndex < static_cast<int>(mButtons.size())){
        mButtons[mSelectedButtonIndex]->SetHighlighted(true);
    }

}

void UIScreen::Close()
{
    mState = UIState::Closing;
}

UIText* UIScreen::AddText(const std::string &name, const Vector2 &pos, const Vector2 &dims, const int pointSize, const int unsigned wrapLength)
{
    UIText* t = new UIText(name, mFont, pointSize, wrapLength, pos, dims );
    mTexts.push_back(t);
    return t;
}

UIButton* UIScreen::AddButton(const std::string& name, const Vector2 &pos, const Vector2& dims, std::function<void()> onClick)
{
    Vector3 buttonColor(1.0f, 0.5f, 0.0f);
    Vector3 textColor(1.0f, 1.0f, 1.0f);
    auto* b = new UIButton(name, mFont, onClick, pos, dims, buttonColor, 30, 1024);
    mButtons.emplace_back(b);

    if (mButtons.size() == 1){
        mSelectedButtonIndex = 0;
        b->SetHighlighted(true);
    }

    return b;
}

UIImage* UIScreen::AddImage(SDL_Renderer *renderer, const std::string &imagePath, const Vector2 &pos, const Vector2 &dims, const Vector3 &color)
{

    auto* img = new UIImage(renderer, imagePath, pos, dims, color);
    mImages.emplace_back(img);
    return img;
}