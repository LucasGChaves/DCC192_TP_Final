#include "UIWinScreen.h"
#include "../Game.h"
#include "UIText.h"

UIWinScreen::UIWinScreen(Game* game, const std::string& fontName)
    : UIScreen(game, fontName)
{
    Vector2 center(game->GetWindowWidth() / 2.0f, game->GetWindowHeight() / 2.0f);
    Vector2 textSize(600, 60);
    AddText("Thank you for playing!", center - Vector2(300, 60), textSize, 48);
    AddText("Game by Adalberto, Felipe, Iago and Lucas", center + Vector2(-300, 20), textSize, 32);
    AddText("Press ESC to go back to the menu", center + Vector2(-300, 80), textSize, 28);
}
