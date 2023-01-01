/* BSD 3-Clause License
 *
 * Copyright © 2008-2023, Jice and the libtcod contributors.
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
#ifndef LIBTCOD_MATRIX_HPP_
#define LIBTCOD_MATRIX_HPP_
#include <array>
#include <stdexcept>
#include <string>
#include <vector>

namespace tcod {
namespace internal {
/// Return a small array has a human readable string.
template <typename ArrayType>
[[nodiscard]] static std::string array_as_string(const ArrayType& arr) {
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
}  // namespace internal
/***************************************************************************
    @brief A view into a strided multi-dimensional array.

    @tparam T The type viewed by this object.
    @tparam Dimensions The number of dimensions of the view.

    This class is a work-in-progress.
 */
template <typename T, size_t Dimensions>
class MatrixView {
 public:
  using size_type = int;  // The int size of indexes.
  using shape_type = std::array<size_type, Dimensions>;  // The type used to store the matrixes shape.
  using stride_type = std::array<size_type, Dimensions>;  // The type used to store strides.
  using index_type = std::array<size_type, Dimensions>;  // The type used to index the container.
  using reference = T&;
  using const_reference = const T&;
  /// Default constructor.
  constexpr MatrixView() = default;
  /// Create a new multi-dimensional view.
  constexpr MatrixView(const shape_type& shape_xy, const stride_type& strides_xy, T* data) noexcept
      : shape_xy_{shape_xy}, strides_xy_{strides_xy}, data_{reinterpret_cast<data_ptr>(data)} {};

  /// Get the item at index.
  [[nodiscard]] constexpr reference operator[](const index_type& index) noexcept {
    return *reinterpret_cast<T*>(data_ + get_offset(index));
  }
  /// Get the const item at index.
  [[nodiscard]] constexpr const_reference operator[](const index_type& index) const noexcept {
    return *reinterpret_cast<const T*>(data_ + get_offset(index));
  }
  /// Get the item at index, checking bounds.
  [[nodiscard]] constexpr reference at(const index_type& index) {
    return *reinterpret_cast<T*>(data_ + check_range(index));
  }
  /// Get the const item at index, checking bounds.
  [[nodiscard]] constexpr const_reference at(const index_type& index) const {
    return *reinterpret_cast<const T*>(data_ + check_range(index));
  }

  /// Return true if index is within the bounds of this matrix.
  [[nodiscard]] constexpr bool in_bounds(const index_type& index) const noexcept {
    for (size_t dimension = 0; dimension < Dimensions; ++dimension) {
      if (!(0 <= index.at(dimension) && index.at(dimension) < shape_xy_.at(dimension))) return false;
    }
    return true;
  }

 private:
  // `unsigned char*` pointer which has the same const as `T`.
  using data_ptr = typename std::conditional<std::is_const<T>::value, const unsigned char*, unsigned char*>::type;
  /// Return the byte offset of `data_` for the index for this view.
  [[nodiscard]] constexpr size_t get_offset(const index_type& index) const noexcept {
    size_t data_index = 0;
    for (size_t dimension{0}; dimension < Dimensions; ++dimension) {
      data_index += strides_xy_.at(dimension) * index.at(dimension);
    }
    return data_index;
  }
  /// Check for out-of-bounds, then return the data offset for `index`.
  constexpr size_t check_range(const index_type& index) const {
    using internal::array_as_string;
    if (!in_bounds(index)) {
      throw std::out_of_range(
          std::string("Out of bounds lookup ") + array_as_string(index) + " on matrix of shape " +
          array_as_string(shape_xy_) + ".");
    }
    return get_offset(index);
  }
  shape_type shape_xy_;  // The shape of this view.
  stride_type strides_xy_;  // The strides of this view in bytes.
  data_ptr data_;  // A pointer to the viewed memory.
};
/*****************************************************************************
    @brief A template container for holding a multi-dimensional array of items.

    @tparam T The type of value contained by this matrix.
    @tparam Dimensions The number of dimensions of this matrix type.
    @tparam Container The `std::vector`-like container used for this matrix.

    This class is a work-in-progress.
 */
template <typename T, size_t Dimensions, typename Container = std::vector<T>>
class Matrix {
 public:
  using size_type = int;  // The int size of indexes.
  using shape_type = std::array<size_type, Dimensions>;  // The type used to measure the matrixes shape.
  using index_type = std::array<size_type, Dimensions>;  // The type used to index the container.
  using reference = typename Container::reference;
  using const_reference = typename Container::const_reference;
  /// Default constructor.
  constexpr Matrix() = default;

  /// Create a matrix of the given shape.
  constexpr explicit Matrix(const shape_type& shape) : shape_{shape}, data_(get_size_from_shape(shape)) {}

  /// Create a matrix of the given shape filled with a default value.
  constexpr Matrix(const shape_type& shape, const T& fill_value)
      : shape_{shape}, data_(get_size_from_shape(shape), fill_value) {}

  /// Return the iterator beginning.
  [[nodiscard]] constexpr auto begin() noexcept { return data_.begin(); }
  /// Return the iterator beginning.
  [[nodiscard]] constexpr auto begin() const noexcept { return data_.cbegin(); }

  /// Return the iterator end.
  [[nodiscard]] constexpr auto end() noexcept { return data_.end(); }
  /// Return the iterator end.
  [[nodiscard]] constexpr auto end() const noexcept { return data_.cend(); }

  /// Get the item at index.
  [[nodiscard]] constexpr reference operator[](const index_type& index) noexcept { return data_[get_index(index)]; }

  /// Get the const item at index.
  [[nodiscard]] constexpr const_reference operator[](const index_type& index) const noexcept {
    return data_[get_index(index)];
  }
  /// Get the item at index, checking bounds.
  [[nodiscard]] constexpr reference at(const index_type& index) { return data_.at(check_range(index)); }

  /// Get the const item at index, checking bounds.
  [[nodiscard]] constexpr const_reference at(const index_type& index) const { return data_.at(check_range(index)); }

  /// Return the shape of this matrix.
  [[nodiscard]] constexpr const shape_type& get_shape() const noexcept { return shape_; }

  /// Return true if index is within the bounds of this matrix.
  [[nodiscard]] constexpr bool in_bounds(const index_type& index) const noexcept {
    for (size_t dimension = 0; dimension < Dimensions; ++dimension) {
      if (!(0 <= index.at(dimension) && index.at(dimension) < shape_.at(dimension))) return false;
    }
    return true;
  }

  /// Implicit cast to a view of this matrix.
  [[nodiscard]] constexpr operator MatrixView<T, Dimensions>() noexcept {
    return {get_shape(), get_strides(), data_.data()};
  }
  /// Implicit cast to a const view of this matrix.
  [[nodiscard]] constexpr operator MatrixView<const T, Dimensions>() const noexcept {
    return {get_shape(), get_strides(), data_.data()};
  }

  /// Get the flat container for this matrix.
  [[nodiscard]] constexpr Container& get_container() noexcept { return data_; }

  /// Get the const flat container for this matrix.
  [[nodiscard]] constexpr const Container& get_container() const noexcept { return data_; }

  template <class Archive>
  void serialize(Archive& archive) {
    archive(shape_, data_);
  }

 private:
  /// Return the total number of items in a given shape.
  [[nodiscard]] static constexpr size_t get_size_from_shape(const shape_type& shape) noexcept {
    size_t size = 1;
    for (auto& it : shape) size *= it;
    return size;
  }
  /// Return the 1D index to the flat container of this matrix.
  [[nodiscard]] constexpr size_t get_index(const index_type& index) const noexcept {
    size_t stride = 1;
    size_t data_index = 0;
    for (size_t dimension = 0; dimension < Dimensions; ++dimension) {
      data_index += stride * index.at(dimension);
      stride *= shape_.at(dimension);
    }
    return data_index;
  }
  /// Return the 1D index to the flat container.  Throw if out-of-bounds.
  constexpr size_t check_range(const index_type& index) const {
    using internal::array_as_string;
    if (!in_bounds(index)) {
      throw std::out_of_range(
          std::string("Out of bounds lookup ") + array_as_string(index) + " on matrix of shape " +
          array_as_string(shape_) + ".");
    }
    return get_index(index);
  }
  /// Return the byte-strides of this matrix.
  [[nodiscard]] constexpr index_type get_strides() const noexcept {
    index_type strides{};
    int stride = static_cast<int>(sizeof(T));
    for (size_t dimension = 0; dimension < Dimensions; ++dimension) {
      strides.at(dimension) = stride;
      stride *= shape_.at(dimension);
    }
    return strides;
  }
  shape_type shape_;
  Container data_;
};
}  // namespace tcod
#endif  // LIBTCOD_MATRIX_HPP_
