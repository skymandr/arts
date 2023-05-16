#pragma once

#include "matpack_lazy.h"
#include "rtepack_concepts.h"

#include <type_traits>

namespace rtepack {
using namespace matpack::lazy;

template <typename T>
concept lazy_stokvec =
    constexpr_vec_data_like<T> and std::remove_cvref_t<T>::size() == 4;

struct stokvec final : vec4 {
  constexpr stokvec(Numeric a = 0.0, Numeric b = 0.0, Numeric c = 0.0,
                    Numeric d = 0.0)
      : vec4{a, b, c, d} {}

  constexpr stokvec(lazy_stokvec auto &&a)
    requires(std::is_rvalue_reference_v<decltype(a)>)
      : vec4(std::move(a)) {}
};

//! Addition of two stokvec vectors
constexpr auto operator+(const stokvec &a, const stokvec &b) {
  return constexpr_vec_data{a} + constexpr_vec_data{b};
}

//! Addition of a stokvec vector and a lazy one
constexpr auto operator+(const stokvec &a, const lazy_stokvec auto &b) {
  return constexpr_vec_data{a} + b;
}

//! Addition of a stokvec vector and a lazy one
constexpr auto operator+(const lazy_stokvec auto &a, const stokvec &b) {
  return a + constexpr_vec_data{b};
}

//! Subtraction between two stokvec vectors
constexpr auto operator-(const stokvec &a, const stokvec &b) {
  return constexpr_vec_data{a} - constexpr_vec_data{b};
}

//! Subtraction between a stokvec vector and a lazy one
constexpr auto operator-(const stokvec &a, const lazy_stokvec auto &b) {
  return constexpr_vec_data{a} - b;
}

//! Subtraction between a stokvec vector and a lazy one
constexpr auto operator-(const lazy_stokvec auto &a, const stokvec &b) {
  return a - constexpr_vec_data{b};
}

//! Scaling a stokvec vector
constexpr auto operator*(const Numeric &a, const stokvec &b) {
  return vscl{a, constexpr_vec_data{b}};
}

//! Scaling a stokvec vector
constexpr auto operator*(const stokvec &a, const Numeric &b) {
  return vscl{b, constexpr_vec_data{a}};
}

//! Take the average of two stokvec vectors
constexpr auto avg(const stokvec &a, const stokvec &b) {
  return 0.5 * a + 0.5 * b;
}

//! Take the average of a stokvec vector and a non-stokvec vector
constexpr auto avg(const stokvec &a, const lazy_stokvec auto &b) {
  return 0.5 * a + 0.5 * b;
}

//! Take the average of a stokvec vector and a non-stokvec vector
constexpr auto avg(const lazy_stokvec auto &a, const stokvec &b) {
  return 0.5 * a + 0.5 * b;
}

using stokvec_vector = matpack::matpack_data<stokvec, 1>;
using stokvec_vector_view = matpack::matpack_view<stokvec, 1, false, false>;
using stokvec_vector_const_view = matpack::matpack_view<stokvec, 1, true, false>;

using stokvec_matrix = matpack::matpack_data<stokvec, 2>;
using stokvec_matrix_view = matpack::matpack_view<stokvec, 2, false, false>;
using stokvec_matrix_const_view = matpack::matpack_view<stokvec, 2, true, false>;

using stokvec_tensor3 = matpack::matpack_data<stokvec, 3>;
using stokvec_tensor3_view = matpack::matpack_view<stokvec, 3, false, false>;
using stokvec_tensor3_const_view = matpack::matpack_view<stokvec, 3, true, false>;
} // namespace rtepack
