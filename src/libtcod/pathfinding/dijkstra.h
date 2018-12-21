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
#include <utility>
#include <vector>
#endif // __cplusplus
#include "../utility/vector2.h"

#ifdef __cplusplus
namespace tcod {
namespace pathfinding {

// {distance, {x, y}}
using DijkstraNode_ = std::pair<ptrdiff_t, std::pair<ptrdiff_t, ptrdiff_t>>;
using DijkstraHeap_ = std::vector<DijkstraNode_>;
bool dijkstra_heap_cmp_(DijkstraNode_ a, DijkstraNode_ b)
{
  return a.first < b.first;
}
/**
 *  Create a heap from a distance map.
 */
template <typename GridType>
auto dijkstra_make_heap(const GridType& dist_grid) -> DijkstraHeap_
{
  const typename GridType::value_type MAX_DIST =
      std::numeric_limits<typename GridType::value_type>::max();
  DijkstraHeap_ heap = {};
  for (ptrdiff_t y = 0; y < dist_grid.height(); ++y) {
    for (ptrdiff_t x = 0; x < dist_grid.width(); ++x) {
      if (dist_grid.at(x, y) == MAX_DIST) { continue; }
      heap.emplace_back({dist_grid.at(x, y), {x, y}});
    }
  }
  std::make_heap(heap.begin(), heap.end(), dijkstra_heap_cmp_);
  return heap;
}
/**
 *  Hard-coded edge positions.
 */
static constexpr std::array<std::array<int, 2>, 8> EDGES_{{
    {{-1, -1}}, {{0, -1}}, {{1, -1}}, {{-1, -0}}, {{1, 0}},
    {{-1, 1}}, {{0, 1}}, {{1, 1}},
}};

template <typename GridType, typename EdgeCostFunc>
void dijkstra_compute(GridType& dist_grid, EdgeCostFunc& cost_func)
{
  using dist_type = typename GridType::value_type;
  DijkstraHeap_ heap = dijkstra_make_heap(dist_grid);
  while (heap.size()) {
    const DijkstraNode_ current_node = heap.front();
    std::pop_heap(heap.begin(), heap.end(), dijkstra_heap_cmp_);
    heap.pop_back();
    const dist_type& current_dist = current_node.first;
    const auto& current_pos = current_node.second;
    if (current_dist > dist_grid.at(current_node.second)) { continue; }
    for (const auto& edge : EDGES_) {
      std::pair<ptrdiff_t, ptrdiff_t> other_pos{
          current_pos.first + edge.at(0),
          current_pos.second + edge.at(1),
      };
      if (other_pos.first < 0 || other_pos.first >= dist_grid.width() ||
          other_pos.second < 0 || other_pos.second >= dist_grid.height()) {
        continue;
      }
      dist_type edge_cost = cost_func(other_pos, current_pos);
      if (edge_cost <= 0) { continue; }
      dist_type other_dist = current_dist + edge_cost;
      if (other_dist > dist_grid.at(other_pos)) { continue; }
      dist_grid.at(other_pos) = other_dist;
      heap.emplace_back({other_dist, other_pos});
      std::push_heap(heap.begin(), heap.end(), dijkstra_heap_cmp_);
    }
  }
}
/**
 *  Clear a Dijkstra distance map by setting all values to maximum.
 */
template <typename GridType>
void dijkstra_clear(GridType& dist_grid) noexcept
{
  for (auto& i : dist_grid) {
    i = std::numeric_limits<typename GridType::value_type>::max();
  }
}
} // namespace pathfinding
} // namespace tcod
#endif // __cplusplus
#endif // LIBTCOD_PATHFINDING_DIJKSTRA_H_
