#include <gtest/gtest.h>

#include "Nconpp.h"
#include "Nconpp.cpp"

#include "ErrorMessages.h"
#include "Tensor.h"

#include <algorithm>
#include <complex>
#include <set>

#include <xtensor/xarray.hpp>
#include <xtensor/xexpression.hpp>
#include <xtensor/xlayout.hpp>
#include <xtensor/xrandom.hpp>
#include <xtensor-blas/xlinalg.hpp>

class NconppTest : public testing::Test {
    NconppTest() = default;

    ~NconppTest() override = default;
};

TEST(NconppTest, tensorOperations) {
    auto A = xt::random::rand<double>({4, 3, 8});
    auto B = xt::random::rand<double>({2, 3, 1, 3, 3});

    auto C = xt::linalg::tensordot(A, B, {1}, {1});
    xt::xarray<std::size_t>::shape_type shapeC = {4, 8, 2, 1, 3, 3};
    ASSERT_EQ(C.shape(), shapeC);

    xt::xarray<std::complex<double>> D = xt::linalg::trace(C, 0, 4, 5);

    xt::xarray<std::size_t>::shape_type shapeD = {4, 8, 2, 1};
    ASSERT_EQ(D.shape(), shapeD);
}

TEST(NconppTest, logicError_MoreThanTwoLegs) {
    std::vector<Tensor<std::complex<double>>> tensorList =
            {
                    Tensor<std::complex<double>>({3, 4, 5}),
                    Tensor<std::complex<double>>({5, 3, 6, 7, 6}),
                    Tensor<std::complex<double>>({7, 2})
            };

    tensorList[0].randomize();
    tensorList[1].randomize();
    tensorList[2].randomize();

    std::vector<std::vector<int>> legLinks =
            {
                    {3, -2, 2},
                    {2, 3,  1, 3, 1},
                    {4, -1},
            };

    EXPECT_THROW(
            try {
                Tensor<std::complex<double>> finalTensor =
                        Nconpp::contract(tensorList, legLinks);
            }
            catch (const std::logic_error &ex) {
                EXPECT_EQ(ERROR_MESSAGES::MISMATCH, ex.what());
                throw;
            }, std::logic_error);
}

TEST(NconppTest, contract) {
    std::vector<Tensor<std::complex<double>>> tensorList =
            {
                    Tensor<std::complex<double>>({3, 4, 5}),
                    Tensor<std::complex<double>>({5, 3, 6, 7, 6}),
                    Tensor<std::complex<double>>({7, 2}),
                    Tensor<std::complex<double>>({8}),
                    Tensor<std::complex<double>>({8, 9}),
                    Tensor<std::complex<double>>({9, 9})
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

    Tensor<std::complex<double>> finalTensor =
            Nconpp::contract(tensorList, legLinks);

    xt::xarray<std::size_t>::shape_type Shape = {2, 4, 9};
    ASSERT_EQ(finalTensor.shape(), Shape);
}

// TODO
//TEST(NconppTest, DISABLED_decompose) {
//    TensorOperations::array_type<std::complex<double>> tensor =
//            xt::random::rand<double>({2, 4, 9});
//
//    Nconpp::decompose(tensor, 1);
//}