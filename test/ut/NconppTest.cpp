#include <gtest/gtest.h>

#include "Nconpp.h"
#include "Nconpp.cpp"

#include "Container.h"
#include "ErrorMessages.h"
#include "Tensor.h"

#include <xtensor/xarray.hpp>
#include <xtensor/xrandom.hpp>
#include <xtensor-blas/xlinalg.hpp>

#include <algorithm>
#include <complex>
#include <iterator>
#include <numeric>
#include <set>

class NconppTest : public testing::Test {
    NconppTest() = default;

    ~NconppTest() override = default;
};

TEST(NconppTest, tensorOperations) {
    auto A = xt::random::rand<double>({4, 3, 8});
    auto B = xt::random::rand<double>({2, 3, 1, 3, 3});

    auto C = Tensor::tensordot(A, B, {1}, {1});
    Tensor::shape_type shapeC = {4, 8, 2, 1, 3, 3};
    ASSERT_EQ(Tensor::shape(C), shapeC);

    Tensor::array_type<std::complex<double>> D = Tensor::trace(C, 0, 4, 5);

    Tensor::shape_type shapeD = {4, 8, 2, 1};
    ASSERT_EQ(Tensor::shape(D), shapeD);
}

TEST(NconppTest, logicError_MoreThanTwoLegs) {
    std::vector<Tensor::array_type<std::complex<double>>> tensorList =
            {
                    xt::random::rand<double>({3, 4, 5}),
                    xt::random::rand<double>({5, 3, 6, 7, 6}),
                    xt::random::rand<double>({7, 2})
            };

    std::vector<std::vector<int>> legLinks =
            {
                    {3, -2, 2},
                    {2, 3,  1, 3, 1},
                    {4, -1},
            };

    EXPECT_THROW(
            try {
                Tensor::array_type<std::complex<double>> finalTensor =
                        Nconpp::contract(tensorList, legLinks, false);
            }
            catch (const std::logic_error &ex) {
                EXPECT_EQ(ERROR_MESSAGES::MISMATCH, ex.what());
                throw;
            }, std::logic_error);
}

TEST(NconppTest, contract) {
    std::vector<Tensor::array_type<std::complex<double>>> tensorList =
            {
                    xt::random::rand<double>({3, 4, 5}),
                    xt::random::rand<double>({5, 3, 6, 7, 6}),
                    xt::random::rand<double>({7, 2}),
                    xt::random::rand<double>({8}),
                    xt::random::rand<double>({8, 9}),
                    xt::random::rand<double>({9, 9})
            };

    std::vector<std::vector<int>> legLinks =
            {
                    {3, -2, 2},
                    {2, 3,  1, 4, 1},
                    {4, -1},
                    {5},
                    {5, -3},
                    {6, 6}
            };

    Tensor::array_type<std::complex<double>> finalTensor =
            Nconpp::contract(tensorList, legLinks, false);

    Tensor::shape_type shape = {2, 4, 9};
    ASSERT_EQ(Tensor::shape(finalTensor), shape);
}

// TODO
TEST(NconppTest, DISABLED_decompose) {
    Tensor::array_type<std::complex<double>> tensor =
            xt::random::rand<double>({2, 4, 9});

    Nconpp::decompose(tensor, 1);
}