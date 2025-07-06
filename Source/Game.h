// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
//
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <SDL.h>
#include <vector>
#include <unordered_map>
#include "AudioSystem.h"
#include "Math.h"
#include "MapHelper.h"
#include "SpatialHashing.h"

class Game {
public:
    static const int LEVEL_WIDTH = 46;
    static const int LEVEL_HEIGHT = 23;
    static const int TILE_SIZE = 16;
    static const int SPAWN_DISTANCE = 700;
    static const int TRANSITION_TIME = 1;

    //Screen dimension constants
    static const int SCREEN_WIDTH = 1400;
    static const int SCREEN_HEIGHT = 800;

    static const int SCALE = 3;
    //static const int SCALE = 1;

    enum class GameScene
    {
        MainMenu,
        Level1,
        Level2,
        Level3
    };

    enum class SceneManagerState
    {
        None,
        Entering,
        Active,
        Exiting
    };

    enum class GamePlayState
    {
        Playing,
        Paused,
        GameOver,
        LevelComplete,
        Leaving,
        EnteringMap
    };

    enum class FadeState {
        FadeOut,
        FadeIn,
        None
    };

    Game(int windowWidth, int windowHeight);

    bool Initialize();
    void RunLoop();
    void Shutdown();
    void Quit() { mIsRunning = false; }

    // Actor functions
    void UpdateActors(float deltaTime);
    void AddActor(class Actor* actor);
    void RemoveActor(class Actor* actor);
    void ProcessInputActors();
    void HandleKeyPressActors(const int key, const bool isPressed);

    // Level functions
    void LoadMainMenu();
    void LoadLevel(const std::string& levelName, const int levelWidth, const int levelHeight);

    std::vector<Actor *> GetNearbyActors(const Vector2& position, const int range = 1);
    std::vector<class AABBColliderComponent *> GetNearbyColliders(const Vector2& position, const int range = 2);

    void Reinsert(Actor* actor);

    // Camera functions
    Vector2& GetCameraPos() { return mCameraPos; };
    void SetCameraPos(const Vector2& position) { mCameraPos = position; };

    // Audio functions
    class AudioSystem* GetAudio() { return mAudio; }

    // UI functions
    void PushUI(class UIScreen* screen) { mUIStack.emplace_back(screen); }
    const std::vector<class UIScreen*>& GetUIStack() { return mUIStack; }

    // Window functions
    int GetWindowWidth() const { return mWindowWidth; }
    int GetWindowHeight() const { return mWindowHeight; }

    // Loading functions
    class UIFont* LoadFont(const std::string& fileName);
    SDL_Texture* LoadTexture(const std::string& texturePath);

    void SetGameScene(GameScene scene, float transitionTime = .0f);
    void ResetGameScene(float transitionTime = .0f);
    void UnloadScene();

    void SetBackgroundImage(const std::string& imagePath, const Vector2 &position = Vector2::Zero, const Vector2& size = Vector2::Zero);
    void TogglePause();

    GameScene GetGameScene() { return mGameScene; }

    // Game-specific
    const class Player* GetPlayer() { return mPlayer; }
    const class Boss* GetBoss() { return mBoss; }
    SpatialHashing* GetSpatialHashing() { return mSpatialHashing; }
    std::vector<std::vector<bool>> GetPassableVector() { return mPassable; }
    std::vector<std::vector<bool>> GetPassable2x2Vector() { return mPassable2x2; }
    void SetPassable2x2Vector();

    void SetGamePlayState(GamePlayState state) { mGamePlayState = state; }
    GamePlayState GetGamePlayState() const { return mGamePlayState; }

    void DecreaseSkeletonNum();

    UIScreen* CreatePauseMenu();

    int GetNumSkeletons() const { return mSkeletonNum; }

    MapData* mTileMap;
    int mSkeletonNum;

    void SetSpikeGateLowered(bool cond) {mIsSpikeGateLowered = cond;}
    bool GetSpikeGateLowered() {return mIsSpikeGateLowered;}

    const class Dog* GetDog() { return mDog; }

    const class InvisibleWall* GetTopInvisibleWall() { return mTopInvisibleWall; }
    const class InvisibleWall* GetBottomInvisibleWall() { return mBottomInvisibleWall; }

private:
    void ProcessInput();
    void UpdateGame();
    void UpdateCamera();
    void GenerateOutput();

    // Scene Manager
    void UpdateSceneManager(float deltaTime);
    void ChangeScene();
    SceneManagerState mSceneManagerState;
    float mSceneManagerTimer;


    // Load the level from a CSV file as a 2D array
    int **ReadLevelData(const std::string& fileName, int width, int height);
    void BuildLevel(int** levelData, int width, int height);

    // Spatial Hashing for collision detection
    class SpatialHashing* mSpatialHashing;

    void BuildActorsFromMap();

    void HandleVolumeLevelDuringPause(bool resumingGame);

    void GetPassablePos();

    int GetStaticObjectsLayer();

    // All the UI elements
    std::vector<class UIScreen*> mUIStack;
    std::unordered_map<std::string, class UIFont*> mFonts;

    // SDL stuff
    SDL_Window* mWindow;
    SDL_Renderer* mRenderer;
    AudioSystem* mAudio;

    // Window properties
    int mWindowWidth;
    int mWindowHeight;

    // Track elapsed time since game start
    Uint32 mTicksCount;

    // Track actors state
    bool mIsRunning;
    GamePlayState mGamePlayState;

    // Track level state
    GameScene mGameScene;
    GameScene mNextScene;

    // Background and camera
    Vector3 mBackgroundColor;
    Vector3 mModColor;
    Vector2 mCameraPos;

    // Game-specific
    class Player *mPlayer;
    class Dog *mDog;
    class InvisibleWall *mTopInvisibleWall;
    class InvisibleWall *mBottomInvisibleWall;
    class HUD *mHUD;
    class Boss *mBoss;
    SoundHandle mMusicHandle;

    float mGameTimer;
    int mGameTimeLimit;

    FadeState mFadeState;
    float mFadeTime;

    SDL_Texture *mBackgroundTexture;
    Vector2 mBackgroundSize;
    Vector2 mBackgroundPosition;

    bool mShowWinScreen = true;

    bool mIsSpikeGateLowered = false;

    std::vector<std::vector<bool>> mPassable;
    std::vector<std::vector<bool>> mPassable2x2;
    int mStaticBlocksLayerIdx = -1;
};
