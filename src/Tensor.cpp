#include "Tensor.h"

#include <algorithm>
#include <complex>
#include <random>

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
    mShape = shape;
}

template<typename T>
auto Tensor<T>::dimension() {
    return mShape.size();
};

// Return the product of array elements over a given axis.
template<typename T>
void Tensor<T>::prod(std::size_t axis) {
    //xt::prod(mData, axis);
}

// Return the product of array elements over a given axis.
template<typename T>
void Tensor<T>::prod(const std::vector<std::size_t> &axis) {
//    xt::prod(mData, axis);
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
void Tensor<T>::randomize(size_t lower, size_t upper) {
    // First create an instance of an engine.
    std::random_device rnd_device;

    // Specify the engine and distribution.
    std::mt19937 mersenne_engine{rnd_device()};  // Generates random doubles
    std::uniform_real_distribution<double> dist(0, 1.0);

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
