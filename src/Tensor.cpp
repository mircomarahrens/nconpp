#include "Tensor.h"

#include <algorithm>
#include <complex>
#include <random>
#include <unordered_map>

template<class T>
Tensor<T>::Tensor(std::vector<T> data, std::vector<std::size_t> shape) {
    compute_strides(shape);
    auto num_elements =
            std::accumulate(shape.cbegin(), shape.cend(), 1, std::multiplies<std::size_t>{});
    check_number_elements(num_elements, data.size());
    mShape = std::move(shape);
    mData = std::move(data);
}

template<class T>
Tensor<T>::Tensor(std::vector<std::size_t> shape) {
    compute_strides(shape);
    mShape = std::move(shape);
}

template<class T>
Tensor<T>::Tensor(std::initializer_list<std::size_t> shape) : mShape(shape) { compute_strides(shape); }

template<class T>
std::size_t Tensor<T>::dimension() const {
    return mShape.size();
};

template<class T>
std::size_t Tensor<T>::size() const {
    return std::accumulate(mShape.cbegin(), mShape.cend(), std::size_t(1), std::multiplies<>());
}

template<class T>
const std::vector<std::size_t> &Tensor<T>::shape() {
    return mShape;
}

template<class T>
void Tensor<T>::reshape(const std::vector<std::size_t> &shape) {
    check_new_shape(mShape, shape);

    mShape = shape;
    compute_strides(shape);
}

/**
 * Return the product of array elements over a given axis.
 *
 * @tparam T
 * @param axis
 * @return
 */
template<class T>
T Tensor<T>::prod(std::size_t axis) {
    std::vector<std::size_t> indices = mShape;
    for (std::size_t &i: indices)
        i -= 1;

    indices[axis] = 0;
    int index = flatten(indices);
    auto prod = mData[index];
    for (std::size_t i = 1; i < mShape[axis]; ++i) {
        indices[axis] = i;
        prod *= mData[flatten(indices)];
    }
    return prod;
}

/**
 * Return the products of array elements over given axes.
 *
 * @tparam T
 * @param axes
 * @return
 */
template<class T>
std::vector<T> Tensor<T>::prod(const std::vector<std::size_t> &axes) {
    std::vector<T> res(axes.size());
    for (std::size_t i = 0; i < axes.size(); ++i)
        res[i] = prod(axes[i]);
    return std::move(res);
}

// Expand the shape of an array.
// Insert a new axis that will appear at the axis position in the expanded array shape.
template<class T>
void Tensor<T>::expand_dims(std::size_t axis) {
    mShape.insert(mShape.begin() + axis, 1);
    compute_strides(mShape);
}

template<class T>
void Tensor<T>::transpose(const std::vector<std::size_t> &perm) {
    check_perm(perm);
    reorder(mShape, perm);
    reorder(mStrides, perm);
}

template<class T>
const auto &Tensor<T>::getData() {
    return mData;
}

/**
 * Fill the data vector with randomized values using the engine mt19937.
 *
 * @tparam T
 * @param lower
 * @param upper
 */
template<class T>
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

/**
 * Flatten a given list of indices via the formula
 * i = i_d + \sum_{j=0}^{d-1}{i_j\prod_{k=j+1}^{d}{n_k}}
 * with i_j = \{i_0, i_1, \cdots, i_d\}, the "indices",
 * and n_k = \{n_0, n_1, \cdots, n_d\}, the "shape".
 *
 * @tparam T
 * @tparam Args: variadic template (C++11)
 * @param args: index list as (i_0, i_1, ..., i_d).
 * @return
 */
template<class T>
template<typename... Args>
std::size_t Tensor<T>::flatten(Args... args) {
    std::size_t size{sizeof...(Args)};
    check_index_size(size);

    std::vector<std::size_t> indices;
    for (const auto &arg: {args...})
        indices.emplace_back(arg);

    return flatten_details(size, indices);
}

/**
 * Flatten a given list of indices via the formula
 * i = \sum_{j=0}^{d}{i_j\prod_{k=j+1}^{d}{n_k}}
 * with i_j = \{i_0, i_1, \cdots, i_d\}, the "indices",
 * and n_k = \{n_0, n_1, \cdots, n_d\}, the "shape".
 *
 * @tparam T
 * @param args
 * @return
 */
template<class T>
std::size_t Tensor<T>::flatten(std::vector<std::size_t> indices) {
    std::size_t size = indices.size();
    check_index_size(size);
    return flatten_details(size, indices);
}

template<class T>
std::size_t
Tensor<T>::flatten_details(std::size_t size, std::vector<std::size_t> indices) {
    std::size_t id = 0;
    for (std::size_t j = 0; j < size; j++) {
        check_index(indices[j], j);
        id += std::multiplies{}(indices[j], mStrides[j]);
    }
    return id;
}


template<class T>
void Tensor<T>::reorder(std::vector<std::size_t> &v, const std::vector<std::size_t> &order) {
    auto orderCopy = order;
    std::size_t i,j,k;
    for(i = 0; i < orderCopy.size() - 1; ++i) {
        j = orderCopy[i];
        if (j != i) {
            for (k = i + 1; order[k] != i; ++k);
            std::swap(orderCopy[i], orderCopy[k]);
            std::swap(v[i], v[j]);
        }
    }
}

/**
 * Compute the products used in the methods flatten and unflatten.
 *
 * @tparam T
 */
template<class T>
void Tensor<T>::compute_strides(const std::vector<std::size_t> &shape) {
    std::size_t d = shape.size();
    mStrides = std::vector<std::size_t>(d);
    mStrides[d - 1] = 1;
    for (auto j = d - 1; j-- > 0; )
        mStrides[j] = std::multiplies{}(shape[j + 1], mStrides[j + 1]);
}

template<class T>
template<class... I>
T &Tensor<T>::operator()(I... i) {
    return mData[flatten(i...)];
}

template<class T>
template<class... I>
const T &Tensor<T>::operator()(I... i) const {
    return mData[flatten(i...)];
}

template<class T>
void Tensor<T>::check_index_size(std::size_t index_size) {
    if (index_size != mShape.size())
        throw std::logic_error(
                "Index tuple does not belong to this tensor. Number of indices does not match shape size.");
}

template<class T>
void Tensor<T>::check_number_elements(std::size_t num_elements, std::size_t data_size) {
    if (num_elements != data_size)
        throw std::logic_error(
                "Size of data container does not match number of possible num_elements.");
}

template<class T>
void Tensor<T>::check_new_shape(std::vector<std::size_t> s1, std::vector<std::size_t> s2) {
    auto dim1 = std::accumulate(s1.begin(), s1.end(), 1, std::multiplies<>());
    auto dim2 = std::accumulate(s2.begin(), s2.end(), 1, std::multiplies<>());
    if (dim1 != dim2)
        throw std::logic_error(
                "Total number of elements in new shape does not match old shape.");
}

template<class T>
void Tensor<T>::check_index(std::size_t index, std::size_t axis) {
    if (index > mShape[axis] - 1)
        throw std::logic_error(
                "Index " + std::to_string(index) + " out of range for axis " + std::to_string(axis) +
                " with dimension " + std::to_string(mShape[axis]));
}

template<class T>
void Tensor<T>::check_perm(const std::vector<std::size_t> &perm) {
    if (perm.size() != mShape.size())
        throw std::logic_error("Number of axes in perm do not match shape.");
}

template<class T>
auto &Tensor<T>::strides() {
    return mStrides;
}
