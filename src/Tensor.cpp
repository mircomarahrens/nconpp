#include "Tensor.h"

#include <algorithm>
#include <complex>
#include <random>
//#include <cstdarg>
//#include <iostream>
//#include <cassert>

template<typename T>
Tensor<T>::Tensor(std::vector<T> data, std::vector<int> shape) : mData(std::move(data)), mShape(std::move(shape)) {}

template<typename T>
Tensor<T>::Tensor(std::vector<int> shape) : mShape(std::move(shape)) {}

template<typename T>
const std::vector<int> &Tensor<T>::shape() {
    return mShape;
}

template<typename T>
void Tensor<T>::reshape(const std::vector<int> &shape) {
    auto dim1 = std::accumulate(begin(shape), end(shape), 1, std::multiplies<>());
    auto dim2 = std::accumulate(begin(mShape), end(mShape), 1, std::multiplies<>());
    if (dim1 == dim2)
        mShape = shape;
    else
        throw std::logic_error("Total number of elements in new shape does not match old shape.");
}

template<typename T>
auto Tensor<T>::dimension() {
    return mShape.size();
};

// Return the product of array elements over a given axis.
template<typename T>
T Tensor<T>::prod(int axis) {
    // TODO think about mapping multiple dimension to one dimension
    return 0;
}

// Return the product of array elements over a given axis.
template<typename T>
std::vector<T> Tensor<T>::prod(std::vector<int> axis) {
    std::vector<T> res(axis.size());
    std::fill(res.begin(), res.end(), 1);
    return res;
}

// Expand the shape of an array.
// Insert a new axis that will appear at the axis position in the expanded array shape.
template<typename T>
void Tensor<T>::expand_dims(std::size_t axis) {
//    xt::expand_dims(mData, axis);
}

// Reverse or permute the axes of an array; returns the modified array.
// For an array a with two axes, transpose(a) gives the matrix transpose.
template<typename T>
void Tensor<T>::transpose(const std::vector<int> &perm) {
//    xt::transpose(mData, perm);
}

template<typename T>
const auto &Tensor<T>::getData() {
    return mData;
}

// Fill the data array with randomized data.
template<typename T>
void Tensor<T>::randomize(double lower, double upper) {
    // First create an instance of an engine.
    std::random_device rnd_device;

    // Specify the engine and distribution.
    std::mt19937 mersenne_engine{rnd_device()};  // Generates random doubles
    std::uniform_real_distribution<double> dist(lower, upper);

    int dim = std::accumulate(std::begin(mShape), std::end(mShape), 1, std::multiplies<>());
    mData = std::vector<T>(dim);

    if (std::is_same<T, std::complex<double>>::value)
        std::generate(std::begin(mData), std::end(mData), [&dist, &mersenne_engine]() {
            return std::complex<double>(dist(mersenne_engine), dist(mersenne_engine));
        });
    else
        std::generate(std::begin(mData), std::end(mData), [&dist, &mersenne_engine]() {
            return dist(mersenne_engine);
        });
}

//int floor_div(int a, int b) {
//    assert(b != 0);
//    div_t r = div(a, b);
//    if (r.rem != 0 && ((a < 0) ^ (b < 0)))
//        r.quot--;
//    return r.quot;
//}

/**
 *
 * i_j = \frac{i-\sum_{l=0}^{j-1}{i_l\prod_{k=l+1}^{d}{n_k}}}{\prod_{k=j+1}^{d}n_k}
 *
 * @tparam T
 * @param val
 * @return
 */
template<typename T>
auto Tensor<T>::unflatten(int val) {
    int d = static_cast<int>(mShape.size());
    std::unordered_map<int, int> prods;
    prods[d - 1] = mShape[d - 1];
    for (int j = d - 2; j >= 0; j--)
        prods[j] = mShape[j] * prods[j + 1];

    std::vector<int> indices = {val/prods[1]};
    for (int j = 1; j < d; j++) {
        double tmp = val;
        for (int l = 0; l < j; l++)
            tmp -= prods[l+1]*indices[l];
        if (j < d - 1)
            tmp = floor(tmp/prods[j+1]);
        indices.emplace_back(tmp);
    }
    return indices;
}

/**
 * Flatten a given list of indices via the formula
 *
 * i = i_d + \sum_{j=0}^{d-1}{i_j\prod_{k=j+1}^{d}{n_k}}
 *
 * with i_j = \{i_0, i_1, \cdots, i_d\}, the "indices",
 * and n_k = \{n_0, n_1, \cdots, n_d\}, the "shape".
 *
 * Remark: The method makes use of variadic template (C++11).
 *
 * @tparam T
 * @tparam Args
 * @param args: index list as (i_0, i_1, ..., i_d).
 * @return
 */
template<typename T>
template<typename... Args>
int Tensor<T>::flatten(Args... args) {
    int size{sizeof...(Args)};
    if (size != mShape.size())
        throw std::logic_error(
                "Index tuple does not belong to this tensor. Number of indices does not match shape size.");

    std::vector<int> indices;
    for (const auto &arg: {args...}) {
        indices.emplace_back(arg);
    };

    int result = indices[size - 1];
    int temp = 1;
    for (int j1 = 0; j1 < size - 1; j1++) {
        temp = indices[j1];
        for (int j2 = j1 + 1; j2 < size; j2++)
            temp *= mShape[j2];
        result += temp;
    }

    return result;
}

template<typename T>
std::unordered_map<int, int> Tensor<T>::getProducts() {
    int d = mShape.size();
    std::unordered_map<int, int> prods;
    prods[d - 1] = mShape[d - 1];
    for (int j = d - 2; j >= 0; j--)
        prods[j] = mShape[j] * prods[j + 1];
    return prods;
}
