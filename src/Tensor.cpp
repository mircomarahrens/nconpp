#include "Tensor.h"

#include <utility>

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
    return mData.dimension();
};

template<typename T>
void Tensor<T>::prod(std::size_t axis) {
    //xt::prod(mData, axis);
}

template<typename T>
void Tensor<T>::prod(const std::vector<std::size_t> &axis) {
//    xt::prod(mData, axis);
}

template<typename T>
void Tensor<T>::expand_dims(std::size_t axis) {
//    xt::expand_dims(mData, axis);
}

template<typename T>
void Tensor<T>::transpose(const std::vector<int> &perm) {
//    xt::transpose(mData, perm);
}

template<typename T>
const auto &Tensor<T>::getData() {
    return mData;
}
