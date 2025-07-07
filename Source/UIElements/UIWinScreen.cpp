#include "UIWinScreen.h"
#include "../Game.h"
#include "UIText.h"

UIWinScreen::UIWinScreen(Game* game, const std::string& fontName)
    : UIScreen(game, fontName)
{
    std::array<int, 4> boxColor = {233, 155, 99, 100};
    std::array<int, 4> borderColor = {60, 30, 0, 255};

    auto* t = AddText("Thank you for playing!\n Press ESC to go back to the menu",
        Vector2::Zero, Vector2{600,200}, 40, 1024);

    Vector2 sz = t->GetSize();

    t->SetPosition(Vector2{(mGame->GetWindowWidth() - sz.x) * 0.5f, (mGame->GetWindowHeight() - sz.y) * 0.7f});
    t->SetBox(boxColor, borderColor, 3);

    // Do not erase this comment yet.

    // unsigned wrapLen = 600;
    // Vector2 dims{ float(wrapLen), 200.0f };
    //
    // Vector2 center(game->GetWindowWidth() / 2.0f, game->GetWindowHeight() / 2.0f);
    // //Vector2 textSize(600, 60);
    //
    // auto text = AddText("Thank you for playing!\n Press ESC to go back to the menu",
    //     center - Vector2(300, 60), dims, 48, 600);
    // //auto t2 = AddText("", center + Vector2(-300, 20), textSize, 28);
    // text->SetBox(boxColor, borderColor);
    //
    // Vector2 textSize = text->GetSize();
    // float winW = float(mGame->GetWindowWidth());
    // float winH = float(mGame->GetWindowHeight());
    // // posição de modo que o centro do texto coincida com o centro da janela:
    // Vector2 centeredPos{
    //     (winW - textSize.x) / 2.0f,
    //     (winH - textSize.y) / 2.0f
    //   };
    // text->SetPosition(centeredPos);
}
