/* BSD 3-Clause License
 *
 * Copyright © 2008-2020, Jice and the libtcod contributors.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef LIBTCOD_PATHFINDING_DIJKSTRA_H_
#define LIBTCOD_PATHFINDING_DIJKSTRA_H_
#ifdef __cplusplus
#include <algorithm>
#include <array>
#include <cstdbool>
#include <cstddef>
#include <limits>
#include <tuple>
#include <utility>
#include <vector>
#endif  // __cplusplus
#include "../utility/vector2.h"
#include "generic.h"
#include "graph.h"

#ifdef __cplusplus
namespace tcod {
namespace pathfinding {
/**
 *  Create an unsorted heap from a distance map.
 */
template <typename GridType, typename IndexSize = ptrdiff_t, typename DistType = typename GridType::value_type>
auto dijkstra_make_heap(const GridType& dist_grid) {
  const typename GridType::value_type MAX_DIST = std::numeric_limits<typename GridType::value_type>::max();
  using node_type = std::tuple<DistType, std::array<IndexSize, 2>>;
  std::vector<node_type> heap = {};
  for (IndexSize y = 0; y < dist_grid.height(); ++y) {
    for (IndexSize x = 0; x < dist_grid.width(); ++x) {
      if (dist_grid.at({y, x}) == MAX_DIST) {
        continue;
      }
      heap.emplace_back(node_type(dist_grid.at({y, x}), {y, x}));
    }
  }
  return heap;
}
/**
 *  Recompute a Dijkstra distance map.
 */
template <typename DistGrid, typename CostGrid, typename CostType, typename PathGrid>
void dijkstra_compute(
    DistGrid& dist_grid,
    const CostGrid& cost_grid,
    CostType cardinal = 2,
    CostType diagonal = 3,
    PathGrid* path_grid = nullptr) {
  using dist_type = typename DistGrid::value_type;
  using index_type = std::array<ptrdiff_t, 2>;

  if (cost_grid.width() > dist_grid.width() || cost_grid.height() > dist_grid.height()) {
    throw std::invalid_argument("cost_grid must be smaller than the dist_grid.");
  }
  if (path_grid && (path_grid->width() > dist_grid.width() || path_grid->height() > dist_grid.height())) {
    throw std::invalid_argument("path_grid must be smaller than the dist_grid.");
  }

  auto distance_at = [&](index_type index) -> dist_type& { return dist_grid[index]; };
  auto get_edges = [&](index_type index, auto new_edge) {
    for (const auto& edge : EDGES_) {
      std::array<ptrdiff_t, 2> other_pos{
          index.at(0) + std::get<0>(edge),
          index.at(1) + std::get<1>(edge),
      };
      if ((std::get<0>(edge) && (other_pos.at(0) < 0 || other_pos.at(0) >= dist_grid.height())) ||
          (std::get<1>(edge) && (other_pos.at(1) < 0 || other_pos.at(1) >= dist_grid.width()))) {
        continue;
      }
      auto cost = cost_grid[other_pos];
      const bool is_diagonal = std::get<0>(edge) && std::get<1>(edge);
      cost *= is_diagonal ? diagonal : cardinal;
      if (cost <= 0) {
        continue;
      }
      new_edge(other_pos, cost);
    }
  };
  auto is_goal = [](auto, auto) { return 0; };
  auto heuristic = [](auto dist, auto) { return dist; };
  auto mark_path = [&](auto index, auto next) {
    if (path_grid) {
      (*path_grid)[index] = next;
    }
  };

  Pathfinder<index_type, dist_type> pathfinder;
  pathfinder.set_heap(dijkstra_make_heap(dist_grid), heuristic);
  pathfinder.compute(distance_at, get_edges, is_goal, heuristic, mark_path);
}
template <typename HeapNode>
inline bool dijktra_heap_sort_(const HeapNode& a, const HeapNode& b) noexcept {
  return std::get<0>(a) > std::get<0>(b);
}
template <typename DistMatrix, typename Graph, typename GoalFunc, typename EdgeFunc>
inline void dijkstra(
    DistMatrix& dist_map,
    const Graph& graph,
    const GoalFunc& goal_func,  // goal_func(position) -> bool
    const EdgeFunc& edge_func)  // edge_func(origin, destination)
{
  using dist_type = typename DistMatrix::value_type;
  const dist_type MAX_DIST = std::numeric_limits<dist_type>::max();
  using index_type = std::array<ptrdiff_t, 2>;
  using heap_node = std::tuple<dist_type, index_type>;
  auto heap_compare = [](const heap_node& a, const heap_node& b) { return std::get<0>(a) > std::get<0>(b); };
  std::vector<heap_node> heap;
  for (ptrdiff_t i = 0; i < dist_map.get_shape().at(0); ++i) {
    for (ptrdiff_t j = 0; j < dist_map.get_shape().at(1); ++j) {
      if (dist_map[{i, j}] >= MAX_DIST) {
        continue;
      }
      heap.emplace_back(heap_node{dist_map[{i, j}], {i, j}});
    }
  }
  std::make_heap(heap.begin(), heap.end(), heap_compare);
  while (heap.size()) {
    const heap_node current_node = heap.front();
    if (goal_func(std::get<1>(current_node))) {
      return;
    }
    std::pop_heap(heap.begin(), heap.end(), heap_compare);
    heap.pop_back();
    const dist_type& current_distance = std::get<0>(current_node);
    const index_type& current_point = std::get<1>(current_node);
    auto add_edge = [&](const index_type& next, const dist_type& cost) {
      dist_type next_distance = current_distance + cost;
      if (dist_map[next] <= next_distance) {
        return;
      }
      dist_map[next] = next_distance;
      heap.emplace_back(heap_node{next_distance, next});
      std::push_heap(heap.begin(), heap.end(), heap_compare);
      edge_func(current_point, next);
    };
    graph.with_edges(add_edge, current_point);
  }
}
template <typename DistMatrix, typename Graph>
inline void dijkstra(DistMatrix& dist_map, const Graph& graph) {
  const auto goal_func = [](auto) { return 0; };
  const auto edge_func = [](auto, auto) { return; };
  return dijkstra(dist_map, graph, goal_func, edge_func);
}
template <typename DistMatrix, typename CostMatrix>
inline void dijkstra2d(DistMatrix& dist_map, const CostMatrix& cost_map, int cardinal = 1, int diagonal = 1) {
  auto graph = SimpleGraph2D<CostMatrix>(cost_map, cardinal, diagonal);
  dijkstra(dist_map, graph);
}
}  // namespace pathfinding
}  // namespace tcod
#endif  // __cplusplus
#endif  // LIBTCOD_PATHFINDING_DIJKSTRA_H_
