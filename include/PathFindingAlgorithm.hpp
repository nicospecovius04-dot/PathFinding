#pragma once
#include <concepts>

template <typename T>
concept PathFindingAlgorithm = requires(T algo) {
  { algo.Execute() } -> std::same_as<void>; // returnvalue
};
