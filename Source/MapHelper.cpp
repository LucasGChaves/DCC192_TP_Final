//
// Created by Lucas on 21/06/2025.
//

#include "MapHelper.h"

#include <SDL_image.h>

using namespace tinyxml2;

std::string PATH_PREFIX = "../Assets/Images/mapDrafts";

MapData* LoadTileMap(const std::string& jsonPath, SDL_Renderer* renderer) {
    std::ifstream file(jsonPath);
    auto json = nlohmann::json::parse(file);

    MapData* map = new MapData();
    map->mapWidth = json["width"].get<int>();
    map->mapHeight = json["height"].get<int>();
    map->tileWidth = json["tilewidth"].get<int>();
    map->tileHeight = json["tileheight"].get<int>();

    TilesetInfo tileInfo;

    for (auto& tile : json["tilesets"]) {
      tileInfo.firstGid = tile["firstgid"].get<int>();
      tileInfo.tsxSource = PATH_PREFIX + tile["source"].get<std::string>().substr(2);
      tileInfo.isCollection = false;
      tileInfo.imageTexture = nullptr;
      map->tilesets.push_back(tileInfo);
    }

    std::sort(map->tilesets.begin(), map->tilesets.end(), [](auto a, auto b){return a.firstGid < b.firstGid;});

    for (auto& tile : map->tilesets) {
        XMLDocument doc;
        if (doc.LoadFile(tile.tsxSource.c_str()) != XML_SUCCESS) {
             SDL_Log("Failed to load tileset %s", tile.tsxSource.c_str());
             continue;
        }

        auto* root = doc.RootElement();
        int columns   = root->IntAttribute("columns", 0);
        int tilecount = root->IntAttribute("tilecount", 0);
        int tw = root->IntAttribute("tilewidth",  map->tileWidth);
        int th = root->IntAttribute("tileheight", map->tileHeight);

        if (columns > 0) {
            tile.isCollection = false;
            tile.tileWidth = tw;
            tile.tileHeight = th;
            tile.columns = columns;
            tile.rows = (tilecount + columns - 1) / columns;

            auto* imageEl = root->FirstChildElement("image");
            std::string imageSrc = imageEl->Attribute("source");
            imageSrc = PATH_PREFIX + imageSrc.substr(2);
            tile.imageTexture = IMG_LoadTexture(renderer, imageSrc.c_str());

            if (!tile.imageTexture) {
                SDL_Log("Failed to load image %s", imageSrc.c_str());
            }
        }
        else {
            tile.isCollection = true;
            tile.textures.resize(tilecount);
            tile.sizes.resize(tilecount);

            for (auto* tileEl = root->FirstChildElement("tile"); tileEl; tileEl = tileEl->NextSiblingElement("tile")) {
                int id = tileEl->IntAttribute("id", 0);
                auto* imageEl = tileEl->FirstChildElement("image");
                std::string imageSrc = imageEl->Attribute("source");
                imageSrc = PATH_PREFIX + imageSrc.substr(2);
                int w = imageEl->IntAttribute("width", 0);
                int h = imageEl->IntAttribute("height", 0);

                tile.textures[id] = IMG_LoadTexture(renderer, imageSrc.c_str());
                tile.sizes[id] = SDL_Rect{0, 0, w, h};

                if (!tile.textures[id]) {
                    SDL_Log("Failed to load image %s", imageSrc.c_str());
                }
            }
        }
    }

    for (auto& l : json["layers"]) {
        if (l["type"] == "tilelayer") {
            Layer layer;
            layer.type = LayerType::Block;
            layer.name = l["name"];
            layer.width = l["width"];
            layer.height = l["height"];
            layer.data = l["data"].get<std::vector<int>>();
            map->layers.push_back(layer);
        }
        else if (l["type"] == "objectgroup") {
            Layer layer;
            layer.name = l["name"];
            layer.type = LayerType::Object;

            for (auto& o : l["objects"]) {
                DynamicObject obj;
                obj.name = o["name"];
                obj.scene = o["type"];
                obj.pos = Vector2{o["x"], o["y"]};
                obj.width = o["width"];
                obj.height = o["height"];
                layer.objects.push_back(obj);
            }
            map->layers.push_back(layer);
        }
    }
    return map;

}

int FindTilesetIndex(const MapData* map, int gid) {
    int idx = -1;
    for (int i=0; i<map->tilesets.size(); i++) {
        if (map->tilesets[i].firstGid <= gid) {
            idx = i;
        }
        else break;
    }
    return idx;
}

void RenderLayer(SDL_Renderer* renderer, const MapData* map, int layerIdx,
    Vector2& cameraPos, float windowWidth, float windowHeight, int scale, float extraRadius) {

//     auto [sr, sc, er, ec] = GetCameraTileBounds(
//     cameraPos,
//     windowWidth,
//     windowHeight,
//     extraRadius,
//     map->tileWidth * scale,
//     map->mapWidth,
//     map->mapHeight
// );

    const auto& layer = map->layers[layerIdx];

    if (map->layers[layerIdx].name == "staticObjects" || map->layers[layerIdx].name == "dynamicObjects") {
        return;
    }

    bool flipped = false;

    for (int y=0; y<layer.height; y++) {
    //for (int y=sr; y<=er; y++) {
        //for (int x=sc; x<=ec; x++) {
        for (int x=0; x<layer.width; x++) {
            int gid = layer.data[y*layer.width + x];

            if (gid == 0) continue;

            TileRenderInfo tsInfo = GetTileFlipInfoFromGID(gid);
            uint32_t clean_gid = tsInfo.clean_gid;

            int tsxIdx = FindTilesetIndex(map, clean_gid);
            const auto& ts = map->tilesets[tsxIdx];
            int localId = clean_gid - ts.firstGid;

            if (clean_gid != gid) {
                flipped = true;
            }

            if (!ts.isCollection) {

                if (!ts.imageTexture) {
                    SDL_Log("Failed to load image %s", ts.tsxSource.c_str());
                }

                int row = localId / ts.columns;
                int col = localId % ts.columns;

                SDL_Rect src{col * ts.tileWidth, row * ts.tileHeight, ts.tileWidth, ts.tileHeight};
                SDL_Rect dst{x * map->tileWidth * scale - static_cast<int>(cameraPos.x),
                    y * map->tileHeight * scale - static_cast<int>(cameraPos.y),
                    ts.tileWidth * scale, ts.tileHeight * scale};

                if (!flipped) {
                    SDL_RenderCopy(renderer, ts.imageTexture, &src, &dst);
                }
                else {
                    SDL_RenderCopyEx(renderer, ts.imageTexture, &src, &dst, tsInfo.angle_deg, nullptr, tsInfo.flip);
                }
            }
            else {
                if (!ts.textures[localId]) {
                    SDL_Log("Failed to load image %s", ts.tsxSource.c_str());
                }

                const SDL_Rect& size = ts.sizes[localId];
                SDL_Rect dst{x * map->tileWidth * scale - static_cast<int>(cameraPos.x),
                    (y * map->tileHeight * scale) + (map->tileHeight * scale) - (size.h * scale) - static_cast<int>(cameraPos.y),
                    size.w * scale, size.h * scale};
                SDL_RenderCopy(renderer, ts.textures[localId], nullptr, &dst);
            }
        }
    }
}



std::array<int,4> GetCameraTileBounds(const Vector2& cameraPos, float screenW, float screenH, float extraRadius,
    int tileSize, int mapWidth, int mapHeight)
{

    float left   = cameraPos.x - extraRadius;
    float top    = cameraPos.y - extraRadius;
    float right  = cameraPos.x + screenW + extraRadius;
    float bottom = cameraPos.y + screenH + extraRadius;


    int startCol = static_cast<int>(left  / tileSize);
    int endCol   = static_cast<int>(right / tileSize);
    int startRow = static_cast<int>(top   / tileSize);
    int endRow   = static_cast<int>(bottom/ tileSize);


    startCol = std::max(0, startCol);
    endCol   = std::min(endCol, mapWidth  - 1);
    startRow = std::max(0, startRow);
    endRow   = std::min(endRow, mapHeight - 1);

    return { startRow, startCol, endRow, endCol };
}

int GetLayerIdx(MapData map, std::string layerName) {
    for (int i=0; i<map.layers.size(); i++) {
        if (map.layers[i].name == layerName) {
            return i;
        }
    }
    return -1;
}

TileRenderInfo GetTileFlipInfoFromGID(uint32_t gid_with_flags) {
    constexpr uint32_t FLIP_H = 0x80000000;
    constexpr uint32_t FLIP_V = 0x40000000;
    constexpr uint32_t FLIP_D = 0x20000000;

    bool flip_h = gid_with_flags & FLIP_H;
    bool flip_v = gid_with_flags & FLIP_V;
    bool flip_d = gid_with_flags & FLIP_D;

    uint32_t clean_gid = gid_with_flags & ~(FLIP_H | FLIP_V | FLIP_D);

    double angle_deg = 0.0;
    SDL_RendererFlip flip = SDL_FLIP_NONE;

    if (!flip_h && !flip_v && !flip_d) {
        angle_deg = 0;
        flip = SDL_FLIP_NONE;
    }
    else if (flip_h && !flip_v && !flip_d) {
        angle_deg = 0;
        flip = SDL_FLIP_HORIZONTAL;
    }
    else if (!flip_h && flip_v && !flip_d) {
        angle_deg = 0;
        flip = SDL_FLIP_VERTICAL;
    }
    else if (flip_h && flip_v && !flip_d) {
        angle_deg = 180;
        flip = SDL_FLIP_NONE;
    }
    else if (!flip_h && !flip_v && flip_d) {
        angle_deg = 90;
        flip = SDL_FLIP_HORIZONTAL;
    }
    else if (flip_h && !flip_v && flip_d) {
        angle_deg = 270;
        flip = SDL_FLIP_NONE;
    }
    else if (!flip_h && flip_v && flip_d) {
        angle_deg = 90;
        flip = SDL_FLIP_NONE;
    }
    else if (flip_h && flip_v && flip_d) {
        angle_deg = 270;
        flip = SDL_FLIP_HORIZONTAL;
    }

    return {
        clean_gid,
        angle_deg,
        flip
    };

}