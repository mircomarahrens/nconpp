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
};