#include "DrawAnimatedComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../../Json.h"
#include <fstream>
#include <algorithm>

DrawAnimatedComponent::DrawAnimatedComponent(class Actor* owner,
                                             const std::string& spriteSheetPath,
                                             const std::string& spriteSheetData,
                                             int drawOrder)
    : DrawSpriteComponent(owner, spriteSheetPath, 0, 0, drawOrder)
    , mSpriteSheetSurface(nullptr)
{
    LoadSpriteSheet(spriteSheetPath, spriteSheetData);
}

DrawAnimatedComponent::~DrawAnimatedComponent()
{
    for (auto rect : mSpriteSheetData)
    {
        delete rect;
    }
    mSpriteSheetData.clear();
    mFrameNames.clear();
}

void DrawAnimatedComponent::LoadSpriteSheet(const std::string& texturePath, const std::string& dataPath)
{
    mSpriteSheetSurface = mOwner->GetGame()->LoadTexture(texturePath);

    std::ifstream file(dataPath);
    nlohmann::json data = nlohmann::json::parse(file);

    for (const auto& frame : data["frames"])
    {
        int x = frame["frame"]["x"].get<int>();
        int y = frame["frame"]["y"].get<int>();
        int w = frame["frame"]["w"].get<int>();
        int h = frame["frame"]["h"].get<int>();

        SDL_Rect* rect = new SDL_Rect({x, y, w, h});
        mSpriteSheetData.emplace_back(rect);
        mFrameNames.emplace_back(frame["filename"].get<std::string>());
    }
}

void DrawAnimatedComponent::Draw(SDL_Renderer* renderer, const Vector3& modColor)
{
    if (!mIsVisible) return;
    if (mSpriteSheetData.empty() || mAnimations.find(mAnimName) == mAnimations.end()) return;

    int frameIndex = static_cast<int>(mAnimTimer);
    const auto& frames = mAnimations[mAnimName];
    if (frameIndex >= static_cast<int>(frames.size())) return;

    SDL_Rect* srcRect = mSpriteSheetData[frames[frameIndex]];

    SDL_Rect dstRect = {
            static_cast<int>(mOwner->GetPosition().x - mOwner->GetGame()->GetCameraPos().x),
            static_cast<int>(mOwner->GetPosition().y - mOwner->GetGame()->GetCameraPos().y),
            srcRect->w * Game::SCALE,
            srcRect->h * Game::SCALE
    };

    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if (mOwner->GetRotation() == Math::Pi)
        flip = SDL_FLIP_HORIZONTAL;

    SDL_SetTextureBlendMode(mSpriteSheetSurface, SDL_BLENDMODE_BLEND);
    SDL_SetTextureColorMod(mSpriteSheetSurface,
                           static_cast<Uint8>(modColor.x),
                           static_cast<Uint8>(modColor.y),
                           static_cast<Uint8>(modColor.z));

    SDL_RenderCopyEx(renderer, mSpriteSheetSurface, srcRect, &dstRect, 0.0, nullptr, flip);
}

void DrawAnimatedComponent::ForceSetAnimation(const std::string& name)
{
    if (mAnimName != name && mAnimations.find(name) != mAnimations.end())
    {
        mAnimName = name;
        mAnimTimer = 0.0f;
    }
    else if (mAnimations.find(name) == mAnimations.end())
    {
        SDL_Log("Animacao '%s' nao encontrada!", name.c_str());
    }
}

void DrawAnimatedComponent::Update(float deltaTime)
{
    if (mIsPaused || mAnimations.find(mAnimName) == mAnimations.end()) return;

    const auto& frames = mAnimations[mAnimName];
    if (frames.empty()) return;

    mAnimTimer += mAnimFPS * deltaTime;

    if (mAnimTimer >= frames.size())
    {
        mAnimTimer -= static_cast<float>(frames.size());
    }
}

void DrawAnimatedComponent::SetAnimation(const std::string& name)
{
    if (mAnimName != name && mAnimations.find(name) != mAnimations.end())
    {
        mAnimName = name;
        mAnimTimer = 0.0f;
    }
}

void DrawAnimatedComponent::AddAnimation(const std::string& name, const std::vector<int>& spriteNums)
{
    mAnimations[name] = spriteNums;
}

bool DrawAnimatedComponent::IsAnimationFinished() const
{
    if (mAnimations.find(mAnimName) == mAnimations.end())
        return true;

    const auto& frames = mAnimations.at(mAnimName);
    return static_cast<int>(mAnimTimer) >= static_cast<int>(frames.size()) - 1;
}