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
    using at = xt::xarray<T, xt::layout_type::dynamic>;

    // dynamic multi-dimensional shape object
    using st = xt::dynamic_shape<size_t>;

    explicit Tensor(const at &data) : mData(data) {};

    explicit Tensor(const st &shape) : mData(xt::random::rand<T>(shape)) {};

    explicit Tensor() = default;

    // shape
    st shape() {
        return mData.shape();
    };

    // reshape
    void reshape(const st &shape) {
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

    void transpose(const std::vector<int> &perm) {
        xt::transpose(mData, perm);
    };

    const at &getData() const {
        return mData;
    };

private:
    at &mData;
};
