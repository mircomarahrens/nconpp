// Copyright 2026 Mirco Marahrens

#include <gtest/gtest.h>

#include <vector>

#include "nconpp/LinAlg.hpp"

namespace {
template <typename T>
auto reconstruct_from_hosvd(const std::vector<npp::tensor_type<T>> &factors,
                            const npp::tensor_type<T> &core) {
    npp::tensor_type<T> reconstructed = core;

    for (std::size_t mode = 0; mode < factors.size(); ++mode) {
        npp::tensor_type<T> contracted =
            xt::linalg::tensordot(factors[mode], reconstructed, {1}, {mode});

        auto permutation = std::vector<std::size_t>{};
        permutation.reserve(reconstructed.dimension());
        for (std::size_t axis = 0; axis < mode; ++axis) {
            permutation.push_back(axis + 1);
        }
        permutation.push_back(0);
        for (std::size_t axis = mode + 1; axis < reconstructed.dimension(); ++axis) {
            permutation.push_back(axis);
        }

        reconstructed = npp::tensor_type<T>(xt::transpose(contracted, permutation));
    }

    return reconstructed;
}
} // namespace

TEST(LinAlgTest, hosvd_full_rank_reconstructs_tensor) {
    npp::tensor_type<double> tensor = {
        {{1.0, 2.0, 3.0, 4.0}, {5.0, 6.0, 7.0, 8.0}, {9.0, 10.0, 11.0, 12.0}},
        {{13.0, 14.0, 15.0, 16.0}, {17.0, 18.0, 19.0, 20.0}, {21.0, 22.0, 23.0, 24.0}}};

    auto [factors, core] = npp::linalg::hosvd(tensor);

    ASSERT_EQ(factors.size(), 3);
    ASSERT_EQ(factors[0].shape(), npp::shape_type({2, 2}));
    ASSERT_EQ(factors[1].shape(), npp::shape_type({3, 3}));
    ASSERT_EQ(factors[2].shape(), npp::shape_type({4, 4}));
    ASSERT_EQ(core.shape(), npp::shape_type({2, 3, 4}));

    for (const auto &factor : factors) {
        auto gram = npp::linalg::dot(xt::transpose(factor), factor);
        auto identity = xt::eye<double>(factor.shape()[1]);
        ASSERT_TRUE(npp::allclose(gram, identity, 1e-8, 1e-8));
    }

    auto reconstructed = reconstruct_from_hosvd(factors, core);
    ASSERT_TRUE(npp::allclose(reconstructed, tensor, 1e-8, 1e-8));
}

TEST(LinAlgTest, hosvd_rank_truncation_limits_factor_columns) {
    npp::tensor_type<double> tensor = {
        {{1.0, 2.0, 3.0, 4.0}, {5.0, 6.0, 7.0, 8.0}, {9.0, 10.0, 11.0, 12.0}},
        {{13.0, 14.0, 15.0, 16.0}, {17.0, 18.0, 19.0, 20.0}, {21.0, 22.0, 23.0, 24.0}}};

    auto [factors, core] = npp::linalg::hosvd(tensor, 2);

    ASSERT_EQ(factors.size(), 3);
    ASSERT_EQ(factors[0].shape(), npp::shape_type({2, 2}));
    ASSERT_EQ(factors[1].shape(), npp::shape_type({3, 2}));
    ASSERT_EQ(factors[2].shape(), npp::shape_type({4, 2}));
    ASSERT_EQ(core.shape(), npp::shape_type({2, 2, 2}));
}
