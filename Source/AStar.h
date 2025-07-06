//
// Created by Lucas on 04/07/2025.
//

#include "Game.h"
#include "Math.h"
#include <vector>
#include <queue>

#pragma once

struct Node {
    int r, c;
    float f;         // f = g + h
    float g;         // custo até aqui
    Node* parent;
};

struct NodeCmp {
    bool operator()(Node* a, Node* b) const {
        return a->f > b->f;  // min-heap
    }
};

std::vector<Vector2> FindPathAStar(const std::vector<std::vector<bool>>& passable,
    int startR, int startC, int goalR,  int goalC);
