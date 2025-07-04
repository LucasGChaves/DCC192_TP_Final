#pragma once
#include "UIScreen.h"

class UIWinScreen : public UIScreen {
public:
    UIWinScreen(class Game* game, const std::string& fontName);
};
