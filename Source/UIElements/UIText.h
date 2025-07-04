//
// Created by Lucas N. Ferreira on 22/05/25.
//

#pragma once

#include <string>
#include <sstream>
#include <SDL.h>
#include "../Math.h"
#include "UIElement.h"
#include <array>
#include <vector>

class UIText : public UIElement {
public:
    UIText(const std::string& text, class UIFont* font, int pointSize = 40, const unsigned wrapLength = 1024,
           const Vector2 &pos = Vector2::Zero, const Vector2 &size = Vector2(100.f, 20.0f), const Vector3& color = Color::White);

    ~UIText();

    void SetText(const std::string& name);
    void Draw(SDL_Renderer* renderer, const Vector2 &screenPos) override;
    void SetBox(std::array<int,4> boxColor, std::array<int,4> borderColor, int borderWidth = 2);

    const Vector2& GetPosition() const { return mPosition; }
    void SetPosition(const Vector2& pos) { mPosition = pos; }
    const Vector2& GetSize()     const { return mSize; }

protected:
    std::string mText;
    class UIFont* mFont;
    //SDL_Texture *mTextTexture;

    unsigned int mPointSize;
    unsigned int mWrapLength;

    bool mHasBox = false;
    std::array<int, 4> mBoxColor = {0,0,0,0};
    std::array<int, 4> mBorderColor = {1,1,1,1};
    int mBorderWidth = 2;

    std::vector<SDL_Texture*> mLineTextures;
    std::vector<Vector2>      mLineSizes;    // w/h de cada linha
    int                       mLineSpacing = 4; // pixels entre linhas
};