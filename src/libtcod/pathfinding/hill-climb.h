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
#ifndef LIBTCOD_PATHFINDING_HILL_CLIMB_H_
#define LIBTCOD_PATHFINDING_HILL_CLIMB_H_
#ifdef __cplusplus
#include <functional>
#include <vector>
namespace tcod {
namespace pathfinding {
template <typename DistMatrix, typename Graph, typename IndexType = typename DistMatrix::index_type, typename Compare>
inline auto simple_hillclimb(
    const DistMatrix& cost_map,
    const Graph& graph,
    const IndexType& start,
    const Compare& compare)
-> std::vector<IndexType>
{
  std::vector<IndexType> results;
  IndexType current = start;
  IndexType next = start;
  auto find_next = [&](const IndexType& checked_index, auto) {
    if (compare(cost_map[checked_index], cost_map[next])) {
      next = checked_index;
    }
  };
  do {
    current = next;
    results.emplace_back(current);
    graph.with_edges(find_next, current);
  } while (cost_map[current] != cost_map[next]);
  return results;
}
template <typename DistMatrix, typename Graph, typename IndexType = typename DistMatrix::index_type>
inline auto simple_hillclimb(
    const DistMatrix& cost_map,
    const Graph& graph,
    const IndexType& start)
-> std::vector<IndexType>
{
  return simple_hillclimb(cost_map, graph, start,
                          std::less<typename DistMatrix::value_type>());
}
} // namespace pathfinding
} // namespace tcod
#endif // __cplusplus
#endif // LIBTCOD_PATHFINDING_HILL_CLIMB_H_
