/* libtcod
 * Copyright © 2008-2018 Jice and the libtcod contributors.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * The name of copyright holder nor the names of its contributors may not
 *       be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef LIBTCOD_PATHFINDING_DIJKSTRA_H_
#define LIBTCOD_PATHFINDING_DIJKSTRA_H_
#ifdef __cplusplus
#include <cstdbool>
#include <cstddef>
#include <algorithm>
#include <array>
#include <limits>
#include <tuple>
#include <utility>
#include <vector>
#endif // __cplusplus
#include "../utility/vector2.h"

#ifdef __cplusplus
namespace tcod {
namespace pathfinding {

template <typename IndexType, typename DistType = ptrdiff_t>
class Pathfinder
{
 public:
  using heap_node = std::pair<DistType, IndexType>;
  using heap_type = std::vector<heap_node>;
  Pathfinder() = default;
  /**
   *  Continue the computation until is_goal is met or the heap runs empty.
   *
   *  distance_at(IndexType index) -> DistType&
   *  Return a reference to the distance value at index.  This function will
   *  read and write to these values.
   *
   *  get_edges(IndexType index, auto new_edge)
   *  Get the edges for index.  Edges are added using the new_edge callback:
   *  new_edge(IndexType destination, DistType cost)
   *
   *  is_goal(DistType dist, IndexType index) -> bool
   *  When this returns True or when all paths are exhausted the computation
   *  will end.
   *
   *  heuristic(DistType dist, IndexType index) -> DistType
   *  Determine the order of when nodes will be checked.  The lowest values
   *  are checked before higher values.
   *
   *  mark_path(IndexType current, IndexType next)
   *  Provides the best path from the current index so far.
   */
  template <typename DistanceAt, typename GetEdges, typename IsGoal,
            typename Heuristic, typename MarkPath>
  void compute(
      DistanceAt& distance_at,
      GetEdges& get_edges,
      IsGoal& is_goal,
      Heuristic& heuristic,
      MarkPath& mark_path)
  {
    while (heap_.size()) {
      const heap_node current_node = heap_.front();
      if (is_goal(current_node.first, current_node.second)) { return; }
      std::pop_heap(heap_.begin(), heap_.end(), heap_compare);
      heap_.pop_back();
      const DistType current_dist = current_node.first;
      const IndexType current_pos = current_node.second;
      if (current_dist > distance_at(current_pos)) { continue; }
      auto edge_lambda = [&](IndexType dest, DistType cost) {
          add_edge(current_pos, dest, cost, distance_at, heuristic, mark_path);
      };
      get_edges(current_pos, edge_lambda);
    }
  }
  /**
   *  Configure the heap.
   */
  void set_heap(heap_type&& heap) noexcept
  {
    heap_ = std::move(heap);
    std::make_heap(heap_.begin(), heap_.end(), heap_compare);
  }
 private:
  /**
   *  Compute an edge, adding the results to the distance map and heap.
   */
  template <typename DistanceAt, typename Heuristic, typename MarkPath>
  void add_edge(IndexType origin, IndexType dest, DistType cost,
                DistanceAt& distance_at, Heuristic& heuristic,
                MarkPath& mark_path)
  {
    if (cost <= 0) { return; }
    DistType distance = distance_at(origin) + cost;
    if (distance >= distance_at(dest)) { return; }
    distance_at(dest) = distance;
    mark_path(dest, origin);
    heap_.emplace_back(heuristic(distance, dest), dest);
    std::push_heap(heap_.begin(), heap_.end(), heap_compare);
  }
  /**
   *  Compare values in the heap.
   */
  static bool heap_compare(const heap_node& a, const heap_node& b) noexcept
  {
    return a.first < b.first;
  }
  /**
   *  A priority queue of which nodes to check next.
   */
  heap_type heap_ = {};
};
/**
 *  Create an unsorted heap from a distance map.
 */
template <typename GridType,
          typename IndexSize=ptrdiff_t,
          typename DistType=typename GridType::value_type>
auto dijkstra_make_heap(const GridType& dist_grid)
{
  const typename GridType::value_type MAX_DIST =
      std::numeric_limits<typename GridType::value_type>::max();
  using node_type = std::pair<DistType, std::pair<IndexSize, IndexSize>>;
  std::vector<node_type> heap = {};
  for (IndexSize y = 0; y < dist_grid.height(); ++y) {
    for (IndexSize x = 0; x < dist_grid.width(); ++x) {
      if (dist_grid.at(x, y) == MAX_DIST) { continue; }
      heap.emplace_back(node_type(dist_grid.at(x, y), {x, y}));
    }
  }
  return heap;
}
/**
 *  Private hard-coded edge positions.
 *
 *  {x, y, is_diagonal}
 */
static constexpr std::array<std::tuple<int, int, bool>, 8> EDGES_{{
    {-1, -1, 1}, {0, -1, 0}, {1, -1, 1},
    {-1, 0, 0}, {1, 0, 0},
    {-1, 1, 1}, {0, 1, 0}, {1, 1, 1},
}};
/**
 *  Recompute a Dijkstra distance map.
 */
template <typename DistGrid, typename CostGrid, typename CostType,
          typename PathGrid>
void dijkstra_compute(DistGrid& dist_grid, const CostGrid& cost_grid,
                      CostType cardinal=2, CostType diagonal=3,
                      PathGrid* path_grid=nullptr)
{
  using dist_type = typename DistGrid::value_type;
  using index_type = std::pair<ptrdiff_t, ptrdiff_t>;

  auto distance_at = [&](index_type index) -> dist_type& {
    return dist_grid.at(index);
  };
  auto get_edges = [&](index_type index, auto new_edge) {
    for (const auto& edge : EDGES_) {
      std::pair<ptrdiff_t, ptrdiff_t> other_pos{
          index.first + std::get<0>(edge),
          index.second + std::get<1>(edge),
      };
      if (other_pos.first < 0 || other_pos.first >= dist_grid.width() ||
          other_pos.second < 0 || other_pos.second >= dist_grid.height()) {
        continue;
      }
      auto cost = cost_grid.at(other_pos);
      cost *= std::get<2>(edge) ? diagonal : cardinal;
      if (cost <= 0) { continue; }
      new_edge(other_pos, cost);
    }
  };
  auto is_goal = [](auto, auto) { return 0; };
  auto heuristic = [](auto dist, auto){ return dist; };
  auto mark_path = [&](auto index, auto next) {
    if(path_grid) { path_grid->at(index) = next; }
  };

  Pathfinder<index_type, dist_type> pathfinder;
  pathfinder.set_heap(dijkstra_make_heap(dist_grid));
  pathfinder.compute(distance_at, get_edges, is_goal, heuristic, mark_path);
}
/**
 *  Clear a distance map by setting all values to maximum.
 */
template <typename DistGrid>
void distance_clear(DistGrid& dist_grid) noexcept
{
  for (auto& i : dist_grid) {
    i = std::numeric_limits<typename DistGrid::value_type>::max();
  }
}
/**
 *  Clear a path map, setting all paths to themselves.
 */
template <typename PathGrid>
void path_clear(PathGrid& path_grid) noexcept
{
  for (ptrdiff_t y = 0; y < path_grid.height(); ++y) {
    for (ptrdiff_t x = 0; x < path_grid.width(); ++x) {
      path_grid.at(x, y) = {x, y};
    }
  }
}
} // namespace pathfinding
} // namespace tcod
#endif // __cplusplus
#endif // LIBTCOD_PATHFINDING_DIJKSTRA_H_
