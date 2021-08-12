/* BSD 3-Clause License
 *
 * Copyright © 2008-2021, Jice and the libtcod contributors.
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
#ifndef LIBTCOD_MATRIX_H_
#define LIBTCOD_MATRIX_H_
#ifdef __cplusplus
#include <array>
#include <stdexcept>
#include <string>
#include <vector>

namespace tcod {
/*****************************************************************************
    @brief A template container for holding a multi-dimensional array of items.

    @tparam T The type of value contained by this matrix.
    @tparam Dimensions The number of dimensions of this matrix type.

    This class is a work-in-progress.
 */
template <typename T, size_t Dimensions>
class Matrix {
 public:
  using size_type = int;  // The int size of indexes.
  using shape_type = std::array<size_type, Dimensions>;  // The type used to measure the matrixes shape.
  using index_type = std::array<size_type, Dimensions>;  // The type used to index the container.
  using container_type = std::vector<T>;  // The underlying container type.
  using reference = typename container_type::reference;
  using const_reference = typename container_type::const_reference;
  Matrix() = default;
  explicit Matrix(const shape_type& shape) : shape_(shape), data_(get_size_from_shape(shape)) {}
  Matrix(const shape_type& shape, const T& fill_value) : shape_(shape), data_(get_size_from_shape(shape), fill_value) {}
  auto begin() noexcept { return data_.begin(); }
  auto begin() const noexcept { return data_.cbegin(); }
  auto end() noexcept { return data_.end(); }
  auto end() const noexcept { return data_.cend(); }

  reference operator[](const index_type& index) noexcept { return data_[get_index(index)]; }
  const_reference operator[](const index_type& index) const noexcept { return data_[get_index(index)]; }

  reference at(const index_type& index) { return data_.at(check_range(index)); }
  const_reference at(const index_type& index) const { return data_.at(check_range(index)); }

  const shape_type& get_shape() const { return shape_; }
  bool in_bounds(const index_type& index) const noexcept {
    for (size_t dimension = 0; dimension < Dimensions; ++dimension) {
      if (!(0 <= index.at(dimension) && index.at(dimension) < shape_.at(dimension))) {
        return false;
      }
    }
    return true;
  }

  container_type& get_container() noexcept { return data_; }
  const container_type& get_container() const noexcept { return data_; }

 private:
  static size_t get_size_from_shape(const shape_type& shape) noexcept {
    size_t size = 1;
    for (auto& it : shape) size *= it;
    return size;
  }
  size_t get_index(const index_type& index) const noexcept {
    size_t stride = 1;
    size_t data_index = 0;
    for (size_t dimension = 0; dimension < Dimensions; ++dimension) {
      data_index += stride * index.at(dimension);
      stride *= shape_.at(dimension);
    }
    return data_index;
  }
  size_t check_range(const index_type& index) const {
    if (!in_bounds(index)) {
      throw std::out_of_range(
          std::string("Out of bounds lookup ") + array_as_string(index) + " on matrix of shape " +
          array_as_string(shape_) + ".");
    }
    return get_index(index);
  }
  template <typename ArrayType>
  static std::string array_as_string(const ArrayType& arr) {
    std::string result{"{"};
    for (const auto& it : arr) {
      result += std::to_string(it);
      if (&it != &arr.back()) {
        result += ", ";
      }
    }
    result += "}";
    return result;
  }
  shape_type shape_;
  container_type data_;
};
}  // namespace tcod
#endif  // __cplusplus
#endif  // LIBTCOD_MATRIX_H_
