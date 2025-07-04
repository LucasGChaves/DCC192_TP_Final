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
                              const std::string& spriteSheetData, int drawOrder = 100, int updateOrder = 100);
  ~DrawBossAnimatedComponent();

  void Draw(SDL_Renderer* renderer, const Vector3& modColor) override;

  Vector2 GetDefaultFrameSize() { return mDefaultFrameSize; }
  Vector2 GetSpAttackFrameSize() { return mSpAttackFrameSize; }

  void SetDefaultFrameSize(Vector2 frameSize) { mDefaultFrameSize = frameSize; }
  void SetDefaultSpAttackFrameSize(Vector2 frameSize) { mSpAttackFrameSize = frameSize; }

  Vector2 GetFrameSize(int frameIdx);

  private:
    float mOriginalSize;
    Vector2 mDefaultFrameSize;
    Vector2 mSpAttackFrameSize;
};