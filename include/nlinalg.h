#pragma once

#include <algorithm>
#include "Tensor.h"
#include "nblas.h"

namespace nlinalg {
    /**
 * Compute the tensor product of two tensors a,b along axes ax_a, ax_b.
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

        // determine new axes of tensor a
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

        // determine new axes of tensor b
        std::vector<std::size_t> newaxes_b = {};
        for (std::size_t i = 0; i < b.dimension(); ++i) {

        }

        // scalar multiplication
        if (a.dimension() == 1 and b.dimension() == 1) {
            return (a*b);
        }
        // matrix/vector multiplication
        else if (a.dimension() == 2 and b.dimension() == 2) {
            return dot(a, b, 2);
        }
        // multiply by scalar
        else if (a.dimension() == 1) {
            return dot(b, a, 1);
        }
        else if (b.dimension() == 1) {
            return dot(a, b);
        }
        // tensor product
        else {
            return tdot(a,b);
        }
    }



    template<class T>
    auto tensordot(const Tensor<T>& a, const Tensor<T>& b, const std::vector<std::size_t>& ax_a,
                   const std::vector<std::size_t>& ax_b) {

        std::size_t n_ax = ax_a.size();

        std::vector<std::size_t> newaxes_a = {};
        for (std::size_t i = 0; i < a.dimension(); ++i)
        {
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

        a.transpose(newaxes_a);
        b.transpose(newaxes_b);

        // WIP
        // see https://github.com/xtensor-stack/xtensor-blas/blob/master/include/xtensor-blas/xlinalg.hpp
        return tensordot(a,b,n_ax);
    }

    template <class T, class U>
    auto tensordot(const Tensor<T>& a, const Tensor<U>& b, std::size_t naxes = 2) {

        Tensor<T> result;
        if (naxes == 0)
        {
//            // special case tensor outer product product
//            std::vector<std::size_t> result_shape(a.dimension() + b.dimension());
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
//            auto vec_a = a.getData();
//            auto vec_b = b.getData();
//            // take the outer product of the two vectors
//            result = outer(vec_a, vec_b);
//            // reshape the result
//            result.reshape(result_shape);
        }
        else
        {
            // Sum of products over last n axes of A and the first n axis of b
//            XTENSOR_ASSERT(a.dimension() >= naxes);
//            XTENSOR_ASSERT(b.dimension() >= naxes);

            auto as_it = a.shape().begin() + (a.dimension() - naxes);
            auto bs_it = b.shape().begin();
            std::size_t sum_len = 1;
            for (std::size_t i = 0; i < naxes; ++i)
            {
                auto a_val = *as_it;
                auto b_val = *bs_it;
                // check for axes size match
                if (a_val != b_val)
                {
                    //XTENSOR_THROW(std::runtime_error, "Shape mismatch for sum");
                }
                else
                {
                    sum_len *= a_val;
                }
                ++as_it;
                ++bs_it;
            }
            std::vector<std::size_t> result_shape;
            std::size_t keep_a_len = 1;
            for (auto it = a.shape().begin(); it != a.shape().begin() + (a.dimension() - naxes); ++it)
            {
                std::size_t len = *it;
                keep_a_len *= len;
                result_shape.push_back(len);
            }
            std::size_t keep_b_len = 1;
            for (auto it = b.shape().begin() + naxes; it != b.shape().end(); ++it)
            {
                std::size_t len = *it;
                keep_b_len *= len;
                result_shape.push_back(len);
            }
            Tensor<T> a_mat = a;
            a_mat.reshape({keep_a_len, sum_len});
            Tensor<U> b_mat = b;
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

};