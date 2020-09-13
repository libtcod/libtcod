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
#ifndef LIBTCOD_PATHFINDING_ASTAR_H_
#define LIBTCOD_PATHFINDING_ASTAR_H_
#ifdef __cplusplus
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <tuple>
#include <vector>

#include "graph.h"
namespace tcod {
namespace pathfinding {
template <
    typename DistMatrix,
    typename Graph,
    typename IndexType = typename DistMatrix::index_type,
    typename NewEdgeFunc>
inline void astar(
    DistMatrix& dist_map, const Graph& graph, const IndexType& goal_point, const NewEdgeFunc& on_new_edge) {
  using dist_type = typename DistMatrix::value_type;
  const dist_type MAX_DIST = std::numeric_limits<dist_type>::max();
  using heap_node = std::tuple<dist_type, IndexType>;
  auto heap_compare = [](const heap_node& a, const heap_node& b) { return std::get<0>(a) > std::get<0>(b); };
  std::vector<heap_node> heap;
  for (ptrdiff_t i = 0; i < dist_map.get_shape().at(0); ++i) {
    for (ptrdiff_t j = 0; j < dist_map.get_shape().at(1); ++j) {
      if (dist_map[{i, j}] >= MAX_DIST) {
        continue;
      }
      heap.emplace_back(heap_node{dist_map[{i, j}] + graph.heuristic({i, j}, goal_point), {i, j}});
    }
  }
  std::make_heap(heap.begin(), heap.end(), heap_compare);
  while (heap.size()) {
    const IndexType current_point = std::get<1>(heap.front());
    if (current_point == goal_point) {
      break;
    }
    std::pop_heap(heap.begin(), heap.end(), heap_compare);
    heap.pop_back();
    const dist_type current_distance = dist_map[current_point];
    auto add_edge = [&](const IndexType& next, const dist_type& cost) {
      dist_type next_distance = current_distance + cost;
      if (dist_map[next] <= next_distance) {
        return;
      }
      dist_map[next] = next_distance;
      on_new_edge(current_point, next);
      heap.emplace_back(heap_node{next_distance + graph.heuristic(next, goal_point), next});
      std::push_heap(heap.begin(), heap.end(), heap_compare);
    };
    graph.with_edges(add_edge, current_point);
  }
}
template <typename DistMatrix, typename Graph, typename IndexType = typename DistMatrix::index_type>
inline void astar(DistMatrix& dist_map, const Graph& graph, const IndexType& goal_point) {
  auto on_new_edge = [](auto, auto) {};
  astar(dist_map, graph, goal_point, on_new_edge);
}
template <typename DistMatrix, typename CostMatrix, typename IndexType = typename DistMatrix::index_type>
inline void astar2d(
    DistMatrix& dist_map, const CostMatrix& cost_map, const IndexType& goal_point, int cardinal = 1, int diagonal = 1) {
  auto graph = SimpleGraph2D<CostMatrix>(cost_map, cardinal, diagonal);
  astar(dist_map, graph, goal_point);
}
}  // namespace pathfinding
}  // namespace tcod
#endif  // __cplusplus
#endif  // LIBTCOD_PATHFINDING_ASTAR_H_
