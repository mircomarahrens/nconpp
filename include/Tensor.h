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

    template<typename... Args>
    explicit Tensor(Args... args) {
        std::initializer_list<int> list{args...};
        mData = xt::random::rand<double>(list);
    };

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

    void transpose(const std::vector<int> &perm) {
        xt::transpose(mData, perm);
    };

    const array_type &getData() const {
        return mData;
    };

private:
    array_type mData;
};
