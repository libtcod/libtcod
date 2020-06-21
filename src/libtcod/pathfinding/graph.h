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
#ifndef LIBTCOD_PATHFINDING_GRAPH_H_
#define LIBTCOD_PATHFINDING_GRAPH_H_
#ifdef __cplusplus
#include <algorithm>
#include <array>
#include <cmath>
#include <memory>
#include <tuple>
#include <utility>
#endif  // __cplusplus
#ifdef __cplusplus
namespace tcod {
namespace pathfinding {
/**
 *  Private hard-coded edge positions.
 *
 *  {i, j}
 */
static constexpr std::array<std::tuple<int, int>, 8> EDGES_{{
    {-1, 0},
    {1, 0},
    {0, -1},
    {0, 1},
    {-1, -1},
    {1, -1},
    {-1, 1},
    {1, 1},
}};

template <typename CostMatrix>
class SimpleGraph2D {
 public:
  SimpleGraph2D(std::shared_ptr<const CostMatrix> cost, int cardinal = 1, int diagonal = 1)
      : cost_{std::move(cost)}, edges_{make_edges(cardinal, diagonal)}, cardinal_{cardinal}, diagonal_{diagonal} {}
  SimpleGraph2D(const CostMatrix& cost, int cardinal = 1, int diagonal = 1)
      : SimpleGraph2D{std::make_shared<const CostMatrix>(cost), cardinal, diagonal} {}
  SimpleGraph2D(CostMatrix&& cost, int cardinal = 1, int diagonal = 1)
      : SimpleGraph2D{std::make_shared<const CostMatrix>(cost), cardinal, diagonal} {}
  /**
   *  Calls `edge_func(index, cost) -> void` with the edges from `index`.
   *
   *  Within the lambda parameters `index` is the destination node, and `cost`
   *  is the cost between nodes.
   */
  template <typename F, typename index_type>
  void with_edges(const F& edge_func, const index_type& index) const {
    for (const auto& edge : edges_) {
      if (std::get<2>(edge) <= 0) {
        continue;
      }
      index_type node{index.at(0) + std::get<0>(edge), index.at(1) + std::get<1>(edge)};
      if (!cost_->in_range(node)) {
        continue;
      }
      int cost = std::get<2>(edge) * (*cost_)[node];
      if (cost <= 0) {
        continue;
      }
      edge_func(node, cost);
    }
  }
  /**
   *  Return the heuristic between two points.
   */
  template <typename index_type>
  int heuristic(const index_type& a, const index_type& b) const {
    index_type diff{std::abs(a.at(0) - b.at(0)), std::abs(a.at(1) - b.at(1))};
    int diagonal = std::min(diff.at(0), diff.at(1));
    int cardinal = std::max(diff.at(0), diff.at(1)) - diagonal;
    return cardinal * cardinal_ + diagonal * diagonal_;
  }

 private:
  using edge_array = std::array<std::tuple<int, int, int>, 8>;
  std::shared_ptr<const CostMatrix> cost_;
  edge_array edges_;
  int cardinal_;
  int diagonal_;
  static edge_array make_edges(int cardinal, int diagonal) {
    return edge_array{{
        {-1, 0, cardinal},
        {1, 0, cardinal},
        {0, -1, cardinal},
        {0, 1, cardinal},
        {-1, -1, diagonal},
        {1, -1, diagonal},
        {-1, 1, diagonal},
        {1, 1, diagonal},
    }};
  }
};

}  // namespace pathfinding
}  // namespace tcod
#endif  // __cplusplus
#endif  // LIBTCOD_PATHFINDING_GRAPH_H_
