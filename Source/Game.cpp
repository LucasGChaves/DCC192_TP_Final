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
#include "Actors/Spawner.h"
#include "UIElements/UIScreen.h"
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
        ,mTileMap(nullptr)
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


    mSpatialHashing = new SpatialHashing(TILE_SIZE * 4.0f,
                                         LEVEL_WIDTH * TILE_SIZE,
                                         LEVEL_HEIGHT * TILE_SIZE);
    mTicksCount = SDL_GetTicks();

    mSpatialHashing = new SpatialHashing(TILE_SIZE * 4.0f, LEVEL_WIDTH * TILE_SIZE, LEVEL_HEIGHT * TILE_SIZE);
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
    if (scene == GameScene::MainMenu || scene == GameScene::Level1 || scene == GameScene::Level2){
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
    // TODO
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
    mSpatialHashing = new SpatialHashing(TILE_SIZE * 4.0f, LEVEL_WIDTH * TILE_SIZE, LEVEL_HEIGHT * TILE_SIZE);

    // Scene Manager FSM: using if/else instead of switch
    if (mNextScene == GameScene::MainMenu)
    {
        // Initialize main menu actors
        mBackgroundColor.Set(107.0f, 140.0f, 255.0f);

        mAudio->StopAllSounds();
        mMusicHandle = mAudio->PlaySound("MainMenu.wav", true);

        LoadMainMenu();
    }
    else if (mNextScene == GameScene::Level1)
    {
        // TODO
        float hudScale = 2.0f;
        mHUD = new HUD(this, "../Assets/Fonts/PeaberryBase.ttf");

        mAudio->StopSound(mMusicHandle);
        mMusicHandle = mAudio->PlaySound("Level1.wav", true);

        // Initialize actors
        LoadLevel("../Assets/Images/mapDrafts/maps/e01m05.tmj", LEVEL_WIDTH, LEVEL_HEIGHT);
        BuildActorsFromMap();
        //LoadLevel("../Assets/Levels/level1-1.csv", LEVEL_WIDTH, LEVEL_HEIGHT);
        //mPlayer = new Player(this);
        //mPlayer->SetPosition(Vector2(mWindowWidth / 2.0f, mWindowHeight / 2.0f));


        const float minDistance = 200.0f;
        Vector2 minBounds(0.0f, 0.0f);
        Vector2 maxBounds(static_cast<float>(mWindowWidth), static_cast<float>(mWindowHeight));

        for (int i = 0; i < 5; ++i) {
            Vector2 spawnPos;
            bool validPos = false;

            while (!validPos) {
                spawnPos = Random::GetVector(minBounds, maxBounds);

                Vector2 toPlayer = spawnPos - mPlayer->GetPosition();
                float distance = toPlayer.Length();

                if (distance >= minDistance) {
                    validPos = true;
                }
            }

            auto* newSkeleton = new Skeleton(this, mPlayer);
            newSkeleton->SetPosition(spawnPos);
        }
    }
    else if (mNextScene == GameScene::Level2)
    {
        // TODO

        // Initialize actors
        //LoadLevel("../Assets/Levels/level1-2.csv", LEVEL_WIDTH, LEVEL_HEIGHT);
    }

    // Set new scene
    mGameScene = mNextScene;
}


void Game::LoadMainMenu()
{
    // TODO

    auto mainMenu = new UIScreen(this, "../Assets/Fonts/PeaberryBase.ttf");
    const Vector2 backgroundSize = Vector2(1600.0f, 900.0f);
    const Vector2 backgroundPos = Vector2(mWindowWidth/2.0f - backgroundSize.x/2.0f, 0.0f);
    const Vector2 titleSize = Vector2(250.0f, 270.0f) * 2.0f;
    const Vector2 titlePos = Vector2(mWindowWidth/2.0f - titleSize.x/2.0f, 50.0f);
    mainMenu->AddImage(mRenderer, "../Assets/Images/Background.png", backgroundPos, backgroundSize);
    mainMenu->AddImage(mRenderer, "../Assets/Images/Logo.png", titlePos, titleSize);


    auto button1 = mainMenu->AddButton("Begin Quest!", Vector2(mWindowWidth/2.0f - 200.0f, 600.0f), Vector2(400.0f, 80.0f),
                                       [this]() {SetGameScene(GameScene::Level1);});

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

void Game::BuildLevel(int** levelData, int width, int height)
{

    // Const map to convert tile ID to block type
    const std::map<int, const std::string> tileMap = {
            //{0, "../Assets/Sprites/Blocks/BlockA.png"},
    };

    for (int y = 0; y < LEVEL_HEIGHT; ++y)
    {
        for (int x = 0; x < LEVEL_WIDTH; ++x)
        {
            // int tile = levelData[y][x];

            // TODO - Mudar posição inicial do player e verificar IDs dos tiles nos CSVs
            // if(tile == 16) // Player
            // {
            //     mPlayer = new Player(this);
            //     mPlayer->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
            // }
            // else if(tile == 10) // Spawner
            // {
            //     Spawner* spawner = new Spawner(this, SPAWN_DISTANCE);
            //     spawner->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
            // }
            // else // Blocks
            // {
            //     auto it = tileMap.find(tile);
            //     if (it != tileMap.end())
            //     {
            //         // Create a block actor
            //         //Block* block = new Block(this, it->second);
            //         //block->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
            //     }
            // }
        }
    }
}

int **Game::ReadLevelData(const std::string& fileName, int width, int height)
{
    std::ifstream file(fileName);
    if (!file.is_open())
    {
        SDL_Log("Failed to load paths: %s", fileName.c_str());
        return nullptr;
    }

    // Create a 2D array of size width and height to store the level data
    int** levelData = new int*[height];
    for (int i = 0; i < height; ++i)
    {
        levelData[i] = new int[width];
    }

    // Read the file line by line
    int row = 0;

    std::string line;
    while (!file.eof())
    {
        std::getline(file, line);
        if(!line.empty())
        {
            auto tiles = CSVHelper::Split(line);

            if (tiles.size() != width) {
                SDL_Log("Invalid level data");
                return nullptr;
            }

            for (int i = 0; i < width; ++i) {
                levelData[row][i] = tiles[i];
            }
        }

        ++row;
    }

    // Close the file
    file.close();

    return levelData;
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
                if (event.key.keysym.sym == SDLK_ESCAPE) // troquei para não termos conflito com o input especifico da tela (ver 'if' acima)
                {
                    TogglePause();
                }
                break;
        }
    }

    ProcessInputActors();
}

void Game::ProcessInputActors()
{
    if(mGamePlayState == GamePlayState::Playing)
    {
        // Get actors on camera
        std::vector<Actor*> actorsOnCamera =
                mSpatialHashing->QueryOnCamera(mCameraPos,mWindowWidth,mWindowHeight);

        const Uint8* state = SDL_GetKeyboardState(nullptr);

        bool isPlayerOnCamera = false;
        for (auto actor: actorsOnCamera)
        {
            actor->ProcessInput(state);

            if (actor == mPlayer) {
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

    if (mGameScene != GameScene::MainMenu && mGamePlayState == GamePlayState::Playing){
        UpdateLevelTime(deltaTime);
    }
}

void Game::UpdateSceneManager(float deltaTime)
{
    if (mSceneManagerState == SceneManagerState::Entering){
        mSceneManagerTimer -= deltaTime;

        if (mSceneManagerTimer <= 0.0f){
            mSceneManagerTimer = TRANSITION_TIME;
            mSceneManagerState = SceneManagerState::Active;
        }
    }

    if (mSceneManagerState == SceneManagerState::Active){
        mSceneManagerTimer -= deltaTime;
        if (mSceneManagerTimer <= 0.0f){
            ChangeScene();
            mSceneManagerState = SceneManagerState::None;
        }
    }
}

void Game::UpdateLevelTime(float deltaTime)
{
    // TODO
}

void Game::UpdateCamera()
{
    if (!mPlayer) return;

    mCameraPos.x = mPlayer->GetPosition().x - (mWindowWidth / 2.0f);
    mCameraPos.y = mPlayer->GetPosition().y - (mWindowHeight / 2.0f);
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
    for (int i=0; i<mTileMap->layers.size(); i++) {
        if (mTileMap->layers[i].name == "roof") {
            roofIdx = i;
        }
        else if (mTileMap->layers[i].type == LayerType::Block) {
            RenderLayer(mRenderer, mTileMap, i, mCameraPos, mWindowWidth, mWindowHeight,  SCALE);
        }
    }

    // Draw background texture considering camera position
    if (mBackgroundTexture)
    {
        SDL_Rect dstRect = { static_cast<int>(mBackgroundPosition.x - mCameraPos.x),
                             static_cast<int>(mBackgroundPosition.y - mCameraPos.y),
                             static_cast<int>(mBackgroundSize.x),
                             static_cast<int>(mBackgroundSize.y) };

        SDL_RenderCopy(mRenderer, mBackgroundTexture, nullptr, &dstRect);
    }

    // Get actors on camera
    std::vector<Actor*> actorsOnCamera =
            mSpatialHashing->QueryOnCamera(mCameraPos,mWindowWidth,mWindowHeight);

    // Get list of drawables in draw order
    std::vector<DrawComponent*> drawables;

    for (auto actor : actorsOnCamera)
    {
        auto drawable = actor->GetComponent<DrawComponent>();
        if (drawable && drawable->IsVisible())
        {
            drawables.emplace_back(drawable);
        }
    }

    // Sort drawables by draw order
    std::sort(drawables.begin(), drawables.end(),
              [](const DrawComponent* a, const DrawComponent* b) {
                  return a->GetDrawOrder() < b->GetDrawOrder();
              });

    // Draw all drawables
    for (auto drawable : drawables)
    {
        drawable->Draw(mRenderer, mModColor);
    }

    if (roofIdx > -1) {
        RenderLayer(mRenderer, mTileMap, roofIdx, mCameraPos, mWindowWidth, mWindowHeight,  SCALE);
    }

    // Draw all UI screens
    for (auto ui :mUIStack)
    {
        ui->Draw(mRenderer);
    }

    if (mSceneManagerState == SceneManagerState::Active) {
        SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);
        SDL_Rect fullScreenRect = {0, 0, mWindowWidth, mWindowHeight};
        SDL_RenderFillRect(mRenderer, &fullScreenRect);
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

    // Delete UI screens
    for (auto ui : mUIStack) {
        delete ui;
        mUIStack.pop_back();
    }

    // Delete background texture
    if (mBackgroundTexture) {
        SDL_DestroyTexture(mBackgroundTexture);
        mBackgroundTexture = nullptr;
    }
}

void Game::Shutdown()
{
    // TODO - descomentar e alterar depois
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
            SetGameScene(GameScene::MainMenu);
        }
    );

    pauseMenu->AddButton(
        "Continue",
        secondButtonPos,
        buttonSize,
        [this]() {
            TogglePause();
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
            SDL_Log("Loading player at pos... %f, %f", obj.pos.x, obj.pos.y);
            // TODO - completar ao linkar com código do Adalberto

            mPlayer = new Player(this);
            mPlayer->SetPosition(Vector2(obj.pos.x * TILE_SIZE, obj.pos.y * TILE_SIZE));
        }
    }

    Layer staticObjectLayer = mTileMap->layers[staticObjectsLayerIdx];

    for (int y=0; y<staticObjectLayer.height; y++) {
        for (int x=0; x<staticObjectLayer.width; x++) {

            int gid = staticObjectLayer.data[y*staticObjectLayer.width + x];

            if (gid == 0) continue;

            int tsxIdx = FindTilesetIndex(mTileMap, gid);
            const auto& ts = mTileMap->tilesets[tsxIdx];
            int localId = gid - ts.firstGid;

            if (!ts.isCollection) {
                int row = localId / ts.columns;
                int col = localId % ts.columns;

                Vector2 pos{static_cast<float>(x*TILE_SIZE), static_cast<float>(y*TILE_SIZE)};
                Vector2 srcPos{static_cast<float>(col * TILE_SIZE), static_cast<float>(row * TILE_SIZE)};

                new ColliderBlock(this, pos, srcPos, ts.imageTexture);
            }
            else {
                const SDL_Rect& size = ts.sizes[localId];
                Vector2 pos{static_cast<float>(x * TILE_SIZE),
                    static_cast<float>((y*TILE_SIZE) - size.h + TILE_SIZE)};

                new ColliderBlock(this, pos, Vector2::Zero, ts.textures[localId]);
            }
        }
    }
}