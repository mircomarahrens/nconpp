#include <gtest/gtest.h>

#include "Tensor.h"

#include <xtensor/xarray.hpp>
#include <xtensor/xrandom.hpp>
#include "xtensor-blas/xblas.hpp"
#include "xtensor-blas/xlapack.hpp"
#include "xtensor-blas/xlinalg.hpp"

class TensorTest : public testing::Test {
    TensorTest() = default;

    ~TensorTest() override = default;
};

TEST(TensorTest, tensordot) {
    auto A = xt::random::rand<double>({4, 3, 8});
    auto B = xt::random::rand<double>({2, 3, 1, 3, 3});

    auto C = Tensor::tensordot(A, B, {1}, {1});
    Tensor::shape_type shapeC = {4, 8, 2, 1, 3, 3};
    ASSERT_EQ(Tensor::shape(C), shapeC);

    Tensor::array_type<std::complex<double>> D = Tensor::trace(C, 0, 4, 5);

    Tensor::shape_type shapeD = {4, 8, 2, 1};
    ASSERT_EQ(Tensor::shape(D), shapeD);
}

TEST(TensorTest, svd) {
    xt::xarray<double> arg_0 = {{0,1,2},
                                {3,4,5},
                                {6,7,8}};

    auto res = Tensor::svd(arg_0);

//ASSERT_EQ(Tensor::shape(D), shapeD);
}