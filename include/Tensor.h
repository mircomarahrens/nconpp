#pragma once

#include <xtensor/xarray.hpp>
#include <xtensor/xexpression.hpp>
#include <xtensor/xlayout.hpp>
#include <xtensor/xrandom.hpp>
#include <xtensor-blas/xblas.hpp>
#include <xtensor-blas/xlapack.hpp>
#include <xtensor-blas/xlinalg.hpp>

// Wrapper for tensor containers used by nconpp based on xtensor.
template<typename T>
class Tensor {
public:
    // dynamic multi-dimensional array object
    using array_type = xt::xarray<T, xt::layout_type::dynamic>;

    // dynamic multi-dimensional shape object
    using shape_type = xt::dynamic_shape<size_t>;

    explicit Tensor(const array_type &data) : mData(data) {};

    explicit Tensor() = default;

    // shape
    shape_type shape() {
        return mData.shape();
    };

    // reshape
    void reshape(const shape_type &shape) {
        mData.reshape(shape);
    };

    // dimension
    auto dimension() {
        return mData.dimension();
    };

    // prod
    void prod(std::size_t axis) {
        xt::prod(mData, axis);
    };

    void prod(const std::vector<std::size_t> &axis) {
        xt::prod(mData, axis);
    };

    // expand_dims
    void expand_dims(std::size_t axis) {
        xt::expand_dims(mData, axis);
    };

    void transpose(const std::vector<int>& perm) {
        xt::transpose(mData, perm);
    };

    template<typename D>
    void random(const std::vector<int>& shape) {
        mData = xt::random::rand<D>(shape);
    }

    const array_type &getData() const {
        return mData;
    };

private:
    const array_type &mData;
};

// Wrapper for tensor operations used by nconpp based on xtensor.
namespace TensorOperations {
    using namespace xt::placeholders;

    // general multi-dimensional object
    template<typename T>
    using expression_type = xt::xexpression<T>;

    template<typename T, typename O>
    static inline auto
    tensordot(const Tensor<T> &tensorA, const Tensor<O> &tensorB, const std::vector<std::size_t> &axisA,
              const std::vector<std::size_t> &axisB) -> decltype(xt::linalg::tensordot(tensorA.getData(),
                                                                                       tensorB.getData(), axisA,
                                                                                       axisB)) {
        return xt::linalg::tensordot(tensorA.getData(), tensorB.getData(), axisA, axisB);
    }

    // trace
    template<typename T>
    static inline auto
    trace(const expression_type<T> &M, int offset = 0, std::size_t axis1 = 0, std::size_t axis2 = 1) {
        auto &&vM = xt::view_eval<T::static_layout>(M.derived_cast());
        auto d = xt::diagonal(vM, offset, std::size_t(axis1), std::size_t(axis2));
        std::size_t dim = d.dimension();
        if (dim == 1) {
            return xt::xarray<std::complex<double>>(xt::sum(d)());
        } else {
            return xt::xarray<std::complex<double>>(xt::sum(d, {dim - 1}));
        }
    }

    // svd
    template<typename T>
    static inline auto svd(const expression_type<T> &M, bool full_matrices = true, bool compute_uv = true) {
        auto &&dM = M.derived_cast();
        return xt::linalg::svd(dM, full_matrices, compute_uv);
    }
};
