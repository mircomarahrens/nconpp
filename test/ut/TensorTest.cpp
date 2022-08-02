#include <gtest/gtest.h>

#include "Tensor.h"
#include "Tensor.cpp"

#include <complex>
#include <random>

class TensorTest : public testing::Test {
    TensorTest() = default;

    ~TensorTest() override = default;
};

TEST(TensorTest, tensorWithoutData) {
    std::vector<int> s = {4, 3, 8};
    Tensor<double> A({4, 3, 8});
    ASSERT_EQ(A.shape(), s);
}

TEST(TensorTest, tensorFromRandomData) {
    std::vector<int> s = {4, 3, 8};

    std::vector<double> data(4 * 3 * 8);

    std::default_random_engine gen{std::random_device{}()};
    std::uniform_real_distribution<double> dist(0, 1);
    std::generate(std::begin(data), std::end(data), [&] { return dist(gen); });

    Tensor<double> A(data, {4, 3, 8});
    ASSERT_EQ(A.shape(), s);
    ASSERT_EQ(A.getData(), data);
    ASSERT_EQ(A.getData().size(), 4 * 3 * 8);
}

TEST(TensorTest, tensorRandomizeData) {
    std::vector<int> s = {4, 3, 8};
    Tensor<std::complex<double>> A({4, 3, 8});
    A.randomize();
    ASSERT_EQ(A.shape(), s);
    ASSERT_EQ(A.getData().size(), 4 * 3 * 8);
}


TEST(TensorTest, tensordot) {
//    auto A = xt::random::rand<double>({4, 3, 8});
//    auto B = xt::random::rand<double>({2, 3, 1, 3, 3});
//
//    auto C = TensorOperations::tensordot(A, B, {1}, {1});
//    TensorOperations::shape_type shapeC = {4, 8, 2, 1, 3, 3};
//    ASSERT_EQ(TensorOperations::Shape(C), shapeC);
//
//    TensorOperations::array_type<std::complex<double>> D = TensorOperations::trace(C, 0, 4, 5);
//
//    TensorOperations::shape_type shapeD = {4, 8, 2, 1};
//    ASSERT_EQ(TensorOperations::Shape(D), shapeD);
}

TEST(TensorTest, svd) {
//    // from https://github.com/xtensor-stack/xtensor-blas/blob/master/test/test_linalg.cpp
//    xt::xarray<double> arg_0 = {{0, 1, 2},
//                                {3, 4, 5},
//                                {6, 7, 8}};
//
//    auto res = TensorOperations::svd(arg_0);
//
//    xt::xarray<double, xt::layout_type::column_major> expected_0 = {{-0.13511895, 0.90281571,  0.40824829},
//                                                                    {-0.49633514, 0.29493179,  -0.81649658},
//                                                                    {-0.85755134, -0.31295213, 0.40824829}};
//    xt::xarray<double, xt::layout_type::column_major> expected_1 = {1.42267074e+01, 1.26522599e+00, 5.89938022e-16};
//    xt::xarray<double, xt::layout_type::column_major> expected_2 = {{-0.4663281,  -0.57099079, -0.67565348},
//                                                                    {-0.78477477, -0.08545673, 0.61386131},
//                                                                    {-0.40824829, 0.81649658,  -0.40824829}};
//
//    EXPECT_TRUE(allclose(std::get<0>(res), expected_0));
//    EXPECT_TRUE(allclose(std::get<1>(res), expected_1));
//    EXPECT_TRUE(allclose(std::get<2>(res), expected_2));
}