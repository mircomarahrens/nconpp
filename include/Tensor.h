#pragma once

#include <xtensor/xarray.hpp>
#include <xtensor/xexpression.hpp>
#include <xtensor/xlayout.hpp>
#include <xtensor/xrandom.hpp>
#include <xtensor-blas/xblas.hpp>
#include <xtensor-blas/xlapack.hpp>
#include <xtensor-blas/xlinalg.hpp>

// Wrapper for tensor operations used by nconpp based on xtensor.
namespace Tensor {
    using namespace xt::placeholders;

    // array
    template<typename T>
    using array_type = xt::xarray<T, xt::layout_type::dynamic>;

    // shape_type
    using shape_type = xt::dynamic_shape<size_t>;

    // general multi-dimensional object
    template<typename T>
    using tensor_type = xt::xexpression<T>;

    // reshape
    template<typename T>
    static inline auto reshape(const tensor_type<T> &M, shape_type shape) {
        auto &&dM = M.derived_cast();
        return dM.reshape(shape);
    }

    // prod
    template<typename T>
    static inline auto prod(const tensor_type<T> &M, std::size_t axis) {
        auto &&dM = M.derived_cast();
        return xt::prod(dM, axis);
    }

    // range
    template<class A, class B>
    static inline auto range(A start_val, B stop_val) {
        return xt::range(start_val, stop_val);
    }

    // shape
    template<typename T>
    static inline auto shape(const tensor_type<T> &M) {
        auto &&dM = M.derived_cast();
        return dM.shape();
    }

    // dimension
    template<typename T>
    static inline auto dimension(const tensor_type<T> &M) {
        auto &&dM = M.derived_cast();
        return dM.dimension();
    }

    // expand_dims
    template<typename T>
    static inline auto expand_dims(tensor_type<T> &M, std::size_t axis) {
        auto &&dM = M.derived_cast();
        return xt::expand_dims(dM, axis);
    }

    // tensordot
    template<typename T, typename O>
    static inline auto
    tensordot(const tensor_type<T> &xa, const tensor_type<O> &xb, const std::vector<std::size_t> &ax_a,
              const std::vector<std::size_t> &ax_b) -> decltype(xt::linalg::tensordot(xa, xb, ax_a, ax_b)) {
        return xt::linalg::tensordot(xa, xb, ax_a, ax_b);
    }

    // trace
    template<typename T>
    static inline auto
    trace(const tensor_type<T> &M, int offset = 0, std::size_t axis1 = 0, std::size_t axis2 = 1) {
        auto &&vM = xt::view_eval<T::static_layout>(M.derived_cast());
        auto d = xt::diagonal(vM, offset, std::size_t(axis1), std::size_t(axis2));
        std::size_t dim = d.dimension();
        if (dim == 1) {
            return xt::xarray<std::complex<double>>(xt::sum(d)());
        } else {
            return xt::xarray<std::complex<double>>(xt::sum(d, {dim - 1}));
        }
    }

    // transpose
    template<typename T>
    static inline auto transpose(const tensor_type<T> &M, const std::vector<int> &perm) {
        auto &&dM = M.derived_cast();
        return xt::transpose(dM, perm);
    }

    // svd
    template<typename T>
    static inline auto svd(const tensor_type<T> &M, bool full_matrices = true, bool compute_uv = true) {
        auto &&dM = M.derived_cast();
        return xt::linalg::svd(dM, full_matrices, compute_uv);
    }
};
