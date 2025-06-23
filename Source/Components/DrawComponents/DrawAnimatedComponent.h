#pragma once
#include "DrawSpriteComponent.h"
#include <string>
#include <unordered_map>
#include <vector>

class DrawAnimatedComponent : public DrawSpriteComponent
{
public:
    DrawAnimatedComponent(class Actor* owner, const std::string& spriteSheetPath, const std::string& spriteSheetData, int drawOrder = 100);
    ~DrawAnimatedComponent();

    void Draw(SDL_Renderer* renderer, const Vector3& modColor) override;
    void Update(float deltaTime) override;

    void AddAnimation(const std::string& name, const std::vector<int>& spriteNums);
    void SetAnimation(const std::string& name);
    const std::string& GetAnimationName() const { return mAnimName; }

    void SetAnimFPS(float fps) { mAnimFPS = fps; }

    const std::vector<std::string>& GetFrameNames() const { return mFrameNames; }
    std::string GetFrameName(int index) const { return mFrameNames[index]; }
    void ForceSetAnimation(const std::string& name);
    bool IsAnimationFinished() const;
    const std::vector<SDL_Rect*>& GetSpriteSheetData() const { return mSpriteSheetData; }

private:
    void LoadSpriteSheet(const std::string& texturePath, const std::string& dataPath);

    std::unordered_map<std::string, std::vector<int>> mAnimations;
    std::string mAnimName;
    float mAnimFPS = 10.0f;
    float mAnimTimer = 0.0f;

    SDL_Texture* mSpriteSheetSurface;
    std::vector<SDL_Rect*> mSpriteSheetData;
    std::vector<std::string> mFrameNames;
    bool mIsPaused = false;
};