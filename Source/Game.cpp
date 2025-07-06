// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
//
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "CSV.h"
#include "Random.h"
#include "Game.h"
#include "HUD.h"
#include "SpatialHashing.h"
#include "Actors/Actor.h"
#include "Actors/Skeleton.h"
#include "Actors/Player.h"
#include "Actors/ColliderBlock.h"
#include "Actors/InvisibleWall.h"
#include "Actors/SpikeGate.h"
#include "Actors/Dog.h"
#include "Actors/Boss.h"
#include "UIElements/UIScreen.h"
#include "UIElements/UIWinScreen.h"
#include "Components/DrawComponents/DrawComponent.h"
#include "Components/DrawComponents/DrawSpriteComponent.h"
#include "Components/DrawComponents/DrawPolygonComponent.h"
#include "Components/ColliderComponents/AABBColliderComponent.h"

Game::Game(int windowWidth, int windowHeight)
        :mWindow(nullptr)
        ,mRenderer(nullptr)
        ,mTicksCount(0)
        ,mIsRunning(true)
        ,mWindowWidth(windowWidth)
        ,mWindowHeight(windowHeight)
        ,mPlayer(nullptr)
        ,mHUD(nullptr)
        ,mBoss(nullptr)
        ,mTopInvisibleWall(nullptr)
        ,mBottomInvisibleWall(nullptr)
        ,mBackgroundColor(0, 0, 0)
        ,mModColor(255, 255, 255)
        ,mCameraPos(Vector2::Zero)
        ,mAudio(nullptr)
        ,mGameTimer(0.0f)
        ,mGameTimeLimit(0)
        ,mSceneManagerTimer(0.0f)
        ,mSceneManagerState(SceneManagerState::None)
        ,mGameScene(GameScene::MainMenu)
        ,mNextScene(GameScene::Level1)
        ,mBackgroundTexture(nullptr)
        ,mBackgroundSize(Vector2::Zero)
        ,mBackgroundPosition(Vector2::Zero)
        ,mFadeState(FadeState::None)
        ,mFadeTime(0.f)
        ,mTileMap(nullptr)
        ,mSkeletonNum(0)
{

}

bool Game::Initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    mWindow = SDL_CreateWindow("TP Final: A Ruff Quest", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, mWindowWidth, mWindowHeight, 0);
    if (!mWindow)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!mRenderer)
    {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return false;
    }

    if (IMG_Init(IMG_INIT_PNG) == 0)
    {
        SDL_Log("Unable to initialize SDL_image: %s", SDL_GetError());
        return false;
    }

    // Initialize SDL_ttf
    if (TTF_Init() != 0)
    {
        SDL_Log("Failed to initialize SDL_ttf");
        return false;
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
    {
        SDL_Log("Failed to initialize SDL_mixer");
        return false;
    }

    // Start random number generator
    Random::Init();

    mAudio = new AudioSystem(8);

    mSpatialHashing = new SpatialHashing(TILE_SIZE * 4.0f * SCALE,
                                         LEVEL_WIDTH * TILE_SIZE * SCALE,
                                         LEVEL_HEIGHT * TILE_SIZE * SCALE);

    mTicksCount = SDL_GetTicks();

    // Init all game actors
    SetGameScene(GameScene::MainMenu);

    return true;
}

void Game::SetGameScene(Game::GameScene scene, float transitionTime)
{
    if (mSceneManagerState != SceneManagerState::None){
        SDL_Log("SetGameScene: Scene transition already in progress.");
        return;
    }
    if (scene == GameScene::MainMenu || scene == GameScene::Level1 || scene == GameScene::Level2 || scene == GameScene::Level3){
        mNextScene = scene;
        mSceneManagerState = SceneManagerState::Entering;
        mSceneManagerTimer = transitionTime;
    }else{
        SDL_Log("SetGameScene: Invalid scene passed.");
        return;
    }
}

void Game::ResetGameScene(float transitionTime)
{
    SetGameScene(mGameScene, transitionTime);
}

void Game::ChangeScene()
{
    // Unload current Scene
    UnloadScene();

    // Reset camera position
    mCameraPos.Set(0.0f, 0.0f);

    // Reset game timer
    mGameTimer = 0.0f;

    // Reset gameplay state
    mGamePlayState = GamePlayState::Playing;

    // Reset scene manager state
    mSpatialHashing = new SpatialHashing(TILE_SIZE * 4.0f * SCALE,
                                         LEVEL_WIDTH * TILE_SIZE * SCALE,
                                         LEVEL_HEIGHT * TILE_SIZE * SCALE);


    // Scene Manager FSM: using if/else instead of switch
    if (mNextScene == GameScene::MainMenu)
    {

        mAudio->StopAllSounds();
        mMusicHandle = mAudio->PlaySound("MainMenu.mp3", true);

        LoadMainMenu();
    }
    else if (mNextScene == GameScene::Level1)
    {
        mAudio->StopAllSounds();
        mMusicHandle = mAudio->PlaySound("Level1.wav", true);

        // Initialize level and actors
        LoadLevel("../Assets/Images/mapDrafts/maps/e01m05.tmj", LEVEL_WIDTH, LEVEL_HEIGHT);
        BuildActorsFromMap();
        mTopInvisibleWall->GetComponent<AABBColliderComponent>()->SetEnabled(true);
        mIsSpikeGateLowered = true;
        mPlayer->LockActor();
        mDog->SetState(Dog::State::Wander);
    }
    else if (mNextScene == GameScene::Level2)
    {
        //mShowWinScreen = false;
        float hudScale = 2.0f;
        mHUD = new HUD(this, "../Assets/Fonts/PeaberryBase.ttf");

        mAudio->StopAllSounds();
        mMusicHandle = mAudio->PlaySound("Level2.wav", true);

        LoadLevel("../Assets/Images/mapDrafts/maps/e01m04.tmj", LEVEL_WIDTH, LEVEL_HEIGHT);
        BuildActorsFromMap();
        mTopInvisibleWall->GetComponent<AABBColliderComponent>()->SetEnabled(false);
        mIsSpikeGateLowered = false;
        mGamePlayState = GamePlayState::EnteringMap;

        if (mTileMap) {
            int W = mTileMap->mapWidth;
            int H = mTileMap->mapHeight;

            mPassable.assign(H, std::vector<bool>(W, true));

            mStaticBlocksLayerIdx = GetStaticObjectsLayer();

            if (mStaticBlocksLayerIdx >= 0) {
                for (int r = 0; r < H; ++r) {
                    for (int c = 0; c < W; ++c) {
                        int gid = mTileMap->layers[mStaticBlocksLayerIdx].data[r*W + c];
                        if (gid != 0) mPassable[r][c] = false;
                    }
                }
            }
            SetPassable2x2Vector();
        }
    }
    else if (mNextScene == GameScene::Level3)
    {
        mShowWinScreen = true;
        float hudScale = 2.0f;
        mHUD = new HUD(this, "../Assets/Fonts/PeaberryBase.ttf");

        mAudio->StopAllSounds();
        mMusicHandle = mAudio->PlaySound("Level3.wav", true);

        LoadLevel("../Assets/Images/mapDrafts/maps/e01m01.tmj", LEVEL_WIDTH, LEVEL_HEIGHT);
        mSkeletonNum = 0;
        BuildActorsFromMap();
        mIsSpikeGateLowered = false;
        mDog->SetState(Dog::State::Idle);
        mGamePlayState = GamePlayState::EnteringMap;
    }

    // Set new scene
    mGameScene = mNextScene;
}


void Game::LoadMainMenu()
{

    auto mainMenu = new UIScreen(this, "../Assets/Fonts/PeaberryBase.ttf");
    const Vector2 backgroundSize = Vector2(1600.0f, 900.0f);
    const Vector2 backgroundPos = Vector2(mWindowWidth/2.0f - backgroundSize.x/2.0f, 0.0f);
    const Vector2 titleSize = Vector2(250.0f, 270.0f) * 2.0f;
    const Vector2 titlePos = Vector2(mWindowWidth/2.0f - titleSize.x/2.0f, 50.0f);
    mainMenu->AddImage(mRenderer, "../Assets/Images/Background.png", backgroundPos, backgroundSize);
    mainMenu->AddImage(mRenderer, "../Assets/Images/Logo.png", titlePos, titleSize);
    mainMenu->AddImage(mRenderer, "../Assets/Images/howToPlay.png",
        Vector2{mWindowWidth * 0.70f, mWindowHeight * 0.02f}, Vector2{377.f, 463.f});

    mainMenu->AddButton("Press ENTER to begin your quest!", Vector2(mWindowWidth/2.0f - 200.0f, 600.0f),
        Vector2(400.0f, 80.0f),
        [this]() {
            mFadeState = FadeState::FadeOut;
            mFadeTime  = 0.f;
            SetGameScene(GameScene::Level1);
            mAudio->PlaySound("dogBark.wav");
        },
        Vector2{350.f, 20.f}, 20);

    mainMenu->AddButton("Quit", Vector2(mWindowWidth/2.0f - 200.0f, 700.0f),
        Vector2(400.0f, 80.0f),[this]() {Quit();},Vector2{50.f, 20.f}, 20);

}

void Game::LoadLevel(const std::string& mapPath, const int levelWidth, const int levelHeight)
{
    // Load level data
    mTileMap = LoadTileMap(mapPath, mRenderer);

    if (!mTileMap) {
        SDL_Log("Failed to load map data");
        return;
    }

    // Instantiate level actors
    //BuildLevel(mLevelData, levelWidth, levelHeight);
    //BuildActorsFromMap();
}

void Game::RunLoop()
{
    while (mIsRunning)
    {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}

void Game::ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                Quit();
                break;
            case SDL_KEYDOWN:
                // Handle key press for UI screens
                if (!mUIStack.empty()) {
                    mUIStack.back()->HandleKeyPress(event.key.keysym.sym);
                }

                HandleKeyPressActors(event.key.keysym.sym, event.key.repeat == 0);

                // Check if the Return key has been pressed to pause/unpause the game
                // if (event.key.keysym.sym == SDLK_RETURN)
                if (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_p) // troquei para não termos conflito com o input especifico da tela (ver 'if' acima)
                {
                    mMusicHandle = mAudio->PlaySound("dogBark.wav", false);
                    TogglePause();
                    if (mGamePlayState == GamePlayState::Paused) {
                        HandleVolumeLevelDuringPause(false);
                    }
                    else {
                        HandleVolumeLevelDuringPause(true);
                    }
                }
                break;
        }
    }

    ProcessInputActors();
}

void Game::ProcessInputActors()
{
    if(mGamePlayState == GamePlayState::Playing ||
        mGamePlayState == GamePlayState::Leaving ||
        mGamePlayState == GamePlayState::EnteringMap)
    {
        // Get actors on camera
        std::vector<Actor*> actorsOnCamera =
                mSpatialHashing->QueryOnCamera(mCameraPos,mWindowWidth,mWindowHeight);

        const Uint8* state = SDL_GetKeyboardState(nullptr);

        bool isPlayerOnCamera = false;
        for (auto actor: actorsOnCamera)
        {
            if (mGamePlayState == GamePlayState::Playing && actor != mPlayer) {
                actor->ProcessInput(state);
            }

            if (actor == mPlayer) {
                actor->ProcessInput(state);
                isPlayerOnCamera = true;
            }
        }

        // If Player is not on camera, process input for him
        if (!isPlayerOnCamera && mPlayer) {
            mPlayer->ProcessInput(state);
        }
    }
}

void Game::HandleKeyPressActors(const int key, const bool isPressed)
{
    std::vector<Actor*> actorsOnCamera = mSpatialHashing->QueryOnCamera(mCameraPos, mWindowWidth, mWindowHeight);
    bool isPlayerOnCamera = false;

    for (auto actor : actorsOnCamera)
    {
        actor->HandleKeyPress(key, isPressed);
        if (actor == mPlayer)
            isPlayerOnCamera = true;
    }

    if (!isPlayerOnCamera && mPlayer)
    {
        mPlayer->HandleKeyPress(key, isPressed);
    }

}

void Game::TogglePause()
{

    if (mGameScene != GameScene::MainMenu)
    {
        if (mGamePlayState == GamePlayState::Playing)
        {
            mGamePlayState = GamePlayState::Paused;
            CreatePauseMenu();
        }
        else if (mGamePlayState == GamePlayState::Paused)
        {
            mGamePlayState = GamePlayState::Playing;
            if (!mUIStack.empty())
            {
                mUIStack.back()->Close();
            }
        }
    }
}

void Game::UpdateGame()
{
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));

    float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
    if (deltaTime > 0.05f)
    {
        deltaTime = 0.05f;
    }
    mTicksCount = SDL_GetTicks();
    if(mGamePlayState != GamePlayState::Paused && mGamePlayState != GamePlayState::GameOver)
    {
        // Reinsert all actors and pending actors
        UpdateActors(deltaTime);
    }
    // Reinsert audio system
    mAudio->Update(deltaTime);

    if (mShowWinScreen && mPlayer && mDog && !mBoss && mGameScene == GameScene::Level3
        && mPlayer->GetScore() == mSkeletonNum
        && mDog->GetDistanceWithOwner() <= TILE_SIZE * SCALE * 3
        && mDog->GetState() == Dog::State::Follow) {
            mShowWinScreen = false;
            new UIWinScreen(this, "../Assets/Fonts/PeaberryBase.ttf");
            mAudio->StopAllSounds();
            mMusicHandle = mAudio->PlaySound("dogBark.wav", false);
            mMusicHandle = mAudio->PlaySound("win.wav", false);
    }

    // Reinsert UI screens
    for (auto ui : mUIStack) {
        if (ui->GetState() == UIScreen::UIState::Active) {
            ui->Update(deltaTime);
        }
    }

    // Delete any UIElements that are closed
    auto iter = mUIStack.begin();
    while (iter != mUIStack.end()) {
        if ((*iter)->GetState() == UIScreen::UIState::Closing) {
            delete *iter;
            iter = mUIStack.erase(iter);
        } else {
            ++iter;
        }
    }

    UpdateCamera();
    UpdateSceneManager(deltaTime);
}

void Game::UpdateSceneManager(float deltaTime)
{
    if (mSceneManagerState == SceneManagerState::Entering){
        mSceneManagerTimer -= deltaTime;

        if (mSceneManagerTimer <= 0.0f){
            mSceneManagerTimer = TRANSITION_TIME;
            mSceneManagerState = SceneManagerState::Active;

            if (mFadeState == FadeState::None || mGamePlayState == GamePlayState::GameOver) {
                mFadeState = FadeState::FadeOut;
                }
        }
    }

    if (mSceneManagerState == SceneManagerState::Active){
        mSceneManagerTimer -= deltaTime;
        if (mSceneManagerTimer <= 0.0f){
            ChangeScene();
            mSceneManagerState = SceneManagerState::None;
        }
    }

    if (mFadeState == FadeState::FadeOut) {
        mFadeTime += deltaTime;
        if (mFadeTime >= TRANSITION_TIME) {
            mFadeTime = 0.f;
            mFadeState = FadeState::FadeIn;
        }
    }
    else if (mFadeState == FadeState::FadeIn) {
        mFadeTime += deltaTime;
        if (mFadeTime >= TRANSITION_TIME) {
            mFadeTime = 0.f;
            mFadeState = FadeState::None;
        }
    }
}


void Game::UpdateCamera()
{
    if (!mPlayer) return;

    SetCameraPos(Vector2{mPlayer->GetPosition().x - (mWindowWidth / 2.0f),
        mPlayer->GetPosition().y - (mWindowHeight / 2.0f)});
}

void Game::UpdateActors(float deltaTime)
{
    std::vector<Actor*> actorsOnCamera =
            mSpatialHashing->QueryOnCamera(mCameraPos, mWindowWidth, mWindowHeight);

    std::vector<Actor*> toDelete;
    bool isPlayerOnCamera = false;

    for (auto actor : actorsOnCamera)
    {
        actor->Update(deltaTime);
        if (actor == mPlayer)
            isPlayerOnCamera = true;

        if (actor->GetState() == ActorState::Destroy)
            toDelete.emplace_back(actor);
    }
    if (!isPlayerOnCamera && mPlayer)
    {
        mPlayer->Update(deltaTime);
    }

    // Remoção separada para evitar usar ponteiros já deletados
    for (auto actor : toDelete)
    {
        if (actor == mPlayer)
        {
            mPlayer = nullptr;
        }
        else if (actor == mBoss) {
            mBoss = nullptr;
        }
        else if (actor == mDog) {
            mDog == nullptr;
        }
        delete actor;
    }
}

void Game::AddActor(Actor* actor)
{
    mSpatialHashing->Insert(actor);
}

void Game::RemoveActor(Actor* actor)
{
    mSpatialHashing->Remove(actor);
}
void Game::Reinsert(Actor* actor)
{
    mSpatialHashing->Reinsert(actor);
}

std::vector<Actor *> Game::GetNearbyActors(const Vector2& position, const int range)
{
    return mSpatialHashing->Query(position, range);
}

std::vector<AABBColliderComponent *> Game::GetNearbyColliders(const Vector2& position, const int range)
{
    return mSpatialHashing->QueryColliders(position, range);
}

void Game::GenerateOutput()
{
    // Clear frame with background color
    SDL_SetRenderDrawColor(mRenderer, mBackgroundColor.x, mBackgroundColor.y, mBackgroundColor.z, 255);

    // Clear back buffer
    SDL_RenderClear(mRenderer);

    int roofIdx = -1;
    int wallDetailsIdx = -1;

    if (mTileMap && !mTileMap->layers.empty()) {
        for (int i=0; i<mTileMap->layers.size(); i++) {
            if (mTileMap->layers[i].name == "roof") {
                roofIdx = i;
            }
            else if (mTileMap->layers[i].name == "wallDetails") {
                wallDetailsIdx = i;
            }
            else if (mTileMap->layers[i].type == LayerType::Block && mTileMap->layers[i].name != "staticObjects") {
                RenderLayer(mRenderer, mTileMap, i, mCameraPos, mWindowWidth, mWindowHeight,  SCALE);
            }
        }
    }

    // Get actors on camera
    std::vector<Actor*> actorsOnCamera =
            mSpatialHashing->QueryOnCamera(mCameraPos,mWindowWidth,mWindowHeight);

    // Get list of drawables in draw order
    std::vector<DrawComponent*> drawables;

    for (auto actor : actorsOnCamera)
    {
        if (actor == mBoss) continue;

        auto drawable = actor->GetComponent<DrawComponent>();
        if (drawable && drawable->IsVisible())
        {
            drawables.emplace_back(drawable);
        }
    }

    std::sort(drawables.begin(), drawables.end(),
              [](const DrawComponent* a, const DrawComponent* b) {
                  return a->GetDrawOrder() < b->GetDrawOrder();
              });

    for (auto drawable : drawables)
    {
        drawable->Draw(mRenderer, mModColor);
    }

    if (roofIdx > -1) {
        RenderLayer(mRenderer, mTileMap, roofIdx, mCameraPos, mWindowWidth, mWindowHeight,  SCALE);
    }
    if (wallDetailsIdx > -1) {
        RenderLayer(mRenderer, mTileMap, wallDetailsIdx, mCameraPos, mWindowWidth, mWindowHeight,  SCALE);
    }

    if (mBoss) {
        auto drawable = mBoss->GetComponent<DrawComponent>();
        if (drawable && drawable->IsVisible()) {
            drawable->Draw(mRenderer, mModColor);
        }

    }

    for (auto ui :mUIStack)
    {
        ui->Draw(mRenderer);
    }

    // if (mSceneManagerState == SceneManagerState::Active) {
    //     SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);
    //     SDL_Rect fullScreenRect = {0, 0, mWindowWidth, mWindowHeight};
    //     SDL_RenderFillRect(mRenderer, &fullScreenRect);
    // }

    if (mFadeState == FadeState::FadeOut) {
        float alphaOut = mFadeTime/TRANSITION_TIME;
        SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255 * alphaOut);
        SDL_RenderFillRect(mRenderer, nullptr);
    }

    if (mFadeState == FadeState::FadeIn) {
        float alphaIn = mFadeTime/TRANSITION_TIME;
        SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255 * (1 - alphaIn));
        SDL_RenderFillRect(mRenderer, nullptr);
    }

    // Swap front buffer and back buffer
    SDL_RenderPresent(mRenderer);
}

void Game::SetBackgroundImage(const std::string& texturePath, const Vector2 &position, const Vector2 &size)
{
    if (mBackgroundTexture) {
        SDL_DestroyTexture(mBackgroundTexture);
        mBackgroundTexture = nullptr;
    }

    // Load background texture
    mBackgroundTexture = LoadTexture(texturePath);
    if (!mBackgroundTexture) {
        SDL_Log("Failed to load background texture: %s", texturePath.c_str());
    }

    // Set background position
    mBackgroundPosition.Set(position.x, position.y);

    // Set background size
    mBackgroundSize.Set(size.x, size.y);
}

SDL_Texture* Game::LoadTexture(const std::string& texturePath)
{
    SDL_Surface* surface = IMG_Load(texturePath.c_str());

    if (!surface) {
        SDL_Log("Failed to load image: %s", IMG_GetError());
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(mRenderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        SDL_Log("Failed to create texture: %s", SDL_GetError());
        return nullptr;
    }

    return texture;
}

UIFont* Game::LoadFont(const std::string& fileName)
{
    auto iter = mFonts.find(fileName);
    if (iter != mFonts.end()){
        return iter->second;
    }
    UIFont* newFont = new UIFont(mRenderer);
    if (newFont->Load(fileName)){
        mFonts.emplace(fileName, newFont);
        return newFont;
    }
    else{
        newFont->Unload();
        delete newFont;
        SDL_Log("Failed to load font: %s", fileName.c_str());
        return nullptr;
    }
}

void Game::UnloadScene()
{
    // Delete actors
    delete mSpatialHashing;
    mSpatialHashing = nullptr;

    mHUD = nullptr;
    mPlayer = nullptr;
    mDog = nullptr;
    mBoss = nullptr;
    mTopInvisibleWall = nullptr;
    mBottomInvisibleWall = nullptr;

    // Delete UI screens
    for (auto ui : mUIStack) {
        delete ui;
        mUIStack.pop_back();
    }
    mUIStack.clear();

    // Delete background texture
    if (mBackgroundTexture) {
        SDL_DestroyTexture(mBackgroundTexture);
        mBackgroundTexture = nullptr;
    }
}

void Game::Shutdown()
{
    UnloadScene();
    for (auto font : mFonts) {
         font.second->Unload();
         delete font.second;
    }
     mFonts.clear();

     delete mAudio;
     mAudio = nullptr;

     Mix_CloseAudio();

     Mix_Quit();
     TTF_Quit();
     IMG_Quit();

    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

UIScreen* Game::CreatePauseMenu()
{
    UIScreen* pauseMenu = new UIScreen(this, "../Assets/Fonts/PeaberryBase.ttf");

    const Vector2 backgroundOriginalSize(1024.0f, 1049.0f);
    const float backgroundScale = 0.6f;
    const Vector2 backgroundSize = backgroundOriginalSize * backgroundScale;

    const Vector2 backgroundPos(
        mWindowWidth / 2.0f - backgroundSize.x / 2.0f,
        mWindowHeight / 2.0f - backgroundSize.y / 2.0f
    );

    const Vector2 titleSize(320.0f, 60.0f);
    const int titleFontSize = 52;
    const float titleYOffset = 0.16f;

    const Vector2 titlePos = backgroundPos + Vector2(
        (backgroundSize.x - titleSize.x) / 2.0f,
        backgroundSize.y * titleYOffset
    );

    const Vector2 buttonSize(400.0f, 80.0f);
    const float buttonSpacing = 20.0f;
    const float firstButtonYOffset = 0.4f;

    const Vector2 firstButtonPos = backgroundPos + Vector2(
        (backgroundSize.x - buttonSize.x) / 2.0f,
        backgroundSize.y * firstButtonYOffset
    );

    const Vector2 secondButtonPos = firstButtonPos + Vector2(0.0f, buttonSize.y + buttonSpacing);

    pauseMenu->AddImage(
        mRenderer,
        "../Assets/Images/pauseMenu.png",
        backgroundPos,
        backgroundSize
    );

    pauseMenu->AddText(
        "Game Paused",
        titlePos,
        titleSize,
        titleFontSize,
        0);

    pauseMenu->AddButton(
        "Main Menu",
        firstButtonPos,
        buttonSize,
        [this, pauseMenu]() {
            if (pauseMenu->GetState() != UIScreen::UIState::Active) return;
            pauseMenu->Close();
            mFadeState = FadeState::FadeOut;
            mFadeTime  = 0.f;
            mIsSpikeGateLowered = true;
            mSkeletonNum = 0;
            SetGameScene(GameScene::MainMenu);
            HandleVolumeLevelDuringPause(true);
        }
    );

    pauseMenu->AddButton(
        "Continue",
        secondButtonPos,
        buttonSize,
        [this]() {
            TogglePause();
            HandleVolumeLevelDuringPause(true);
        }
    );

    return pauseMenu;
}


void Game::BuildActorsFromMap() {
    if (!mTileMap) {
        return;
    }

    int dynamicObjectsLayerIdx = GetLayerIdx(*mTileMap, "dynamicObjects");
    int staticObjectsLayerIdx = GetLayerIdx(*mTileMap, "staticObjects");

    for (auto obj : mTileMap->layers[dynamicObjectsLayerIdx].objects) {
        if (obj.name == "player") {
            mPlayer = new Player(this, Vector2(obj.pos.x * SCALE, obj.pos.y * SCALE));
        }
        else if (obj.name == "dog")
        {
            mDog = new Dog(this, Vector2(obj.pos.x * SCALE - 48.0f, obj.pos.y * SCALE - 48.0f));
            if (mPlayer) mDog->SetOwner(mPlayer);
        }
        else if (obj.name.find("invisibleWall") != std::string::npos) {
            size_t pos = obj.name.find('-');

            if (pos != std::string::npos) {
                std::string topOrBottom = obj.name.substr(pos + 1);

                if (topOrBottom == "bottom") {
                    mBottomInvisibleWall = new InvisibleWall(this, Vector2(obj.pos.x * SCALE, obj.pos.y * SCALE),
                    obj.width * SCALE, obj.height * SCALE, obj.scene, topOrBottom);
                }
                else {
                    mTopInvisibleWall = new InvisibleWall(this, Vector2(obj.pos.x * SCALE, obj.pos.y * SCALE),
                    obj.width * SCALE, obj.height * SCALE, obj.scene, topOrBottom);
                }

            }
        }
        else if (obj.name == "boss") {
            mBoss = new Boss(this, mPlayer, Vector2(obj.pos.x * SCALE, obj.pos.y * SCALE));
        }
        else if (obj.name.find("spike-gate") != std::string::npos)
        {
            int drawOrder = obj.name.back() - '0';
            new SpikeGate(this, Vector2(obj.pos.x * SCALE, obj.pos.y * SCALE),
                obj.width * SCALE, obj.height * SCALE, drawOrder);
        }
        else if (obj.name == "skeleton") {
            //if (auto i = Random::GetIntRange(0, 1); i == 0) continue;
            new Skeleton(this, mPlayer, Vector2(obj.pos.x * SCALE, obj.pos.y * SCALE));
            mSkeletonNum++;
        }
        else if (obj.name == "spAttackPlaceholder" && mBoss) {
            mBoss->SetSpAttackPos(Vector2{obj.pos.x * SCALE, obj.pos.y * SCALE});
        }
    }

    Layer staticObjectLayer = mTileMap->layers[staticObjectsLayerIdx];
    bool rotate = false;

    for (int y=0; y<staticObjectLayer.height; y++) {
        for (int x=0; x<staticObjectLayer.width; x++) {

            int gid = staticObjectLayer.data[y*staticObjectLayer.width + x];

            if (gid == 0) continue;

            TileRenderInfo tsInfo = GetTileFlipInfoFromGID(gid);
            uint32_t clean_gid = tsInfo.clean_gid;

            int tsxIdx = FindTilesetIndex(mTileMap, clean_gid);
            const auto& ts = mTileMap->tilesets[tsxIdx];
            int localId = clean_gid - ts.firstGid;

            if (clean_gid != gid) {
                rotate = true;
            }

            if (!ts.isCollection) {
                int row = localId / ts.columns;
                int col = localId % ts.columns;

                Vector2 pos{static_cast<float>(x*TILE_SIZE*SCALE), static_cast<float>(y*TILE_SIZE*SCALE)};
                Vector2 srcPos{static_cast<float>(col * TILE_SIZE), static_cast<float>(row * TILE_SIZE)};

                new ColliderBlock(this, pos, srcPos, TILE_SIZE * SCALE, TILE_SIZE * SCALE, ts.imageTexture, rotate);
            }
            else {
                const SDL_Rect& size = ts.sizes[localId];
                Vector2 pos{static_cast<float>(x * TILE_SIZE * SCALE),
                    static_cast<float>(((y*TILE_SIZE) - size.h + TILE_SIZE) * SCALE)};

                new ColliderBlock(this, pos, Vector2::Zero, size.w * SCALE, size.h * SCALE, ts.textures[localId]);
            }
        }
    }
}

void Game::DecreaseSkeletonNum() {
    mSkeletonNum--;
    if (mSkeletonNum <= 0) {
        mSkeletonNum = 0;
    }
};

void Game::HandleVolumeLevelDuringPause(bool resumingGame) {
    if (mGameScene == GameScene::Level1 || mGameScene == GameScene::Level2 || mGameScene == GameScene::Level3) {
        std::string soundName = "";
        if (mGameScene == GameScene::Level1) {
            soundName = "Level1.wav";
        }
        else if (mGameScene == GameScene::Level2) {
            soundName = "Level2.wav";
        }
        else if (mGameScene == GameScene::Level3) {
            soundName = "Level3.wav";
        }
        Mix_Chunk* chunk = mAudio->GetSound(soundName);

        if (resumingGame) {
            Mix_VolumeChunk(chunk, MIX_MAX_VOLUME);
            return;
        }
        Mix_VolumeChunk(chunk, MIX_MAX_VOLUME/4);
    }
}

int Game::GetStaticObjectsLayer() {
    if (!mTileMap) return -1;

    for (int i=0; i<mTileMap->layers.size(); i++) {
        if (mTileMap->layers[i].name == "staticObjects") return i;
    }
    return -1;
}

void Game::SetPassable2x2Vector() {
    int h = mPassable.size(), w = mPassable[0].size();
    mPassable2x2 = mPassable;
    for (int r = 0; r < h-1; ++r) {
        for (int c = 0; c < w-1; ++c) {
            if (mPassable[r][c] && mPassable[r+1][c] &&
                mPassable[r][c+1] && mPassable[r+1][c+1]) {
                mPassable2x2[r][c] = true;
                } else {
                    mPassable2x2[r][c] = false;
                }
        }
    }
}