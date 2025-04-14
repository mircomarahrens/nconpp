// Copyright 2023 Mirco Marahrens

#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include "TensorNetwork.hpp"

using ::testing::ElementsAre;

/**
 * @brief Create a pair consists of randomize tensors (first) and corresponding
 * descending leg indices (second) from a given vector of shapes.
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
        for (auto i : shape)
        {
            tlegs.push_back(leg);
            leg--;
        }

        legs.emplace_back(tlegs);
    }

    return std::make_pair(tensorList, legs);
}

class TensorNetworkTest : public testing::Test
{
    TensorNetworkTest() = default;

    ~TensorNetworkTest() override = default;
};

TEST(TensorNetworkTest, explicitCopyConstructor)
{
    auto vals = createTensorList({{2, 3, 1}, {3, 1, 2}});

    TensorNetwork<> tn(vals.first, vals.second);

    ASSERT_THAT(tn.DanglingLegs(), ElementsAre(-6, -5, -4, -3, -2, -1));
    ASSERT_THAT(tn.Legs(), ElementsAre());
    ASSERT_THAT(tn.TensorShapes(), ElementsAre(npp::shape_type({2, 3, 1}), npp::shape_type({3, 1, 2})));
}

TEST(TensorNetworkTest, copyConstructor)
{
    auto vals = createTensorList({{2, 3, 1}, {3, 1, 2}});

    TensorNetwork<> tn1(vals.first, vals.second);

    // call copy constructor
    TensorNetwork<> tn2(tn1);

    ASSERT_EQ(tn1.DanglingLegs(), tn2.DanglingLegs());
    ASSERT_EQ(tn1.Legs(), tn2.Legs());
    ASSERT_EQ(tn1.TensorShapes(), tn2.TensorShapes());
}

TEST(TensorNetworkTest, copyAssignment)
{
    auto vals = createTensorList({{2, 3, 1}, {3, 1, 2}});

    TensorNetwork<> tn1(vals.first, vals.second);

    // call copy assignment
    TensorNetwork<> tn2 = tn1;

    ASSERT_EQ(tn1.DanglingLegs(), tn2.DanglingLegs());
    ASSERT_EQ(tn1.Legs(), tn2.Legs());
    ASSERT_EQ(tn1.TensorShapes(), tn2.TensorShapes());
}

TEST(TensorNetworkTest, moveConstructor)
{
    auto vals = createTensorList({{2, 3, 1}, {3, 1, 2}});

    TensorNetwork<> tn1(vals.first, vals.second);

    ASSERT_THAT(tn1.DanglingLegs(), ElementsAre(-6, -5, -4, -3, -2, -1));
    ASSERT_THAT(tn1.Legs(), ElementsAre());
    ASSERT_THAT(tn1.TensorShapes(), ElementsAre(npp::shape_type({2, 3, 1}), npp::shape_type({3, 1, 2})));

    // call move constructor
    TensorNetwork<> tn2(std::move(tn1));

    ASSERT_THAT(tn2.DanglingLegs(), ElementsAre(-6, -5, -4, -3, -2, -1));
    ASSERT_THAT(tn2.Legs(), ElementsAre());
    ASSERT_THAT(tn2.TensorShapes(), ElementsAre(npp::shape_type({2, 3, 1}), npp::shape_type({3, 1, 2})));

    ASSERT_THAT(tn1.DanglingLegs(), ElementsAre());
    ASSERT_THAT(tn1.Legs(), ElementsAre());
    ASSERT_THAT(tn1.TensorShapes(), ElementsAre());
}

TEST(TensorNetworkTest, moveAssignment)
{
    auto vals = createTensorList({{2, 3, 1}, {3, 1, 2}});

    TensorNetwork<> tn1(vals.first, vals.second);

    ASSERT_THAT(tn1.DanglingLegs(), ElementsAre(-6, -5, -4, -3, -2, -1));
    ASSERT_THAT(tn1.Legs(), ElementsAre());
    ASSERT_THAT(tn1.TensorShapes(), ElementsAre(npp::shape_type({2, 3, 1}), npp::shape_type({3, 1, 2})));

    // call move assignment
    TensorNetwork<> tn2 = std::move(tn1);

    ASSERT_THAT(tn2.DanglingLegs(), ElementsAre(-6, -5, -4, -3, -2, -1));
    ASSERT_THAT(tn2.Legs(), ElementsAre());
    ASSERT_THAT(tn2.TensorShapes(), ElementsAre(npp::shape_type({2, 3, 1}), npp::shape_type({3, 1, 2})));

    ASSERT_THAT(tn1.DanglingLegs(), ElementsAre());
    ASSERT_THAT(tn1.Legs(), ElementsAre());
    ASSERT_THAT(tn1.TensorShapes(), ElementsAre());
}

TEST(TensorNetworkTest, explicitMoveConstructor)
{
    std::vector<npp::shape_type> shapes = {{2, 3, 1}, {3, 1, 2}};

    std::vector<npp::tensor_type<std::complex<double>>> tensorList = {};
    std::vector<std::vector<int>> legs = {};
    int leg = -1;
    for (auto shape : shapes)
    {
        // tensor
        tensorList.emplace_back(xt::random::rand<double>(shape));

        // legs
        std::vector<int> tlegs = {};
        for (auto i : shape)
        {
            tlegs.push_back(leg);
            leg--;
        }

        legs.emplace_back(tlegs);
    }

    // call explicit move constructor
    TensorNetwork<> tn(std::move(tensorList), std::move(legs));

    ASSERT_THAT(tn.DanglingLegs(), ElementsAre(-6, -5, -4, -3, -2, -1));
    ASSERT_THAT(tn.Legs(), ElementsAre());
    ASSERT_THAT(tn.TensorShapes(), ElementsAre(npp::shape_type({2, 3, 1}), npp::shape_type({3, 1, 2})));
}

TEST(TensorNetworkTest, logicError_MoreThanTwoLegs)
{
    std::vector<npp::tensor_type<std::complex<double>>> tensorList = {
        npp::tensor_type<std::complex<double>>({3, 4, 5}),
        npp::tensor_type<std::complex<double>>({5, 3, 6, 7, 6}),
        npp::tensor_type<std::complex<double>>({7, 2}),
    };

    std::vector<std::vector<int>> legLinks = {
        {3, -2, 2},
        {2, 3, 1, 3, 1},
        {4, -1},
    };

    EXPECT_THROW(
        try { TensorNetwork tn(std::move(tensorList), legLinks); } catch (const std::invalid_argument &ex) {
            EXPECT_STREQ(ErrorMessages::ERROR_EDGEID_PRESENT, ex.what());
            throw;
        },
        std::invalid_argument);
}

TEST(TensorNetworkTest, copy_constructed_contract)
{
    std::vector<npp::tensor_type<std::complex<double>>> tensorList = {
        xt::random::rand<double>({3, 4, 5}), xt::random::rand<double>({5, 3, 6, 7, 6}),
        xt::random::rand<double>({7, 2}),    xt::random::rand<double>({8}),
        xt::random::rand<double>({8, 9}),    xt::random::rand<double>({9, 9}),
    };

    std::vector<std::vector<int>> legLinks = {
        {3, -2, 2}, {2, 3, 1, 4, 1}, {4, -1}, {5}, {5, -3}, {6, 6},
    };

    TensorNetwork tn(tensorList, legLinks);

    tn.contract();

    auto nt = tn.NumTensors();

    ASSERT_EQ(nt, 3);

    ASSERT_EQ(tn.vertices[0].tensor.shape(), npp::shape_type({4, 2}));
    ASSERT_EQ(tn.vertices[3].tensor.shape(), npp::shape_type({9}));
    ASSERT_EQ(tn.vertices[5].tensor.shape(), npp::shape_type({}));
}

TEST(TensorNetworkTest, move_constructed_contract)
{
    std::vector<npp::shape_type> shapes = {
        {3, 4, 5}, {5, 3, 6, 7, 6}, {7, 2}, {8}, {8, 9}, {9, 9},
    };

    TensorNetwork tn(createTensorList(shapes).first, {
                                                         {3, -2, 2},
                                                         {2, 3, 1, 4, 1},
                                                         {4, -1},
                                                         {5},
                                                         {5, -3},
                                                         {6, 6},
                                                     });

    tn.contract();

    auto nt = tn.NumTensors();

    ASSERT_EQ(nt, 3);

    ASSERT_EQ(tn.vertices[0].tensor.shape(), npp::shape_type({4, 2}));
    ASSERT_EQ(tn.vertices[3].tensor.shape(), npp::shape_type({9}));
    ASSERT_EQ(tn.vertices[5].tensor.shape(), npp::shape_type({}));
}

TEST(TensorNetworkTest, connect)
{
    std::vector<npp::shape_type> shapes = {
        {3, 4, 5}, {5, 3, 6, 7, 6}, {7, 2}, {8}, {8, 9}, {9, 9},
    };

    TensorNetwork tn(createTensorList(shapes).first, {
                                                         {3, -2, 2},
                                                         {2, 3, 1, 4, 1},
                                                         {4, -1},
                                                         {5},
                                                         {5, -3},
                                                         {6, 6},
                                                     });

    tn.contract();

    auto nt = tn.NumTensors();

    ASSERT_EQ(nt, 3);

    ASSERT_EQ(tn.vertices[0].tensor.shape(), npp::shape_type({4, 2}));
    ASSERT_EQ(tn.vertices[3].tensor.shape(), npp::shape_type({9}));
    ASSERT_EQ(tn.vertices[5].tensor.shape(), npp::shape_type({}));

    tn.connect();

    nt = tn.NumTensors();

    ASSERT_EQ(nt, 1);

    ASSERT_EQ(tn.vertices[0].tensor.shape(), npp::shape_type({4, 2, 9}));
}

TEST(TensorNetworkTest, split)
{
    std::vector<npp::shape_type> shapes = {
        {3, 4, 5}, {5, 3, 6, 7, 6}, {7, 2}, {8}, {8, 9}, {9, 9},
    };

    TensorNetwork tn(createTensorList(shapes).first, {
                                                         {3, -2, 2},
                                                         {2, 3, 1, 4, 1},
                                                         {4, -1},
                                                         {5},
                                                         {5, -3},
                                                         {6, 6},
                                                     });

    ASSERT_THAT(tn.Legs(), ElementsAre(1, 2, 3, 4, 5, 6));
    ASSERT_THAT(tn.DanglingLegs(), ElementsAre(-3, -2, -1));

    tn.contract();

    auto nt = tn.NumTensors();

    ASSERT_EQ(nt, 3);

    ASSERT_EQ(tn.vertices[0].tensor.shape(), npp::shape_type({4, 2}));
    ASSERT_EQ(tn.vertices[3].tensor.shape(), npp::shape_type({9}));
    ASSERT_EQ(tn.vertices[5].tensor.shape(), npp::shape_type({}));

    tn.outer(0, 3);
    ASSERT_EQ(tn.vertices[0].tensor.shape(), npp::shape_type({4, 2, 9}));

    tn.outer(0, 5);
    ASSERT_EQ(tn.vertices[0].tensor.shape(), npp::shape_type({4, 2, 9}));

    nt = tn.NumTensors();

    ASSERT_EQ(nt, 1);

    auto tensor = tn.vertices[0].tensor;

    // new test starts here
    tn.split(0, 1);

    nt = tn.NumTensors();

    ASSERT_EQ(nt, 2);

    ASSERT_EQ(tn.vertices[0].tensor.shape(), npp::shape_type({4, 4}));
    // ASSERT_EQ(tn.vertices[1].tensor.shape(), npp::shape_type({4}));
    ASSERT_EQ(tn.vertices[1].tensor.shape(), npp::shape_type({4, 2, 9}));

    auto U = tn.vertices[0].tensor;
    auto s = tn.edges[1].singular_values;
    auto V = tn.vertices[1].tensor;

    npp::tensor_type<std::complex<double>> smat = npp::diag(s);

    auto Us = npp::linalg::dot(U, smat);
    ASSERT_EQ(Us.shape(), npp::shape_type({4, 4}));
    auto result = npp::linalg::tensordot(Us, V, {1}, {0});
    ASSERT_EQ(result.shape(), npp::shape_type({4, 2, 9}));
    ASSERT_TRUE(npp::allclose(tensor, result));

    // recontract splitted network
    tn.contract();

    nt = tn.NumTensors();
    ASSERT_EQ(nt, 1);

    ASSERT_EQ(tn.vertices[0].tensor.shape(), npp::shape_type({4, 2, 9}));
    ASSERT_TRUE(npp::allclose(tensor, tn.vertices[0].tensor));
}
