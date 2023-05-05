#include <gtest/gtest.h>

#include "TensorNetwork.h"

/**
 * @brief Create a pair consists of randomize tensors and corresponding descending leg indices
 * from a given vector of shapes.
 * 
 * @param shapes 
 * @return auto 
 */
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

    tensorList = tn.getTensorList();

    // checks before connect
    ASSERT_TRUE(tensorList.size() == 3);
    ASSERT_EQ(tensorList[0].shape(), npp::shape_type({4, 2}));
    ASSERT_EQ(tensorList[1].shape(), npp::shape_type({9}));
    ASSERT_EQ(tensorList[2].shape(), npp::shape_type({}));

    tn.connect();

    nt = tn.numTensors();

    ASSERT_TRUE(nt == 1);

    tensorList = tn.getTensorList();

    ASSERT_TRUE(tensorList.size() == 1);
    ASSERT_EQ(tensorList[0].shape(), npp::shape_type({4, 2, 9}));
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

    auto tensorList = tn.getTensorList();

    // checks before connect
    ASSERT_TRUE(tensorList.size() == 3);
    ASSERT_EQ(tensorList[0].shape(), npp::shape_type({4, 2}));
    ASSERT_EQ(tensorList[1].shape(), npp::shape_type({9}));
    ASSERT_EQ(tensorList[2].shape(), npp::shape_type({}));

    tn.connect();

    nt = tn.numTensors();

    ASSERT_TRUE(nt == 1);

    tensorList = tn.getTensorList();

    ASSERT_TRUE(tensorList.size() == 1);
    ASSERT_EQ(tensorList[0].shape(), npp::shape_type({4, 2, 9}));
}
