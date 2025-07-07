#pragma once
#include "UIScreen.h"

class UIDialogBox : public UIScreen {
public:
    UIDialogBox(class Game* game, const std::string& fontName);
    void Update(float deltaTime) override;
    void SetVisible(bool visible);
private:
    // Removed timer and expired state; visibility is managed externally
};
