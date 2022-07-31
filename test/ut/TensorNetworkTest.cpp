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
