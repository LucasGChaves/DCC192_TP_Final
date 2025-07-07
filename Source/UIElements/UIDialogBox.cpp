#include "UIDialogBox.h"
#include "../Game.h"
#include "UIText.h"

UIDialogBox::UIDialogBox(Game* game, const std::string& fontName)
    : UIScreen(game, fontName)
{
    std::array<int, 4> boxColor = {233, 155, 99, 220};
    std::array<int, 4> borderColor = {60, 30, 0, 255};

    std::string introText = "A dread omen looms... A cavernous, ominous sound echoes from the darkness below.";

    int winW = mGame->GetWindowWidth();
    int winH = mGame->GetWindowHeight();
    Vector2 size(winW, winH / 3.0f);
    Vector2 pos(50, winH - size.y +140); 

    auto* t = AddText(introText, pos, size, 36, winW - 100);
    t->SetBox(boxColor, borderColor, 3);
}

void UIDialogBox::Update(float deltaTime)
{
    UIScreen::Update(deltaTime);
}

void UIDialogBox::SetVisible(bool visible)
{
    mIsVisible = visible;
}
