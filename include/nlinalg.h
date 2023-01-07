#pragma once

#include <algorithm>
#include <cassert>
#include "Tensor.h"
#include "nblas.h"

namespace nlinalg {

    /**
     * Non-broadcasting dot function.
     * In the case of two 1D vectors, computes the vector dot
     * product. In the case of complex vectors, computes the dot
     * product without conjugating the first argument.
     * If \em t or \em o is a 2D matrix, computes the matrix-times-vector
     * product. If both \em t and \em o or 2D matrices, computes
     * the matrix-product.
     *
     * @param t input array
     * @param o input array
     *
     * @return resulting array
     */
    template <class T, class U>
    auto dot(const Tensor<T>& t, const Tensor<U>& o) {

        using common_type_t = typename std::common_type<T, U>::type;
        Tensor<common_type_t> result;

        // scalar multiplication
        if (t.dimension() == 0 || o.dimension() == 0)
        {
            return (t * o);
        }

        // vector-vector multiplication
        if (t.dimension() == 1 && o.dimension() == 1)
        {
//            // TODO implement resize
//            result.resize(std::vector<std::size_t>{1});
            assert(("Dot: shape mismatch.", t.shape()[0] != o.shape()[0]));

            if (std::is_same<typename common_type_t::value_type, std::complex<double>>::value)
            {
                nblas::dotu(t, o, result(0));
            }
            else
            {
                nblas::dot(t, o, result(0));
            }

            return result;
        }
        else
        {
             //TODO
//            if (t.dimension() == 2 && o.dimension() == 1)
//            {
//                XTENSOR_ASSERT(t.layout() == layout_type::row_major || t.layout() == layout_type::column_major);
//                XTENSOR_ASSERT(std::min(t.strides()[0], t.strides()[1]) <= 1);
//
//                if (t.shape()[1] != o.shape()[0])
//                {
//                    XTENSOR_THROW(std::runtime_error, "Dot: shape mismatch.");
//                }
//
//                result.resize({static_cast<std::size_t>(t.shape()[0])});
//
//                blas_index_t shape_x, shape_y;
//                cxxblas::Transpose trans;
//                if (result.layout() != t.layout())
//                {
//                    shape_x = static_cast<blas_index_t>(t.shape()[1]);
//                    shape_y = static_cast<blas_index_t>(t.shape()[0]);
//                    trans = cxxblas::Transpose::Trans;
//                }
//                else
//                {
//                    shape_x = static_cast<blas_index_t>(t.shape()[0]);
//                    shape_y = static_cast<blas_index_t>(t.shape()[1]);
//                    trans = cxxblas::Transpose::NoTrans;
//                }
//
//                cxxblas::gemv<blas_index_t>(
//                        get_blas_storage_order(result),
//                        trans,
//                        shape_x,
//                        shape_y,
//                        value_type(1.0),
//                        t.data() + t.data_offset(),
//                        get_leading_stride(t),
//                        o.data() + o.data_offset(),
//                        get_leading_stride(o),
//                        value_type(0.0),
//                        result.data(),
//                        get_leading_stride(result)
//                );
//            }
//            else if (t.dimension() == 1 && o.dimension() == 2)
//            {
//                XTENSOR_ASSERT(o.layout() == layout_type::row_major || o.layout() == layout_type::column_major);
//                XTENSOR_ASSERT(std::min(o.strides()[0], o.strides()[1]) <= 1);
//
//                if (t.shape()[0] != o.shape()[0])
//                {
//                    XTENSOR_THROW(std::runtime_error, "Dot: shape mismatch.");
//                }
//
//                result.resize({static_cast<std::size_t>(o.shape()[1])});
//
//                blas_index_t shape_x, shape_y;
//                cxxblas::Transpose trans;
//                if (result.layout() != o.layout())
//                {
//                    shape_x = static_cast<blas_index_t>(o.shape()[1]);
//                    shape_y = static_cast<blas_index_t>(o.shape()[0]);
//                    trans = cxxblas::Transpose::NoTrans;
//                }
//                else
//                {
//                    shape_x = static_cast<blas_index_t>(o.shape()[0]);
//                    shape_y = static_cast<blas_index_t>(o.shape()[1]);
//                    trans = cxxblas::Transpose::Trans;
//                }
//
//                cxxblas::gemv<blas_index_t>(
//                        get_blas_storage_order(result),
//                        trans,
//                        shape_x,
//                        shape_y,
//                        value_type(1.0),
//                        o.data() + o.data_offset(),
//                        get_leading_stride(o),
//                        t.data() + t.data_offset(),
//                        get_leading_stride(t),
//                        value_type(0.0),
//                        result.data(),
//                        get_leading_stride(result)
//                );
//            }
//            else if (t.dimension() == 2 && o.dimension() == 2)
//            {
//                XTENSOR_ASSERT(o.layout() == layout_type::row_major || o.layout() == layout_type::column_major);
//                XTENSOR_ASSERT(std::min(o.strides()[0], o.strides()[1]) <= 1);
//                XTENSOR_ASSERT(t.layout() == layout_type::row_major || t.layout() == layout_type::column_major);
//                XTENSOR_ASSERT(std::min(t.strides()[0], t.strides()[1]) <= 1);
//
//                if (t.shape()[1] != o.shape()[0])
//                {
//                    XTENSOR_THROW(std::runtime_error, "Dot: shape mismatch.");
//                }
//
//                cxxblas::Transpose transpose_A = cxxblas::Transpose::NoTrans,
//                        transpose_B = cxxblas::Transpose::NoTrans;
//
//                if (result.layout() != t.layout())
//                {
//                    transpose_A = cxxblas::Transpose::Trans;
//                }
//                if (result.layout() != o.layout())
//                {
//                    transpose_B = cxxblas::Transpose::Trans;
//                }
//
//                // This adds a fast path for A * A' by calling SYRK and only computing
//                // the upper triangle
//                if (std::is_same<typename T::value_type, typename O::value_type>::value &&
//                    (static_cast<const void*>(t.data() + t.data_offset()) == static_cast<const void*>(o.data() + o.data_offset())) &&
//                    ((transpose_A == cxxblas::Transpose::Trans && transpose_B == cxxblas::Transpose::NoTrans) ||
//                     (transpose_A == cxxblas::Transpose::NoTrans && transpose_B == cxxblas::Transpose::Trans)))
//                {
//                    // TODO add check to compare strides & shape
//
//                    result.resize({static_cast<std::size_t>(t.shape()[0]), static_cast<std::size_t>(t.shape()[0])});
//
//                    cxxblas::syrk<blas_index_t>(
//                            get_blas_storage_order(result),
//                            cxxblas::StorageUpLo::Upper,
//                            transpose_A,
//                            static_cast<blas_index_t>(t.shape()[0]),
//                            static_cast<blas_index_t>(t.shape()[1]),
//                            value_type(1.0),
//                            t.data() + t.data_offset(),
//                            get_leading_stride(t),
//                            value_type(0.0),
//                            result.data(),
//                            get_leading_stride(result)
//                    );
//
//                    for (std::size_t i = 0; i < t.shape()[0]; ++i)
//                    {
//                        for (std::size_t j = i + 1; j < t.shape()[0]; ++j)
//                        {
//                            result(j, i) = result(i, j);
//                        }
//                    }
//                    return result;
//                }
//
//                result.resize({static_cast<std::size_t>(t.shape()[0]), static_cast<std::size_t>(o.shape()[1])});
//
//                cxxblas::gemm<blas_index_t>(
//                        get_blas_storage_order(result),
//                        transpose_A,
//                        transpose_B,
//                        static_cast<blas_index_t>(t.shape()[0]),
//                        static_cast<blas_index_t>(o.shape()[1]),
//                        static_cast<blas_index_t>(o.shape()[0]),
//                        value_type(1.0),
//                        t.data() + t.data_offset(),
//                        get_leading_stride(t),
//                        o.data() + o.data_offset(),
//                        get_leading_stride(o),
//                        value_type(0.0),
//                        result.data(),
//                        get_leading_stride(result)
//                );
//            }
//            else
//            {
//                // TODO more testing for different layouts!
//                std::size_t l = t.shape().back();
//                std::size_t match_dim = 0;
//
//                if (o.dimension() > 1)
//                {
//                    match_dim = o.dimension() - 2;
//                }
//                if (o.shape()[match_dim] != l)
//                {
//                    XTENSOR_THROW(std::runtime_error, "Dot: shape mismatch.");
//                }
//
//                blas_index_t a_dim = static_cast<blas_index_t>(t.dimension());
//                blas_index_t b_dim = static_cast<blas_index_t>(o.dimension());
//
//                blas_index_t nd = a_dim + b_dim - 2;
//
//                std::size_t j = 0;
//                std::vector<std::size_t> dimensions(static_cast<std::size_t>(nd));
//
//                for (blas_index_t i = 0; i < a_dim - 1; ++i)
//                {
//                    dimensions[j++] = t.shape()[static_cast<std::size_t>(i)];
//                }
//                for (blas_index_t i = 0; i < b_dim - 2; ++i)
//                {
//                    dimensions[j++] = o.shape()[static_cast<std::size_t>(i)];
//                }
//                if (b_dim > 1)
//                {
//                    dimensions[j++] = o.shape().back();
//                }
//
//                result.resize(dimensions);
//
//                blas_index_t a_stride = static_cast<blas_index_t>(t.strides().back());
//                blas_index_t b_stride = static_cast<blas_index_t>(o.strides()[match_dim]);
//
//                auto a_iter = detail::offset_iter_without_axis<std::decay_t<decltype(t)>>(t, t.dimension() - 1);
//                auto b_iter = detail::offset_iter_without_axis<std::decay_t<decltype(o)>>(o, match_dim);
//
//                value_type temp;
//                auto result_it = result.begin();
//
//                do
//                {
//                    do
//                    {
//                        cxxblas::dot<blas_index_t>(
//                                static_cast<blas_index_t>(l),
//                                t.data() + a_iter.offset(),
//                                a_stride,
//                                o.data() + b_iter.offset(),
//                                b_stride,
//                                temp
//                        );
//                        *(result_it++) = temp;
//
//                    } while (b_iter.next());
//
//                } while (a_iter.next());
//
//            }
            return result;
        }
    }

    /**
     * @brief Compute tensor dot product along specified axes
     *
     * Generic entrypoint to compute the tensor product of two tensors a,b along axes ax_a, ax_b.
     *
     * @tparam T
     * @tparam U
     * @param a
     * @param b
     * @param ax_a
     * @param ax_b
     * @return
     */
    template<class T, class U>
    auto tensordot(const Tensor<T>& a, const Tensor<U>& b, const std::vector<std::size_t>& ax_a = {0}, const std::vector<std::size_t>& ax_b = {0}) {

        using common_type_t = typename std::common_type<T, U>::type;
        Tensor<common_type_t> result;

        assert(ax_a.size() == ax_b.size());
        assert(ax_a.size() < a.dimension());
        assert(ax_b.size() < b.dimension());
        std::size_t n_ax = ax_a.size();
        for (std::size_t i = 0; i < n_ax; ++i)
        {
            assert(ax_a[i] < a.dimension());
            assert(ax_b[i] < b.dimension());
        }

        // scalar multiplication
        if (a.dimension() == 0 or b.dimension() == 0) {
            return (a(0)*b(0));
        }
        // vector/vector multiplication
        else if (a.dimension() == 1 and b.dimension() == 1) {
            return dot(a, b);
        }
        // matrix/matrix multiplication
        else if (a.dimension() == 2 and b.dimension() == 2) {
            return dot(a, b);
        }
        // multiply by scalar
        else if (a.dimension() == 0) {
            return dot(b, a);
        }
        else if (b.dimension() == 0) {
            return dot(a, b);
        }
        // tensor product
        else {
            // Sum of products over last n axes of a and the first n axis of b, therefore

            // Move the axes to sum over to the end of a
            std::vector<std::size_t> newaxes_a = {};
            for (std::size_t i = 0; i < a.dimension(); ++i) {
                auto a_ax_it = std::find(ax_a.begin(), ax_a.end(), i);
                // first pass if i is not in ax_a, add to newaxes_a
                if (a_ax_it == ax_a.end())
                {
                    newaxes_a.push_back(i);
                }
            }
            for (auto& a_ax_it : ax_a)
            {
                newaxes_a.push_back(a_ax_it);
            }

            // Move the axes to sum over to the start of b
            std::vector<std::size_t> newaxes_b = {};
            for(auto& b_ax_it : ax_b)
            {
                newaxes_b.push_back(b_ax_it);
            }
            for (std::size_t i = 0; i < b.dimension(); ++i)
            {
                auto b_ax_it = std::find(ax_b.begin(), ax_b.end(), i);
                // second pass if i is not in ax_b add to newaxes_b
                if (b_ax_it == ax_b.end())
                {
                    newaxes_b.push_back(i);
                }
            }

            // transpose tensors
            auto new_a = transpose(a, newaxes_a);
            auto new_b = transpose(b, newaxes_b);

            // then
            return tensordot(a,b, n_ax);
        }
    }

    /**
     * @brief Compute tensor dot product along specified axes for arrays
     *
     * Compute the sum of products along the last \em naxes axes of a and first
     * \em naxes axes of b.
     *
     * @param a input tensor
     * @param b input tensor
     * @param naxes the number of axes to sum over
     * @return resulting tensor
     */
    template <class T, class U>
    auto tensordot(const Tensor<T>& a, const Tensor<U>& b, std::size_t naxes = 2)
    {
        using common_type_t = typename std::common_type<T, U>::type;
        Tensor<common_type_t> result;

        auto shape_a = a.shape();
        auto shape_b = b.shape();

        if (naxes == 0)
        {
            // special case tensor outer product
            std::vector<std::size_t> result_shape(a.dimension() + b.dimension());
            result_shape.insert(result_shape.end(), shape_a.begin(), shape_a.end());
            result_shape.insert(result_shape.end(), shape_b.begin(), shape_b.end());

            // flatten a/b
            auto vec_a = ravel(a);
            auto vec_b = ravel(b);
            // take the outer product of the two vectors
            result = outer(vec_a, vec_b);
            // reshape the result
            result.reshape(result_shape);
        }
        else
        {
            // Sum of products over last n axes of A and the first n axis of b
            assert(a.dimension() >= naxes);
            assert(b.dimension() >= naxes);

            auto as_it = a.shape().begin() + (a.dimension() - naxes);
            auto bs_it = b.shape().begin();
            std::size_t sum_len = 1;
            for (std::size_t i = 0; i < naxes; ++i)
            {
                auto a_val = *as_it;
                auto b_val = *bs_it;
                // check for axes size match
                assert(("Shape mismatch for sum", a_val != b_val));
                sum_len *= a_val;
                ++as_it;
                ++bs_it;
            }
            // TODO ff
            std::vector<std::size_t> result_shape;
            std::size_t keep_a_len = 1;
            for (auto it = shape_a.begin(); it != shape_a.begin() + (a.dimension() - naxes); ++it)
            {
                std::size_t len = *it;
                keep_a_len *= len;
                result_shape.push_back(len);
            }
            std::size_t keep_b_len = 1;
            for (auto it = shape_b.begin() + naxes; it != shape_b.end(); ++it)
            {
                std::size_t len = *it;
                keep_b_len *= len;
                result_shape.push_back(len);
            }
            Tensor<common_type_t> a_mat = a;
            a_mat.reshape({keep_a_len, sum_len});
            Tensor<common_type_t> b_mat = b;
            b_mat.reshape({sum_len, keep_b_len});
            result = dot(a_mat, b_mat);
            if(result_shape.empty())
            {
                result.reshape({1});
            }
            else
            {
                result.reshape(result_shape);
            }
        }
        return result;
    }

    /**
     * Compute the outer product of two vectors.
     *
     * @param a input vector (1D)
     * @param b input vector (1D)
     *
     * @return resulting vector
     */
    template <class T, class U>
    auto outer(const Tensor<T>& a, const Tensor<U>& b) {
        using common_type_t = typename std::common_type<T, U>::type;
        Tensor<common_type_t> result;

        assert(a.dimension() == 1);
        assert(b.dimension() == 1);

        nblas::ger(a, b, result);

        return result;
    }

    /**
     * Copies the input tensor and transpose the copied version with perm
     *
     * @tparam U
     * @param a
     * @param perm
     * @return resulting tensor
     */
    template<class U>
    Tensor<U> transpose(const Tensor<U>& a, const std::vector<std::size_t> &perm) {
        Tensor<U> b(a);
        b.transpose(perm);
        return std::move(b);
    }

    /**
     * Copies the input tensor and flatten the copy
     *
     * @tparam U
     * @param s
     * @return resulting tensor
     */
    template<class U>
    Tensor<U> ravel(const Tensor<U>& a) {
        Tensor<U> b(a);
        b.reshape(std::accumulate(a.shape().cbegin(), a.shape().cend(), 1, std::multiplies<U>{}));
        return std::move(b);
    }
}