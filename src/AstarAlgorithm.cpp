#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <cstdlib>
#include <random>
#include <stdexcept>
#include <vector>

#include "raylib.h"

struct Node {
    int y;
    int x;
};

using Nodes = std::vector<Node>;

class Collum {
public:
  Collum(const size_t rows) : nodes_(rows) {
    if(nodes_.empty()) {
      throw std::logic_error("Wenigstens eine Reihe son");
    }
  }
  
  [[nodiscard]] bool CanPlaceObstacle(const size_t position) {
    return position != 0 && position != nodes_.size() - 1;
  }

  int GetHeight() {
    return nodes_.size();
  }
  Node& operator[](const size_t row) {
    if(row >= nodes_.size()) {
    throw std::logic_error("Außerhalb des Indexes");
    }
    return nodes_.at(row);
  }
private:
  Nodes nodes_;
};

using Field = std::vector<Collum>;

class Grid {
public:
  Grid(const size_t seite) : field_(Field(seite,Collum(seite))) {}
  Grid(const size_t collums, const size_t rows) : field_(
    Field(collums,Collum(rows))
  ) {
    
      for(auto i{0}; i < collums; i++) {
        for(auto j{0}; j < rows; j++) {
          auto& positionDurchlauf = field_.at(i)[j];
          positionDurchlauf = Node{i, j};
        }
      }
    }

  float Estimation(Node one, Node two) {
    return std::abs(one.x) + std::abs(two.x) - std::abs(one.y) + std::abs(two.y);
  }

  void ObstacleRandomizationPlacement(const size_t obstacleAmount) {
    static std::mt19937 generator {std::random_device()()};
    std::uniform_int_distribution<int>range {0, field_.at(0).GetHeight() - 1}; //einfach irgendeine Spalte, weil zeilen sind für jede Spalte gleich und dann irgendeine zeile 

    for(auto i {0uz}; i < obstacleAmount; i++) {
    int x {range(generator)};
    int y {range(generator)};
    }
  }
private:
  Field field_;
  float hCost_ { };
  float gCost_ { std::numeric_limits<float>::max() };
  float fCost_ { std::numeric_limits<float>::max()}; // f = g + h  Schätzung + Kosten
  bool isBlocked_ {};
};

class Simulate {
public:
Simulate() {


 

    //auto node =  GetNode(x, y);
    //if(node == nullptr) {continue;}
    //if(node == start_ || node == end_) {throw std::logic_error("Node außerhalb des erlaubten Index");}
    //node->isBlocked = true;
  }

  void Update() {
  }

  void Draw() { }

  void Execute() { }

    //float Estimation(const Node* a, const Node* b) {
      //return std::abs(a->x - b->x)
     // + std::abs(a->y - b->y);
    //}
  };
  
  
  
  
  
  //Node* GetNode(int x, int y) {
    //if(y == 0 && x == 0) { return nullptr;}
    //if(y == NODES_ROW_AMOUNT - 1 && x == NODES_ROW_AMOUNT - 1){ return nullptr;}

    //return &grid_[y][x];
  //}

  
  
  /*static constexpr size_t NODES_ROW_AMOUNT { 20 };
  static constexpr size_t NODE_PX_SIZE { 50 };
  static constexpr size_t OBSTACLES_AMOUNT { NODES_ROW_AMOUNT * 5 };

  using NodesRow = std::array<Node, NODES_ROW_AMOUNT>;

  std::array<NodesRow, NODES_ROW_AMOUNT> grid_;
  Node* start_;
  Node* end_;
  */