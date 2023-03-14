#pragma once

#include <xtensor/xarray.hpp>
#include <xtensor/xexpression.hpp>
#include <xtensor/xlayout.hpp>
#include <xtensor/xrandom.hpp>
#include <xtensor-blas/xlinalg.hpp>

// Wrapper for tensor operations used by nconpp based on xtensor.
namespace nc {
    using namespace xt::placeholders;

    // array
    template<typename T>
    using array_type = xt::xarray<T, xt::layout_type::dynamic>;

    // shape_type
    using shape_type = xt::xarray<std::size_t>::shape_type;

    // general multi-dimensional object
    template<typename T>
    using tensor_type = xt::xexpression<T>;

    template<typename T>
    using tensor = xt::xarray<T>;

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

    namespace linalg {

        // tensordot
        template<typename T, typename O>
        static inline auto
        tensordot(const tensor_type <T> &xa, const tensor_type <O> &xb, const std::vector<std::size_t> &ax_a,
                  const std::vector<std::size_t> &ax_b) -> decltype(xt::linalg::tensordot(xa, xb, ax_a, ax_b)) {
            return xt::linalg::tensordot(xa, xb, ax_a, ax_b);
        }

        // trace
        template<typename T>
        static inline auto
        trace(const tensor_type <T> &M, int offset = 0, std::size_t axis1 = 0, std::size_t axis2 = 1) {
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
        static inline auto transpose(const tensor_type <T> &M, const std::vector<int> &perm) {
            auto &&dM = M.derived_cast();
            return xt::transpose(dM, perm);
        }

        // svd
        template<typename T>
        static inline auto svd(const tensor_type <T> &M, bool full_matrices = true, bool compute_uv = true) {
            auto &&dM = M.derived_cast();
            return xt::linalg::svd(dM, full_matrices, compute_uv);
        }
    }
};















//
//#ifndef TEST_FRIENDS
//#define TEST_FRIENDS
//#endif
//
//#include <algorithm>
//#include <complex>
//#include <numeric>
//#include <utility>
//#include <unordered_map>
//#include <random>
//#include <vector>
//
//#include <xtensor/xarray.hpp>
//#include <xtensor/xexpression.hpp>
//#include <xtensor/xlayout.hpp>
//#include <xtensor/xrandom.hpp>
//#include <xtensor-blas/xlinalg.hpp>
//
//template <class T>
//struct raw_tensor
//{
//    using container_type = std::vector<T>;
//    using shape_type = std::vector<std::size_t>;
//    container_type mData;
//    shape_type mShape;
//    shape_type mStrides;
//    shape_type mBackstrides;
//    static constexpr xt::layout_type layout = xt::layout_type::dynamic;
//};
//
//// This is the adaptor we need to define to plug raw_tensor in xtensor
//template <class T>
//class raw_tensor_adaptor;
//
//template <class T>
//struct xt::xcontainer_inner_types<raw_tensor_adaptor<T>>
//{
//    using container_type = typename raw_tensor<T>::container_type;
//    using inner_shape_type = typename raw_tensor<T>::shape_type;
//    using inner_strides_type = inner_shape_type;
//    using inner_backstrides_type = inner_shape_type;
//    using shape_type = inner_shape_type;
//    using strides_type = inner_shape_type;
//    using backstrides_type = inner_shape_type;
//    using storage_type = container_type;
//    using value_type = T;
//    using reference = T&;
//    using const_reference = const T&;
//    using pointer = T*;
//    using const_pointer = const T*;
//    using size_type = std::size_t;
//    using difference_type = std::ptrdiff_t;
//    using temporary_type = xt::xarray<T>;
//    static constexpr layout_type layout = raw_tensor<T>::layout;
//};
//
//template <class T>
//struct xt::xiterable_inner_types<raw_tensor_adaptor<T>>
//        : xcontainer_iterable_types<raw_tensor_adaptor<T>>
//{
//};
//
///**
// * Tensor class based on numpy and xtensor.
// *
// * References:
// *  - https://numpy.org/
// *  - https://github.com/xtensor-stack/xtensor
// *  - https://xtensor.readthedocs.io/en/latest/external-structures.html
// *  - https://johan-mabille.medium.com/how-we-wrote-xtensor-9365952372d9.
// *
// * @tparam T
// */
//template<class T>
//class Tensor : public xt::xcontainer<raw_tensor_adaptor<T>>,
//               public xt::xcontainer_semantic<raw_tensor_adaptor<T>> {
//public:
//    using container_type = std::vector<T>;
//    using shape_type = std::vector<std::size_t>;
//    static constexpr xt::layout_type layout = xt::layout_type::dynamic;
//
//    Tensor(const Tensor&) = default;
//    Tensor& operator=(const Tensor&) = default;
//
//    Tensor(Tensor&&)  noexcept = default;
//    Tensor& operator=(Tensor&&)  noexcept = default;
//
//    /**
//     * Constructor for a tensor object initialized with a linear data array and a shape.
//     *
//     * @param data
//     * @param shape
//     */
//    explicit Tensor(std::vector<T> data, std::vector<std::size_t> shape) {
//        compute_strides(shape);
//        auto num_elements =
//                std::accumulate(shape.cbegin(), shape.cend(), 1, std::multiplies<std::size_t>{});
//        check_number_elements(num_elements, data.size());
//        mShape = std::move(shape);
//        mData = std::move(data);
//    }
//
//    /**
//     * Constructor for a tensor object initialized with a shape and without data.
//     *
//     * @param shape
//     */
//    explicit Tensor(std::vector<std::size_t> shape) {
//        compute_strides(shape);
//        mShape = std::move(shape);
//    }
//
//    /**
//     * Constructor for a tensor object initialized with a shape given as a initializer_list and without data.
//     *
//     * @param shape
//     */
//    Tensor(std::initializer_list<std::size_t> shape) : mShape(shape) {
//        compute_strides(shape);
//    }
//
//    Tensor() = default;
//
//    ~Tensor() = default;
//
//    /**
//     * Returns the dimension of the tensor, i.e. the number of axes.
//     *
//     * @return
//     */
//    [[nodiscard]] std::size_t dimension() const {
//        return mShape.size();
//    }
//
//    /**
//     * Returns the size of the tensor, i.e. the product of all dimensions of the axes.
//     *
//     * @return
//     */
//    [[nodiscard]] std::size_t size() const {
//        return std::accumulate(mShape.cbegin(), mShape.cend(), std::size_t(1), std::multiplies<>());
//    }
//
//    /**
//     * Returns the number of elements.
//     *
//     * @return
//     */
//    [[nodiscard]] std::size_t num_elements() const {
//        return mData.size();
//    }
//
//    /**
//     * Returns the shape of the tensor.
//     *
//     * @return
//     */
//    [[nodiscard]] const std::vector<std::size_t> &shape() const {
//        return mShape;
//    }
//
//    /**
//     * Reshapes the tensor to a new shape.
//     *
//     * Ensure that the new shape needs to fit the current shape.
//     *
//     * @param shape
//     */
//    void reshape(const std::vector<std::size_t> &shape) {
//        check_new_shape(mShape, shape);
//
//        mShape = shape;
//        compute_strides(shape);
//    }
//
//    /**
//     * Resizes the tensor to a new shape. Resizing changes the number of elements, if necessary.
//     *
//     * @param shape
//     */
//    void resize(const std::vector<std::size_t> &shape) {
//        // TODO
//    }
//
//    /**
//     * Fill the data vector with randomized values using the engine mt19937.
//     *
//     * @param lower
//     * @param upper
//     */
//    void randomize(double lower = 0, double upper = 1.0) {
//        // First create an instance of an engine.
//        std::random_device rnd_device;
//
//        // Specify the engine and distribution.
//        std::mt19937 mersenne_engine{rnd_device()};  // Generates random doubles
//        std::uniform_real_distribution<double> dist(lower, upper);
//
//        int dim = std::accumulate(std::begin(mShape), std::end(mShape), 1, std::multiplies<>());
//        mData = std::vector<T>(dim);
//
//        if (std::is_same<T, std::complex<double>>::value)
//            std::generate(std::begin(mData), std::end(mData), [&dist, &mersenne_engine]() {
//                return std::complex<double>(dist(mersenne_engine), dist(mersenne_engine));
//            });
//        else
//            std::generate(std::begin(mData), std::end(mData), [&dist, &mersenne_engine]() {
//                return dist(mersenne_engine);
//            });
//    };
//
//    /**
//     * Return the product of array elements over a given axis.
//     *
//     * @param axis
//     * @return
//     */
//    T prod(std::size_t axis) {
//        std::vector<std::size_t> indices = mShape;
//        for (std::size_t &i: indices)
//            i -= 1;
//
//        indices[axis] = 0;
//        int index = flatten(indices);
//        auto prod = mData[index];
//        for (std::size_t i = 1; i < mShape[axis]; ++i) {
//            indices[axis] = i;
//            prod *= mData[flatten(indices)];
//        }
//        return prod;
//    }
//
//    /**
//     * Return the products of array elements over given axes.
//     *
//     * @param axes
//     * @return
//     */
//    std::vector<T> prod(const std::vector<std::size_t> &axes) {
//        std::vector<T> res(axes.size());
//        for (std::size_t i = 0; i < axes.size(); ++i)
//            res[i] = prod(axes[i]);
//        return std::move(res);
//    }
//
//    /**
//     * @brief Expand the shape of an array.
//     *
//     * Insert a new axis that will appear at the axis position in the expanded array shape.
//     *
//     * @param axis
//     */
//    void expand_dims(std::size_t axis) {
//        mShape.insert(mShape.begin() + static_cast<long>(axis), 1);
//        compute_strides(mShape);
//    }
//
//    /**
//     * Transpose the tensor to a new permutation.
//     *
//     * @param perm
//     */
//    void transpose(const std::vector<std::size_t> &perm) {
//        check_perm(perm);
//        reorder(mShape, perm);
//        reorder(mStrides, perm);
//    }
//
//    /**
//     * Returns a const copy of the data array.
//     *
//     * @return
//     */
//    const auto &getData() {
//        return mData;
//    }
//
//    /**
//     * Return the strides.
//     *
//     * @return
//     */
//    const auto &strides() {
//        return mStrides;
//    };
//
//    /**
//     * Return the backstrides.
//     *
//     * @return
//     */
//    const auto &backstrides() {
//        return mBackstrides;
//    };
//
//    /**
//     * Access operator. Returns the element at position.
//     *
//     * @tparam I
//     * @param i
//     * @return
//     */
//    template<class... I>
//    T &operator()(I... i) {
//        return mData[flatten(i...)];
//    }
//
//    /**
//     * Access operator. Returns the element at position.
//     *
//     * @tparam I
//     * @param i
//     * @return
//     */
//    template<class... I>
//    const T &operator()(I... i) const {
//        return mData[flatten(i...)];
//    }
//
//    /**
//     * Asterisk-equal operator, i.e. multiplication by a single value. Returns the new tensor.
//     *
//     * @tparam I
//     * @param rhs
//     * @return
//     */
//    template<class I>
//    Tensor<T> &operator*=(I rhs) {
//        std::transform(mData.begin(), mData.end(), mData.begin(),
//                       std::bind(std::multiplies<T>(), std::placeholders::_1, rhs));
//        return *this;
//    };
//
//    /**
//     * Asterisk-equal operator, i.e. multiplication by a single value. Returns the new tensor.
//     *
//     * @tparam I
//     * @param i
//     * @return
//     */
//    template<class I>
//    const Tensor<T> &operator*=(I rhs) const {
//        std::transform(mData.begin(), mData.end(), mData.begin(),
//                       std::bind(std::multiplies<T>(), std::placeholders::_1, rhs));
//        return *this;
//    }
//
//private:
//    TEST_FRIENDS;
//    container_type mData;
//    shape_type mShape;
//    shape_type mStrides;
//    shape_type mBackstrides;
//
//    /**
//     * @brief flatten indices given via variadic template
//     *
//     * Flatten a given list of indices via the formula
//     * i = i_d + \sum_{j=0}^{d-1}{i_j\prod_{k=j+1}^{d}{n_k}}
//     * with i_j = \{i_0, i_1, \cdots, i_d\}, the "indices",
//     * and n_k = \{n_0, n_1, \cdots, n_d\}, the "shape".
//     *
//     * @tparam Args: variadic template (C++11)
//     * @param args: index list as (i_0, i_1, ..., i_d).
//     * @return
//     */
//    template<typename... Args>
//    std::size_t flatten(Args... args) {
//        std::size_t size{sizeof...(Args)};
//        check_index_size(size);
//
//        std::vector<std::size_t> indices;
//        for (const auto &arg: {args...})
//            indices.emplace_back(arg);
//
//        return flatten_details(size, indices);
//    }
//
//    /**
//     * @brief flatten indices given via vector
//     *
//     * Flatten a given list of indices via the formula
//     * i = \sum_{j=0}^{d}{i_j\prod_{k=j+1}^{d}{n_k}}
//     * with i_j = \{i_0, i_1, \cdots, i_d\}, the "indices",
//     * and n_k = \{n_0, n_1, \cdots, n_d\}, the "shape".
//     *
//     * @param args
//     * @return
//     */
//    std::size_t flatten(std::vector<std::size_t> indices) {
//        std::size_t size = indices.size();
//        check_index_size(size);
//        return flatten_details(size, indices);
//    }
//
//    /**
//     * Common details for methods flatten.
//     *
//     * @param size
//     * @param indices
//     * @return
//     */
//    std::size_t flatten_details(std::size_t size, std::vector<std::size_t> indices) {
//        std::size_t id = 0;
//        for (std::size_t j = 0; j < size; j++) {
//            check_index(indices[j], j);
//            id += std::multiplies{}(indices[j], mStrides[j]);
//        }
//        return id;
//    }
//
//    /**
//     * Reorders a vector.
//     *
//     * @param v
//     * @param order
//     */
//    void reorder(std::vector<std::size_t> &v, const std::vector<std::size_t> &order) {
//        auto orderCopy = order;
//        std::size_t i, j, k;
//        for (i = 0; i < orderCopy.size() - 1; ++i) {
//            j = orderCopy[i];
//            if (j != i) {
//                for (k = i + 1; order[k] != i; ++k);
//                std::swap(orderCopy[i], orderCopy[k]);
//                std::swap(v[i], v[j]);
//            }
//        }
//    }
//
//    /**
//     * Computes the strides and backstrides for a given shape.
//     *
//     * @param shape
//     */
//    void compute_strides(const std::vector<std::size_t> &shape) {
//        std::size_t d = shape.size();
//        mStrides.resize(d);
//        mBackstrides.resize(d);
//        mStrides.back() = 1;
//        mBackstrides.front() = 1;
//        for (std::size_t j = d - 1; j > 0; --j) {
//            mStrides[j - 1] = mStrides[j] * shape[j];
//        }
//        mStrides[d - 1] = 1;
//        for (std::size_t j = 0; j < d - 1; ++j) {
//            mBackstrides[j + 1] = mBackstrides[j] * shape[j];
//        }
//    }
//
//    /**
//     * Check index size.
//     *
//     * @param index_size
//     */
//    void check_index_size(std::size_t index_size) {
//        if (index_size != mShape.size())
//            throw std::logic_error(
//                    "Index tuple does not belong to this tensor. Number of indices does not match shape size.");
//    }
//
//    /**
//     * Check if a index can be in the indices of this tensor.
//     *
//     * @param index
//     * @param axis
//     */
//    void check_index(std::size_t index, std::size_t axis) {
//        if (index > mShape[axis] - 1)
//            throw std::logic_error(
//                    "Index " + std::to_string(index) + " out of range for axis " + std::to_string(axis) +
//                    " with dimension " + std::to_string(mShape[axis]));
//    }
//
//    /**
//     * Check if permutation fits the shape.
//     *
//     * @param perm
//     */
//    void check_perm(const std::vector<std::size_t> &perm) {
//        if (perm.size() != mShape.size())
//            throw std::logic_error("Number of axes in perm do not match shape.");
//    }
//
//    /**
//     * Check if number of elements corresponds to the size of the data array.
//     *
//     * @param num_elements
//     * @param data_size
//     */
//    void check_number_elements(std::size_t num_elements, std::size_t data_size) {
//        if (num_elements != data_size)
//            throw std::logic_error(
//                    "Size of data container does not match number of possible num_elements.");
//    }
//
//    /**
//     * Check if new shape fits the current shape.
//     *
//     * @param s1
//     * @param s2
//     */
//    void check_new_shape(std::vector<std::size_t> s1, std::vector<std::size_t> s2) {
//        auto dim1 = std::accumulate(s1.begin(), s1.end(), 1, std::multiplies<>());
//        auto dim2 = std::accumulate(s2.begin(), s2.end(), 1, std::multiplies<>());
//        if (dim1 != dim2)
//            throw std::logic_error(
//                    "Total number of elements in new shape does not match old shape.");
//    }
//};
//
//template<class I, class T>
//constexpr Tensor<T> operator*(I lhs, const Tensor<T> &rhs) {
//    auto cp = rhs;
//    return cp *= lhs;
//}
//
//template<class I, class T>
//constexpr Tensor<T> operator*(const Tensor<T> &lhs, I rhs) {
//    auto cp = lhs;
//    return cp *= rhs;
//}
