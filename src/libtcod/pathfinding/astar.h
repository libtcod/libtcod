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
#ifndef LIBTCOD_PATHFINDING_ASTAR_H_
#define LIBTCOD_PATHFINDING_ASTAR_H_
#ifdef __cplusplus
#include <algorithm>
#include <array>
#include <cstdlib>
#include <cmath>
#include <limits>
#include <tuple>
#include <vector>

#include "graph.h"
namespace tcod {
namespace pathfinding {
template <typename DistType, typename IndexType>
inline DistType test_heuristic(const IndexType& a, const IndexType& b) {
  return std::max(std::abs(a.at(0) - b.at(0)), std::abs(a.at(1) - b.at(1)));
}
template <typename HeapNode>
inline bool astar_heap_sort_(const HeapNode& a, const HeapNode& b) noexcept
{
  return std::get<0>(a) > std::get<0>(b);
}
template <typename DistMatrix, typename CostMatrix, typename IndexType = typename DistMatrix::index_type>
inline void astar2d(
  DistMatrix& dist_map,
  const CostMatrix& cost_map,
  const IndexType& goal_point)
{
  using dist_type = typename DistMatrix::value_type;
  const dist_type MAX_DIST = std::numeric_limits<dist_type>::max();
  using heap_node = std::tuple<dist_type, IndexType>;
  std::vector<heap_node> heap;
  for (ptrdiff_t i = 0; i < dist_map.get_shape().at(0); ++i) {
    for (ptrdiff_t j = 0; j < dist_map.get_shape().at(1); ++j) {
      if (dist_map[{i, j}] >= MAX_DIST) { continue; }
      heap.emplace_back(heap_node{ dist_map[{i, j}] + test_heuristic<dist_type>({i, j}, goal_point), {i, j} });
    }
  }
  std::make_heap(heap.begin(), heap.end(), astar_heap_sort_<heap_node>);
  while (heap.size()) {
    const IndexType current_point = std::get<1>(heap.front());
    if (current_point == goal_point) { break; }
    std::pop_heap(heap.begin(), heap.end(), astar_heap_sort_<heap_node>);
    heap.pop_back();
    const dist_type current_distance = dist_map[current_point];
    for (const auto& edge : EDGES_) {
      const IndexType next_point{
          current_point.at(0) + std::get<0>(edge), current_point.at(1) + std::get<1>(edge)
      };
      if (!dist_map.in_range(next_point)) { continue; }
      dist_type cost = cost_map[next_point];
      dist_type next_distance = current_distance + cost;
      if (dist_map[next_point] <= next_distance) { continue; }
      dist_map[next_point] = next_distance;
      heap.emplace_back(heap_node{ next_distance + test_heuristic<dist_type>(next_point, goal_point), next_point });
      std::push_heap(heap.begin(), heap.end(), astar_heap_sort_<heap_node>);
    }
  }
}
} // namespace pathfinding
} // namespace tcod
#endif // __cplusplus
#endif // LIBTCOD_PATHFINDING_ASTAR_H_
