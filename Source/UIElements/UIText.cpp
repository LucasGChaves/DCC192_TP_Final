//
// Created by Lucas N. Ferreira on 22/05/25.
//

#include "UIText.h"
#include "UIFont.h"

UIText::UIText(const std::string &text, class UIFont* font, int pointSize, const unsigned wrapLength,
               const Vector2 &pos, const Vector2 &size, const Vector3 &color)
        :UIElement(pos, size, color)
        ,mFont(font)
        ,mPointSize(pointSize)
        ,mWrapLength(wrapLength)
        //,mTextTexture(nullptr)
{
    SetText(text);
}

UIText::~UIText()
{
}

void UIText::SetText(const std::string &text)
{
    // 1) Limpa texturas antigas
    for (auto t : mLineTextures) SDL_DestroyTexture(t);
    mLineTextures.clear();
    mLineSizes.clear();

    // 2) Quebra o texto em linhas
    std::stringstream ss(text);
    std::string line;
    int maxWidth = 0;
    int totalH   = 0;
    while (std::getline(ss, line, '\n'))
    {
        // renderiza **essa linha** com o seu font e wrapLength
        SDL_Texture* tex = mFont->RenderText(line, mColor, mPointSize, mWrapLength);
        if (!tex)
        {
            SDL_Log("UIText::SetText: falha em RenderText para \"%s\"", line.c_str());
            continue;
        }
        // mede a textura
        int w,h;
        SDL_QueryTexture(tex, nullptr, nullptr, &w, &h);

        mLineTextures.push_back(tex);
        mLineSizes.emplace_back(float(w), float(h));

        maxWidth = std::max(maxWidth, w);
        totalH  += h + mLineSpacing;
    }
    if (!mLineSizes.empty()) totalH -= mLineSpacing; // retira o último espaçamento

    // 3) Ajusta o tamanho do UIElement
    mSize.x = float(maxWidth);
    mSize.y = float(totalH);
}

// void UIText::SetText(const std::string &text)
// {
//     if (mTextTexture){
//         SDL_DestroyTexture(mTextTexture);
//         mTextTexture = nullptr;
//     }
//
//     mTextTexture = mFont->RenderText(text, mColor, mPointSize, mWrapLength);
//
//     // int tw, th;
//     // SDL_QueryTexture(mTextTexture, nullptr, nullptr, &tw, &th);
//     // // define o tamanho real do texto
//     // mSize.x = static_cast<float>(tw);
//     // mSize.y = static_cast<float>(th);
//
//     if (!mTextTexture){
//         SDL_Log("Failed to create text texture for text: %s", text.c_str());
//         return;
//     }
//
//     mText = text;
// }

void UIText::Draw(SDL_Renderer* renderer, const Vector2& screenPos)
{
    if (mLineTextures.empty()) return;

    int paddingX = 0;
    int paddingY = 0;

    if (mHasBox)
    {
        paddingX = 50;
        paddingY = 50;
        SDL_Rect box{
            int(screenPos.x + mPosition.x) - paddingX,
            int(screenPos.y + mPosition.y) - paddingY,
            int(mSize.x) + 2*paddingX,
            int(mSize.y) + 2*paddingY
        };

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer,
            mBoxColor[0],
            mBoxColor[1],
            mBoxColor[2],
            mBoxColor[3]
        );
        SDL_RenderFillRect(renderer, &box);

        // borda
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        SDL_SetRenderDrawColor(renderer,
            mBorderColor[0],
            mBorderColor[1],
            mBorderColor[2],
            mBorderColor[3]
        );
        for (int i = 0; i < mBorderWidth; ++i)
        {
            SDL_Rect r{
                box.x + i,
                box.y + i,
                box.w - 2*i,
                box.h - 2*i
            };
            SDL_RenderDrawRect(renderer, &r);
        }
    }

    float cursorY = screenPos.y + mPosition.y;
    for (size_t i = 0; i < mLineTextures.size(); ++i)
    {
        const auto& sz = mLineSizes[i];
        float lineX = screenPos.x + mPosition.x - paddingX + ((mSize.x) + (2*paddingX) - sz.x) * 0.5f;
        SDL_Rect dst{
            int(lineX),
            int(cursorY),
            int(sz.x),
            int(sz.y)
        };
        SDL_RenderCopy(renderer, mLineTextures[i], nullptr, &dst);
        cursorY += sz.y + mLineSpacing;
    }
}


void UIText::SetBox(std::array<int,4> boxColor, std::array<int,4> borderColor, int borderWidth)
{
    mHasBox = true;
    mBoxColor = boxColor;
    mBorderColor = borderColor;
    mBorderWidth = borderWidth;
}