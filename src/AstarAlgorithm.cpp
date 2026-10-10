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
  bool isobstacle { };
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

  [[nodiscard]] int GetHeight() const {
    return static_cast<int>(nodes_.size());
  }
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
      : field_(
            Field(collums, Collum(rows))) {

    for (auto i { 0 }; i < collums; i++) {
      for (auto j { 0 }; j < rows; j++) {
        auto& positionDurchlauf = field_.at(i)[j];
        positionDurchlauf = Node { j, i };
      }
    }
  }

  [[nodiscard]] float HeuristicEstimation(Node one, Node two) {
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
      if (x == 0 && y == 0) {
        continue;
      }
      if (field_.at(x)[y].isobstacle) {
        continue;
      }
      field_.at(x)[y].isobstacle = true;
      i++;
    }
  }

  [[nodiscard]] Field& GetGrid() {
    return field_;
  }

  [[nodiscard]] int GetNodePixelSize() const {
    return nodePixelSize_;
  }

private:
  Field field_;
  const int nodePixelSize_ { 50 };
  float hCost_ { };
  float gCost_ { std::numeric_limits<float>::max() };
  float fCost_ { std::numeric_limits<float>::max() }; // f = g + h  Schätzung + Kosten
};

class GUIs {
public:
  explicit GUIs() = default;
  virtual ~GUIs() = default;
  virtual void Draw() = 0;
};

// #include "raylib.h"  muss noch in klasse sein eig
class RayLibImplementation : public GUIs {
public:
  RayLibImplementation(Grid grid)
      : grid_(std::move(grid)) { }

  void Draw() override {
    grid_.ObstacleRandomizationPlacement(1);

    InitWindow(1000, 1000, "Pathfinding");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
      BeginDrawing();
      ClearBackground(BLACK);

      this->DrawNotes();

      EndDrawing();
    }
  }

private:
  void DrawNotes() {
    for (int x { }; x < grid_.GetGrid().size(); x++) {
      for (int y { }; y < grid_.GetGrid().at(x).GetHeight(); y++) {
        const Node& node = grid_.GetGrid().at(x)[y];
        Color color { GRAY };
        if (node.isobstacle)
          color = BLACK;

        DrawRectangle(x * grid_.GetNodePixelSize(), y * grid_.GetNodePixelSize(), grid_.GetNodePixelSize(), grid_.GetNodePixelSize(), color);
      }
    }
  }

private:
  Grid grid_; // sollte eig. ein header sein ig(has-a beziehung; A* logik)
};

class Simulate {
public:
  Simulate(GUIs& gui)
      : GUI_(gui) { }

  void Drawing() {
    GUI_.Draw();
  }

private:
  GUIs& GUI_;
};

int main() {
  RayLibImplementation raylib { Grid { 20  } };
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