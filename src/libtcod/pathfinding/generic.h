/* BSD 3-Clause License
 *
 * Copyright © 2008-2019, Jice and the libtcod contributors.
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
#ifndef LIBTCOD_PATHFINDING_GENERIC_H_
#define LIBTCOD_PATHFINDING_GENERIC_H_
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
template <typename IndexType>
void path_clear(Vector2<IndexType>& path_grid) noexcept
{
  for (ptrdiff_t y = 0; y < path_grid.height(); ++y) {
    for (ptrdiff_t x = 0; x < path_grid.width(); ++x) {
      path_grid.at(x, y) = {x, y};
    }
  }
}
template <typename IndexType>
auto get_path(const Vector2<IndexType>& path_grid,
              const IndexType& start) -> std::vector<IndexType>
{
  std::vector<IndexType> path;
  IndexType current = start;
  IndexType next = path_grid.at(current);
  while (current != next) {
    path.emplace_back(next);
    current = next;
    next = path_grid.at(current);
  }
  return path;
}
} // namespace pathfinding
} // namespace tcod
#endif // __cplusplus
#endif // LIBTCOD_PATHFINDING_GENERIC_H_
