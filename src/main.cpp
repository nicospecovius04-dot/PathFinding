#include "PathFindingAlgorithm.hpp"
#include "Raylib.h"
template <PathFindingAlgorithm Algo>
void run(Algo& algo) {
  algo.Execute();
}

int main() {
  InitWindow(1000, 1000, "Pathfinding Algorithms");
  SetTargetFPS(60);
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);
    EndDrawing();
  }
  /*AstarAlgorithm astar{};
  run(astar);*/

  return 0;
};
