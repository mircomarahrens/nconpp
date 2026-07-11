// Copyright (c) 2026 Mirco Marahrens

#ifndef CPP_SRC_LINALG_HPP_
#define CPP_SRC_LINALG_HPP_

#include <algorithm>
#include <complex>
#include <cstddef>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <vector>

#include <xtensor/xmath.hpp>
#include <xtensor/xview.hpp>

#include "Tensor.hpp"

namespace npp::linalg {
namespace detail {
template <typename T> struct is_std_complex : std::false_type {};

template <typename T> struct is_std_complex<std::complex<T>> : std::true_type {};

template <class Shape>
static inline auto make_mode_first_permutation(const Shape &shape, std::size_t mode) {
    if (mode >= shape.size()) {
        throw std::out_of_range("Requested tensor mode is out of range.");
    }

    auto permutation = std::vector<std::size_t>{mode};
    permutation.reserve(shape.size());

    for (std::size_t axis = 0; axis < shape.size(); ++axis) {
        if (axis != mode) {
            permutation.push_back(axis);
        }
    }

    return permutation;
}

template <typename T>
static inline auto unfold(const npp::expression_type<T> &tensor, std::size_t mode) {
    using value_type = typename std::decay_t<decltype(tensor.derived_cast())>::value_type;

    const auto tensor_shape = shape(tensor);
    const auto permutation = make_mode_first_permutation(tensor_shape, mode);
    npp::tensor_type<value_type> unfolded = xt::transpose(tensor.derived_cast(), permutation);

    const auto rows = tensor_shape[mode];
    const auto cols = prod(tensor_shape) / rows;
    unfolded.reshape({rows, cols});

    return unfolded;
}

template <typename T, typename O>
static inline auto mode_product(const npp::expression_type<T> &tensor,
                                const npp::expression_type<O> &matrix, std::size_t mode) {
    using value_type = typename std::decay_t<decltype(tensor.derived_cast())>::value_type;

    npp::tensor_type<value_type> contracted = xt::linalg::tensordot(matrix, tensor, {1}, {mode});

    auto permutation = std::vector<std::size_t>{};
    permutation.reserve(dimension(tensor));
    for (std::size_t axis = 0; axis < mode; ++axis) {
        permutation.push_back(axis + 1);
    }
    permutation.push_back(0);
    for (std::size_t axis = mode + 1; axis < dimension(tensor); ++axis) {
        permutation.push_back(axis);
    }

    return npp::tensor_type<value_type>(xt::transpose(contracted, permutation));
}

template <typename T> static inline auto adjoint(const npp::expression_type<T> &matrix) {
    using value_type = typename std::decay_t<decltype(matrix.derived_cast())>::value_type;

    if constexpr (is_std_complex<value_type>::value) {
        return npp::tensor_type<value_type>(xt::transpose(xt::conj(matrix.derived_cast())));
    } else {
        return npp::tensor_type<value_type>(xt::transpose(matrix.derived_cast()));
    }
}
} // namespace detail

template <typename T>
static inline auto hosvd(const npp::expression_type<T> &M, std::size_t rank = 0) {
    const auto tensor_shape = shape(M);
    const auto order = tensor_shape.size();

    if (order == 0) {
        throw std::invalid_argument("HOSVD requires a tensor with at least one dimension.");
    }

    using value_type = typename std::decay_t<decltype(M.derived_cast())>::value_type;

    auto factors = std::vector<npp::tensor_type<value_type>>{};
    factors.reserve(order);

    for (std::size_t mode = 0; mode < order; ++mode) {
        auto unfolding = detail::unfold(M, mode);
        auto svd_result = npp::linalg::svd(unfolding, false, true);
        auto left_singular_vectors = std::get<0>(svd_result);

        const auto available_rank = left_singular_vectors.shape()[1];
        const auto target_rank = rank == 0 ? available_rank : std::min(rank, available_rank);

        factors.push_back(npp::tensor_type<value_type>(
            xt::view(left_singular_vectors, xt::all(), xt::range(0, target_rank))));
    }

    auto core = npp::tensor_type<value_type>(M.derived_cast());
    for (std::size_t mode = 0; mode < order; ++mode) {
        core = detail::mode_product(core, detail::adjoint(factors[mode]), mode);
    }

    return std::make_tuple(factors, core);
}
} // namespace npp::linalg

#endif // CPP_SRC_LINALG_HPP_