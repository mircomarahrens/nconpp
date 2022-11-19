#pragma once

#include <algorithm>
#include <cassert>
#include "Tensor.h"
#include "nblas.h"

namespace nlinalg {
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
    auto tensordot(Tensor<T> a, Tensor<U> b, const std::vector<size_t>& ax_a = {0}, const std::vector<size_t>& ax_b = {0}) {

        assert(ax_a.size() == ax_b.size());
        assert(ax_a.size() < a.dimension());
        assert(ax_b.size() < b.dimension());
        std::size_t n_ax = ax_a.size();
        for (std::size_t i = 0; i < n_ax; ++i)
        {
            assert(ax_a[i] < a.dimension());
            assert(ax_b[i] < b.dimension());
        }

//        // scalar multiplication
//        if (a.dimension() == 1 and b.dimension() == 1) {
//            return (a[0]*b[0]);
//        }
//        // matrix/matrix multiplication
//        else if (a.dimension() == 2 and b.dimension() == 2) {
//            return nblas::dot(a, b, 2);
//        }
//        // multiply by scalar
//        else if (a.dimension() == 1) {
//            return nblas::dot(b, a, 1);
//        }
//        else if (b.dimension() == 1) {
//            return nblas::dot(a, b, 1);
//        }
//        // tensor product
//        else {
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
            a.transpose(newaxes_a);
            b.transpose(newaxes_b);

            // then
            return 0;//tensordot(a,b, n_ax);
//        }
    }

    /**
     * @brief Compute tensor dot product along specified axes for arrays
     *
     * Compute the sum of products along the last \em naxes axes of a and first
     * \em naxes axes of b.
     *
     * @param xa input array
     * @param xb input array
     * @param naxes the number of axes to sum over
     * @return resulting array
     */
//    template <class T, class U>
//    auto tensordot(Tensor<T> a, Tensor<U> b,, std::size_t naxes = 2)
//    {
//        using value_type = std::common_type_t<typename T::value_type, typename O::value_type>;
//        using result_type = std::conditional_t<T::static_layout == O::static_layout &&
//                                               (T::static_layout != layout_type::dynamic && T::static_layout != layout_type::any),
//                xarray<value_type, T::static_layout>,
//        xarray<value_type, XTENSOR_DEFAULT_LAYOUT>>;
//
//        result_type result;
//        auto&& a = view_eval<T::static_layout>(xa.derived_cast());
//        auto&& b = view_eval<O::static_layout>(xb.derived_cast());
//        if (naxes == 0)
//        {
//            // special case tensor outer product product
//            xt::dynamic_shape<std::size_t> result_shape(a.dimension() + b.dimension());
//            std::size_t j = 0;
//            for (std::size_t i = 0; i < a.dimension(); ++i)
//            {
//                result_shape[j++] = a.shape()[i];
//            }
//
//            for (std::size_t i = 0; i < b.dimension(); ++i)
//            {
//                result_shape[j++] = b.shape()[i];
//            }
//            // flatten a/b
//            auto vec_a = xt::ravel<result_type::static_layout>(a);
//            auto vec_b = xt::ravel<result_type::static_layout>(b);
//            // take the outer product of the two vectors
//            result = outer(vec_a, vec_b);
//            // reshape the result
//            result.reshape(result_shape);
//        }
//        else
//        {
//            // Sum of products over last n axes of A and the first n axis of b
//            XTENSOR_ASSERT(a.dimension() >= naxes);
//            XTENSOR_ASSERT(b.dimension() >= naxes);
//
//            auto as_it = a.shape().begin() + (a.dimension() - naxes);
//            auto bs_it = b.shape().begin();
//            std::size_t sum_len = 1;
//            for (std::size_t i = 0; i < naxes; ++i)
//            {
//                auto a_val = *as_it;
//                auto b_val = *bs_it;
//                // check for axes size match
//                if (a_val != b_val)
//                {
//                    XTENSOR_THROW(std::runtime_error, "Shape mismatch for sum");
//                }
//                else
//                {
//                    sum_len *= a_val;
//                }
//                ++as_it;
//                ++bs_it;
//            }
//            xt::dynamic_shape<std::size_t> result_shape;
//            std::size_t keep_a_len = 1;
//            for (auto it = a.shape().begin(); it != a.shape().begin() + (a.dimension() - naxes); ++it)
//            {
//                std::size_t len = *it;
//                keep_a_len *= len;
//                result_shape.push_back(len);
//            }
//            std::size_t keep_b_len = 1;
//            for (auto it = b.shape().begin() + naxes; it != b.shape().end(); ++it)
//            {
//                std::size_t len = *it;
//                keep_b_len *= len;
//                result_shape.push_back(len);
//            }
//            xarray<value_type, T::static_layout> a_mat = a;
//            a_mat.reshape({keep_a_len, sum_len});
//            xarray<value_type, O::static_layout> b_mat = b;
//            b_mat.reshape({sum_len, keep_b_len});
//            result = dot(a_mat, b_mat);
//            if(result_shape.empty())
//            {
//                result.reshape({1});
//            }
//            else
//            {
//                result.reshape(result_shape);
//            }
//
//        }
//        return result;
//    }
}