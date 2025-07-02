#pragma once
#include "DrawAnimatedComponent.h"
#include <string>
#include <unordered_map>
#include <vector>
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../../Json.h"
#include <fstream>
#include <cmath>

class DrawBossAnimatedComponent : public DrawAnimatedComponent
{
  public:
  DrawBossAnimatedComponent(class Actor* owner, const std::string& spriteSheetPath,
                              const std::string& spriteSheetData, Vector2 originalSize, int drawOrder = 100);
  ~DrawBossAnimatedComponent();

  void Draw(SDL_Renderer* renderer, const Vector3& modColor) override;
  Vector2 GetFrameSize(int frameIdx);
  private:
    Vector2 mOriginalSize;
    Vector2 mOriginalFrameCenter;
};