//
// Created by Lucas on 04/07/2025.
//

#include "AStar.h"


std::vector<Vector2> FindPathAStar(
    const std::vector<std::vector<bool>>& passable,
    int startR, int startC,
    int goalR,  int goalC)
{
  int H = passable.size(), W = passable[0].size();
  auto inBounds = [&](int r, int c){
    return r >= 0 && r < H && c >= 0 && c < W;
  };
  // Heurística: distância Manhattan
  auto h = [&](int r, int c){
    return float(abs(r - goalR) + abs(c - goalC));
  };

  std::priority_queue<Node*, std::vector<Node*>, NodeCmp> open;
  std::vector<std::vector<bool>> closed(H, std::vector<bool>(W,false));
  std::vector<std::vector<float>> bestG(H, std::vector<float>(W, 1e9f));

  // cria o nó inicial
  Node* start = new Node{startR, startC, h(startR,startC), 0.f, nullptr};
  open.push(start);
  bestG[startR][startC] = 0.f;

  Node* goalNode = nullptr;
  const int dr[4] = {-1,1,0,0};
  const int dc[4] = {0,0,-1,1};

  while (!open.empty()) {
    Node* cur = open.top(); open.pop();
    if (closed[cur->r][cur->c]) { delete cur; continue; }
    if (cur->r == goalR && cur->c == goalC) {
      goalNode = cur;
      break;
    }
    closed[cur->r][cur->c] = true;

    for (int i = 0; i < 4; ++i) {
      int nr = cur->r + dr[i], nc = cur->c + dc[i];
      if (!inBounds(nr,nc) || !passable[nr][nc] || closed[nr][nc])
        continue;
      float g2 = cur->g + 1.0f;  // custo unitário
      if (g2 < bestG[nr][nc]) {
        bestG[nr][nc] = g2;
        Node* nxt = new Node{nr,nc, g2 + h(nr,nc), g2, cur};
        open.push(nxt);
      }
    }
  }

  // Reconstrói o caminho
  std::vector<Vector2> path;
  for (Node* p = goalNode; p; p = p->parent) {
    // converte r,c para coordenada de mundo: centro do tile
    float x = (p->c + 0.5f) * Game::TILE_SIZE * Game::SCALE;
    float y = (p->r + 0.5f) * Game::TILE_SIZE * Game::SCALE;
    path.emplace_back(x,y);
  }
  // limpa nós
  while (!open.empty()) { delete open.top(); open.pop(); }
  if (goalNode) {
    for (Node* p = goalNode->parent; p; p = p->parent) delete p;
    delete goalNode;
  }
  std::reverse(path.begin(), path.end());
  return path;
}