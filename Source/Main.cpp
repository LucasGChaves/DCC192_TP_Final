//
// Created by Lucas on 06/05/2025.
//

// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
//
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#define SDL_MAIN_HANDLED
#include "Game.h"

int main(int argc, char** argv) {
    Game game = Game(Game::SCREEN_WIDTH, Game::SCREEN_HEIGHT);
    bool success = game.Initialize();
    if (success) {
        game.RunLoop();
    }
    game.Shutdown();
    return 0;
}