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
#ifndef LIBTCOD_UTILITY_MATRIX_H_
#define LIBTCOD_UTILITY_MATRIX_H_
#ifdef __cplusplus
#include <algorithm>
#include <array>
#include <cstdbool>
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#endif // __cplusplus
#ifdef __cplusplus
namespace tcod {
template <typename T, size_t D>
class MatrixView {
 public:
  using value_type = T;
  using size_type = ptrdiff_t;
  using shape_type = std::array<size_type, D>;
  using strides_type = std::array<size_type, D>;
  MatrixView() = default;
  MatrixView(T* ptr, const shape_type& shape, const strides_type& strides)
  : data_{reinterpret_cast<char*>(ptr)}, shape_{shape}, strides_{strides}
  {}
  MatrixView(T* ptr, const shape_type& shape)
  : MatrixView(ptr, shape, get_contiguous_strides(shape))
  {}
  T& operator[](shape_type index) noexcept
  {
    return *get_data_at(index);
  }
  const T& operator[](shape_type index) const noexcept
  {
    return *get_data_at(index);
  }
  T& at(shape_type index)
  {
    range_check(index);
    return (*this)[index];
  }
  const T& at(shape_type index) const
  {
    range_check(index);
    return (*this)[index];
  }
  shape_type get_shape() const
  {
    return shape_;
  }
 private:
  T* get_data_at(shape_type index)
  {
    auto ptr = data_;
    for (size_t i = 0; i < shape_.size(); ++i) {
      ptr += index.at(i) * strides_.at(i);
    }
    return reinterpret_cast<T*>(ptr);
  }
  MatrixView<T, D-1> get_submatrix(size_type n)
  {
    return {
        data_ + n * strides_.at(0),
        pop_array(shape_),
        pop_array(strides_)
    };
  }
  bool in_bounds(size_type n) const noexcept
  {
    return 0 <= n && n < shape_.at(0);
  }
  bool in_bounds(shape_type index) const noexcept
  {
    for (size_t i = 0; i < shape_.size(); ++i) {
      if (index.at(i) < 0 || index.at(i) >= shape_.at(i)) {
        return false;
      }
    }
    return true;
  }
  void range_check(size_type n) const
  {
    if (in_bounds(n)) { return; }
    throw std::out_of_range(
        std::string("Out of bounds lookup {")
        + std::to_string(n)
        + "} on matrix of shape "
        + array_as_string(shape_)
        + ".");
  }
  void range_check(shape_type index) const
  {
    if (in_bounds(index)) { return; }
    throw std::out_of_range(
        std::string("Out of bounds lookup ")
        + array_as_string(index)
        + " on matrix of shape "
        + array_as_string(shape_)
        + ".");
  }
  template <typename ArrayType>
  static std::string array_as_string(const ArrayType& arr)
  {
    std::string result{"{ "};
    for (const auto& it : arr) {
      result += std::to_string(it);
      if (it != arr.back()) { result += ", "; }
    }
    result += " }";
    return result;
  }
  auto pop_array(const std::array<size_type, D>& array) const noexcept
  -> std::array<size_type, D-1>
  {
    std::array<size_type, D-1> new_array;
    std::copy(array[1], array.end(), new_array.begin());
    return new_array;
  }
  static strides_type get_contiguous_strides(shape_type shape)
  {
    strides_type strides;
    size_type stride = static_cast<size_type>(sizeof(T));
    for (size_type i = strides.size() - 1; i >= 0; --i) {
      strides.at(i) = stride;
      stride *= shape.at(i);
    }
    return strides;
  }
  char* data_{nullptr};
  shape_type shape_;
  strides_type strides_;
};
template <typename T, size_t D>
class Matrix {
 public:
  using view_type = MatrixView<T, D>;
  using value_type = typename view_type::value_type;
  using size_type = typename view_type::size_type;
  using shape_type = typename view_type::shape_type;
  using strides_type = typename view_type::strides_type;
  Matrix() = default;
  Matrix(const shape_type& shape)
  : data_(get_size_from_shape(shape)), view_(data_.data(), shape)
  {}
  Matrix(const shape_type& shape, const value_type& fill)
  : data_(get_size_from_shape(shape), fill), view_(data_.data(), shape)
  {}
  auto begin() noexcept
  {
    return data_.begin();
  }
  auto begin() const noexcept
  {
    return data_.cbegin();
  }
  auto end() noexcept
  {
    return data_.end();
  }
  auto end() const noexcept
  {
    return data_.cend();
  }
  value_type& operator[](shape_type index) noexcept
  {
    return view_[index];
  }
  const value_type& operator[](shape_type index) const noexcept
  {
    return view_[index];
  }
  value_type& at(shape_type index)
  {
    return view_.at(index);
  }
  const value_type& at(shape_type index) const
  {
    return view_.at(index);
  }
  shape_type get_shape() const
  {
    return view_.get_shape();
  }
 private:
  static size_t get_size_from_shape(const shape_type& shape) noexcept
  {
    size_t size = 1;
    for (size_t i = 0; i < shape.size(); ++i) {
      size *= shape.at(i);
    }
    return size;
  }
  std::vector<T> data_;
  view_type view_;
};
} // namespace tcod
#endif // __cplusplus
#endif // LIBTCOD_UTILITY_MATRIX_H_
