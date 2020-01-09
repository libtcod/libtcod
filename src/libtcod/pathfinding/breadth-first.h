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
#ifndef LIBTCOD_PATHFINDING_BREADTH_FIRST_H_
#define LIBTCOD_PATHFINDING_BREADTH_FIRST_H_
#ifdef __cplusplus
#include <array>
#include <initializer_list>
#include <limits>
#include <queue>
#endif // __cplusplus
#include "graph.h"

#ifdef __cplusplus
namespace tcod {
namespace pathfinding {
template <typename DistMatrix, typename Graph>
inline void breadth_first(
    DistMatrix& dist_map,
    const Graph& graph,
    const std::initializer_list<std::array<ptrdiff_t, 2>>& start)
{
  using cost_type = typename DistMatrix::value_type;
  const cost_type MAX_COST = std::numeric_limits<cost_type>::max();
  using index_type = std::array<ptrdiff_t, 2>;
  for (const auto& start_point : start) {
    dist_map.at(start_point) = 0;
  }
  std::queue<index_type> queue(start);
  while (queue.size()) {
    const index_type current_point = queue.front();
    queue.pop();
    auto add_edge = [&](const index_type& next_point, auto) {
      if (dist_map[next_point] < MAX_COST) { return; }
      dist_map[next_point] = dist_map[current_point] + 1;
      queue.push(next_point);
    };
    graph.with_edges(add_edge, current_point);
  }
}
template <typename DistMatrix, typename CostMatrix>
inline void breadth_first2d(
    DistMatrix& dist_map,
    const CostMatrix& cost_map,
    const std::initializer_list<std::array<ptrdiff_t, 2>>& start,
    int cardinal=1,
    int diagonal=1)
{
  auto graph = SimpleGraph2D<CostMatrix>(cost_map, cardinal, diagonal);
  breadth_first(dist_map, graph, start);
}
} // namespace tcod
} // namespace pathfinding
#endif // __cplusplus


#endif // LIBTCOD_PATHFINDING_BREADTH_FIRST_H_
