#include <gtest/gtest.h>

#include "TensorNetwork.h"
#include "TensorNetwork.cpp"

class TensorNetworkTest : public testing::Test {
    TensorNetworkTest() = default;

    ~TensorNetworkTest() override = default;
};

TEST(TensorNetworkTest, tensorNetwork) {
//    std::vector<xt::xarray<std::complex<double>>> tensorList =
//            {
//                    xt::random::rand<double>({3, 4, 5}),
//                    xt::random::rand<double>({5, 3, 6, 7, 6}),
//                    xt::random::rand<double>({7, 2}),
//                    xt::random::rand<double>({8}),
//                    xt::random::rand<double>({8, 9}),
//                    xt::random::rand<double>({9, 9})
//            };
//
//    std::vector<std::vector<int>> legLinks =
//            {
//                    {3, -2, 2},
//                    {2, 3,  1, 4, 1},
//                    {4, -1},
//                    {5},
//                    {5, -3},
//                    {6, 6}
//            };
//
//    TensorNetwork tensorNetwork{tensorList, legLinks};
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
