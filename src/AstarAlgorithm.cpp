#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <exception>
#include <limits>
#include <cstdlib>
#include <queue>
#include <random>
#include <stdexcept>
#include <vector>

#include "raylib.h"

struct Node {
  int y;
  int x;

  bool isobstacle { };
  bool isOpen { };
  bool isClosed { };

  float hCost { };
  float gCost { std::numeric_limits<float>::max() };
  float fCost { std::numeric_limits<float>::max() }; // f = g + h  Schätzung + Kosten

  Node* parent { };

  bool operator==(const Node& node) const { return x == node.x && y == node.y; }
};

using Nodes = std::vector<Node>;

class Collum {
public:
  Collum(const size_t rows)
      : nodes_(rows) {
    if (nodes_.empty()) {
      throw std::logic_error("Wenigstens eine Reihe son");
    }
  }

  [[nodiscard]] bool CanPlaceObstacle(const size_t position) const {
    return position != 0 && position != nodes_.size() - 1;
  }

  [[nodiscard]] int GetHeight() const { return static_cast<int>(nodes_.size()); }

  Node& operator[](const size_t row) {
    if (row >= nodes_.size()) {
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
  Grid(const size_t seite)
      : Grid(seite, seite) { }

  Grid(const size_t collums, const size_t rows)
      : field_(Field(collums, Collum(rows))) {

    for (auto i { 0uz }; i < collums; i++) {
      for (auto j { 0uz }; j < rows; j++) {
        auto& positionDurchlauf = field_.at(i)[j];
        positionDurchlauf = Node { static_cast<int>(j), static_cast<int>(i) };
      }
    }
  }

  [[nodiscard]] float HeuristicEstimation(const Node& one, const Node& two) const {
    return std::abs(one.x - two.x) + std::abs(one.y - two.y);
  }

  void ObstacleRandomizationPlacement(const size_t obstacleAmount) {
    if (obstacleAmount >= (field_.size() * field_.at(0).GetHeight()) / 2) {
      throw std::logic_error("Nicht zu viele Obstacles, damit der Weg nicht versperrt wird");
    }
    static std::mt19937 generator { std::random_device()() };
    std::uniform_int_distribution<int> rangeRows { 0, field_.at(0).GetHeight() - 1 };
    std::uniform_int_distribution<int> rangeCollums { 0, static_cast<int>(field_.size() - 1) };

    for (auto i { 0uz }; i < obstacleAmount;) {
      int y { rangeRows(generator) };
      int x { rangeCollums(generator) };
      if ((x == 0 && y == 0)
          || (x == static_cast<int>(field_.size()) - 1 && y == field_.at(x).GetHeight() - 1)) {
        continue;
      }
      if (field_.at(x)[y].isobstacle) {
        continue;
      }
      field_.at(x)[y].isobstacle = true;
      i++;
    }
  }

  [[nodiscard]] Field& GetGrid() { return field_; }

  [[nodiscard]] int GetNodePixelSize() const { return nodePixelSize_; }

  [[nodiscard]] Node* GetNode(int x, int y) {
    if (x < 0 || x >= static_cast<int>(field_.size())) {
      return nullptr;
    }

    if (y < 0 || y >= field_.at(x).GetHeight()) {
      return nullptr;
    }

    return &field_.at(x)[y];
  }

private:
  Field field_;
  const int nodePixelSize_ { 50 };
};

class GUIs {
public:
  explicit GUIs() = default;
  virtual ~GUIs() = default;
  virtual void Draw() = 0;
};

class AStarAlgo {
public:
  AStarAlgo(Grid& grid)
      : grid_(grid)
      , startNode_(grid_.GetGrid().at(0)[0])
      , lastCollum_(grid_.GetGrid().back())
      , endNode_(lastCollum_[lastCollum_.GetHeight() - 1]) {
    startNode_.gCost = 0;
    startNode_.hCost = grid_.HeuristicEstimation(startNode_, endNode_);
    startNode_.fCost = startNode_.hCost;
    startNode_.isOpen = true;
    opened_.push({ &startNode_, startNode_.fCost });
  }

  void Update() {
    if (isFinished_) {
      return;
    }
    if (opened_.empty()) {
      isFinished_ = true;
      return;
    }

    auto current { opened_.top() };
    opened_.pop();
    if (current.node->isClosed)
      return;
    if (current.fCost != current.node->fCost) {
      return;
    }
    current.node->isClosed = true;
    current.node->isOpen = false;
    if (*current.node == endNode_) {
      ReconstructPath();
      isFinished_ = true;
      return;
    }
    ExpandNote(current.node);
  }

  struct OpenNode {
    Node* node;
    float fCost;

    bool operator<(const OpenNode& other) const {
      return fCost > other.fCost;
    } // kleinster wert soll oben liegen, weil kleinster fCost, also node ist kleiner als other wenn
      // größere fcost besteht = true
  };

  [[nodiscard]] const Node& GetStartNode() const { return startNode_; }

  [[nodiscard]] const Node& GetEndNode() const { return endNode_; }

  [[nodiscard]] const auto& GetFinalPath() const { return finalpath_; }

  void ExpandNote(Node* node) {

    for (auto* neighbour : GetNeighbours(node)) {
      if (neighbour->isobstacle || neighbour->isClosed) {
        continue;
      }
      const float newGCost { node->gCost + 1 };
      if (newGCost >= neighbour->gCost)
        continue;
      neighbour->gCost = newGCost;
      neighbour->hCost = grid_.HeuristicEstimation(*neighbour, endNode_);
      neighbour->fCost = neighbour->hCost + neighbour->gCost;
      neighbour->parent = node;
      neighbour->isOpen = true;
      opened_.push({ neighbour, neighbour->fCost });
    }
  }

  [[nodiscard]] std::vector<Node*> GetNeighbours(Node* node) const {
    std::vector<std::pair<int, int>> directions { { 1, 0 }, { 0, 1 }, { -1, 0 }, { 0, -1 } };
    std::vector<Node*> output { };
    for (const auto direction : directions) {
      auto possibleNeighbour { grid_.GetNode(
          node->x + direction.first, node->y + direction.second) };
      if (!possibleNeighbour)
        continue;
      output.push_back(possibleNeighbour);
    }
    return output;
  }

  void ReconstructPath() {
    auto* current { &endNode_ };
    while (current != nullptr) {
      finalpath_.push_back(current);
      current = current->parent;
    }
    std::reverse(finalpath_.begin(), finalpath_.end());
  }

private:
  Grid& grid_;
  Node& startNode_;
  Collum& lastCollum_;
  Node& endNode_;

  std::priority_queue<OpenNode> opened_ { };
  std::vector<Node*> finalpath_ { };

  bool isFinished_ { };
};

// #include "raylib.h"  muss noch in klasse sein eig
class RayLibImplementation : public GUIs {
public:
  RayLibImplementation(Grid& grid, AStarAlgo& algocalculation)
      : grid_(grid)
      , algocalculation_(algocalculation) {
    grid_.ObstacleRandomizationPlacement(50);
  }

  void Draw() override {

    InitWindow(1000, 1000, "Pathfinding");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
      algocalculation_.Update();

      BeginDrawing();
      ClearBackground(BLACK);

      this->DrawNotes();

      EndDrawing();
    }
    CloseWindow();
  }

private:
  void DrawNotes() {
    auto& startNode = algocalculation_.GetStartNode();
    auto& endNode = algocalculation_.GetEndNode();
    for (auto collum { 0uz }; collum < grid_.GetGrid().size(); collum++) {
      for (auto rows { 0uz }; rows < static_cast<size_t>(grid_.GetGrid().at(collum).GetHeight());
          rows++) {
        const auto& node = grid_.GetGrid().at(collum)[rows];
        Color color { GRAY };

        if (node.isobstacle) {
          color = BLACK;
        } else if (node.isClosed) {
          color = BLUE;
        } else if (node.isOpen) {
          color = ORANGE;
        }

        DrawRectangle(collum * grid_.GetNodePixelSize(), rows * grid_.GetNodePixelSize(),
            grid_.GetNodePixelSize(), grid_.GetNodePixelSize(), color);
      }
    }
    for (const auto node : algocalculation_.GetFinalPath()) {
      DrawRectangle(node->x * grid_.GetNodePixelSize(), node->y * grid_.GetNodePixelSize(),
          grid_.GetNodePixelSize(), grid_.GetNodePixelSize(), YELLOW);
    }
    DrawRectangle(startNode.x * grid_.GetNodePixelSize(), startNode.y * grid_.GetNodePixelSize(),
        grid_.GetNodePixelSize(), grid_.GetNodePixelSize(), GREEN);
    DrawRectangle(endNode.x * grid_.GetNodePixelSize(), endNode.y * grid_.GetNodePixelSize(),
        grid_.GetNodePixelSize(), grid_.GetNodePixelSize(), RED);
  }

private:
  Grid& grid_; // has-a
  AStarAlgo& algocalculation_;
};

class Simulate {
public:
  Simulate(GUIs& gui)
      : GUI_(gui) { }

  void Drawing() { GUI_.Draw(); }

private:
  GUIs& GUI_;
};

int main() {
  Grid grid { 20 };
  AStarAlgo astaralgorithm { grid };
  RayLibImplementation raylib { grid, astaralgorithm };

  Simulate simulate { raylib };
  simulate.Drawing();
  return 0;
}

// Node* GetNode(int x, int y) {
// if(y == 0 && x == 0) { return nullptr;}
// if(y == NODES_ROW_AMOUNT - 1 && x == NODES_ROW_AMOUNT - 1){ return nullptr;}

// return &grid_[y][x];
//}

/*static constexpr size_t NODES_ROW_AMOUNT { 20 };
static constexpr size_t NODE_PX_SIZE { 50 };
static constexpr size_t OBSTACLES_AMOUNT { NODES_ROW_AMOUNT * 5 };

using NodesRow = std::array<Node, NODES_ROW_AMOUNT>;

std::array<NodesRow, NODES_ROW_AMOUNT> grid_;
Node* start_;
Node* end_;
*/