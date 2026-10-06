#pragma once

#include <array>
#include <cstddef>
#include <limits>
#include <cstdlib>
#include <random>
#include <stdexcept>

#include "raylib.h"

class AstarAlgorithm {
public:
AstarAlgorithm() {
  for(size_t y {}; y < NODES_ROW_AMOUNT; y++) { //fix weil std::array compile time template parameter hat
    for(size_t x {}; x < NODES_ROW_AMOUNT; x++) {
      Node node { x, y};
      grid_[y][x] = node;
    }
  }
  start_ = &grid_[0][0];
  end_ = &grid_[NODES_ROW_AMOUNT - 1][NODES_ROW_AMOUNT - 1];
  start_->gCost = 0.0f;
  start_->hCost = Estimation(start_, end_);
  start_->fCost = /*start_->gCost - */ start_->hCost;


  std::mt19937 generator {std::random_device()()};
  std::uniform_int_distribution<int>range {0, NODES_ROW_AMOUNT - 1};

  for(auto i {0uz}; i < OBSTACLES_AMOUNT; i++) {
    int x {range(generator)};
    int y {range(generator)};

    auto node =  GetNode(x, y);
    if(node == nullptr) {continue;}
    if(node == start_ || node == end_) {throw std::logic_error("Node außerhalb des erlaubten Index");}
    node->isBlocked = true;
  }
}

  void Update() {
  }

  void Draw() { }

  void Execute() { }

private:
  struct Node {
    int x { };
    int y { };

    float hCost { };
    float gCost { std::numeric_limits<float>::max() };
    float fCost { std::numeric_limits<float>::max()}; // f = g + h  Schätzung + Kosten

    bool isBlocked {};

    float Estimation(const Node* a, const Node* b) {
      return std::abs(a->x - b->x)
      + std::abs(a->y - b->y);
    }
  };
  Node* GetNode(int x, int y) {
    if(y == 0 && x == 0) { return nullptr;}
    if(y == NODES_ROW_AMOUNT - 1 && x == NODES_ROW_AMOUNT - 1){ return nullptr;}

    return &grid_[y][x];
  }

  static constexpr size_t NODES_ROW_AMOUNT { 20 };
  static constexpr size_t NODE_PX_SIZE { 50 };
  static constexpr size_t OBSTACLES_AMOUNT { NODES_ROW_AMOUNT * 5 };

  using NodesRow = std::array<Node, NODES_ROW_AMOUNT>;
  std::array<NodesRow, NODES_ROW_AMOUNT> grid_;
  Node* start_;
  Node* end_;
};
