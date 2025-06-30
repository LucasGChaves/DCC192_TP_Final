//
// Created by Lucas on 21/06/2025.
//
#include <SDL.h>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include "Json.h"
#include "tinyxml2.h"
#include "Math.h"

#pragma once

enum class LayerType
{
    Block,
    Object
};

struct TilesetInfo {
    bool isCollection;
    int firstGid;
    int tileWidth, tileHeight;
    std::string tsxSource;
    //std::string imagePath;
    std::vector<SDL_Texture*> textures; // if isCollection = true
    std::vector<SDL_Rect> sizes; // if isCollection = true
    SDL_Texture* imageTexture; // if isCollection = false
    int columns, rows; // if isCollection = false

};

struct DynamicObject {
    int id;
    std::string name;
    std::string scene;
    Vector2 pos;
    int width, height;
};

struct Layer {
    LayerType type;
    std::string name;
    int width, height;
    // Block Layer
    std::vector<int> data; // GIDs
    // Object Layer
    std::vector<DynamicObject> objects;
};

struct MapData {
    int mapWidth, mapHeight;
    int tileWidth, tileHeight;
    std::vector<TilesetInfo> tilesets;
    std::vector<Layer> layers;
    std::vector<DynamicObject> dynamicObjects;
};

struct TileRenderInfo {
    uint32_t clean_gid;
    double angle_deg;
    SDL_RendererFlip flip;
};

MapData* LoadTileMap(const std::string& jsonPath, SDL_Renderer* renderer);

int FindTilesetIndex(const MapData* map, int gid);

void RenderLayer(SDL_Renderer* renderer, const MapData* map, int layerIdx,
    Vector2& cameraPos , float windowWidth, float windowHeight, int scale=1, float extraRadius=0.0f);

std::array<int,4> GetCameraTileBounds(
    const Vector2& cameraPos,
    float screenW, float screenH,
    float extraRadius,
    int tileSize,
    int mapWidth, int mapHeight);

int GetLayerIdx(MapData map, std::string layerName);

TileRenderInfo GetTileFlipInfoFromGID(uint32_t gid_with_flags);