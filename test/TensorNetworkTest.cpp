#include <gtest/gtest.h>

#include "TensorNetwork.h"

static auto createTensorList(const std::vector<npp::shape_type> &shapes)
{
    std::vector<npp::tensor_type<std::complex<double>>> tensorList = {};
    std::vector<std::vector<int>> legs = {};
    int leg = -1;
    for (auto shape : shapes)
    {
        // tensor
        tensorList.emplace_back(xt::random::rand<double>(shape));

        // legs
        std::vector<int> tlegs = {};
        for (auto i : shapes)
        {
            tlegs.push_back(leg);
            leg--;
        }
    }

    return std::make_pair<>(tensorList, legs);
};

class TensorNetworkTest : public testing::Test
{
    TensorNetworkTest() = default;

    ~TensorNetworkTest() override = default;
};

TEST(TensorNetworkTest, logicError_MoreThanTwoLegs)
{
    std::vector<npp::tensor_type<std::complex<double>>> tensorList =
        {
            npp::tensor_type<std::complex<double>>({3, 4, 5}),
            npp::tensor_type<std::complex<double>>({5, 3, 6, 7, 6}),
            npp::tensor_type<std::complex<double>>({7, 2}),
        };

    std::vector<std::vector<int>> legLinks =
        {
            {3, -2, 2},
            {2, 3, 1, 3, 1},
            {4, -1},
        };

    EXPECT_THROW(
        try {
            TensorNetwork tn(std::move(tensorList), legLinks);
        } catch (const std::invalid_argument &ex) {
            EXPECT_EQ(ERROR_MESSAGE::EDGE_INDEX_PRESENT, ex.what());
            throw;
        },
        std::invalid_argument);
}

TEST(TensorNetworkTest, copy_constructed_contract)
{
    std::vector<npp::tensor_type<std::complex<double>>> tensorList =
        {
            xt::random::rand<double>({3, 4, 5}),
            xt::random::rand<double>({5, 3, 6, 7, 6}),
            xt::random::rand<double>({7, 2}),
            xt::random::rand<double>({8}),
            xt::random::rand<double>({8, 9}),
            xt::random::rand<double>({9, 9}),
        };

    std::vector<std::vector<int>> legLinks =
        {
            {3, -2, 2},
            {2, 3, 1, 4, 1},
            {4, -1},
            {5},
            {5, -3},
            {6, 6},
        };

    TensorNetwork tn(tensorList, legLinks);

    tn.contract();

    auto nt = tn.numTensors();

    ASSERT_TRUE(nt == 3);

    tn.connect();

    nt = tn.numTensors();

    ASSERT_TRUE(nt == 1);

    tensorList = tn.getTensorList();

    npp::shape_type shape = {4, 2, 9};
    ASSERT_EQ(tensorList[0].shape(), shape);
}

TEST(TensorNetworkTest, move_constructed_contract)
{
    std::vector<npp::shape_type> shapes =
        {
            {3, 4, 5},
            {5, 3, 6, 7, 6},
            {7, 2},
            {8},
            {8, 9},
            {9, 9},
        };

    TensorNetwork tn(
        createTensorList(shapes).first,
        {
            {3, -2, 2},
            {2, 3, 1, 4, 1},
            {4, -1},
            {5},
            {5, -3},
            {6, 6},
        });

    tn.contract();

    auto nt = tn.numTensors();

    ASSERT_TRUE(nt == 3);

    tn.connect();

    nt = tn.numTensors();

    ASSERT_TRUE(nt == 1);

    auto tensorList = tn.getTensorList();

    npp::shape_type shape = {4, 2, 9};
    ASSERT_EQ(tensorList[0].shape(), shape);
}
