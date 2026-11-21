// Copyright 2023 Mirco Marahrens

#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include "nconpp/TensorNetwork.hpp"

using ::testing::ElementsAre;

/**
 * @brief Create a pair consists of randomize tensors (first) and corresponding
 * descending leg indices (second) from a given vector of shapes.
 *
 * @param shapes
 * @return auto
 */
static auto createTensorList(const std::vector<npp::shape_type> &shapes) {
    std::vector<npp::tensor_type<std::complex<double>>> tensorList = {};
    std::vector<std::vector<int>> legs = {};
    int leg = -1;
    for (const auto &shape : shapes) {
        // tensor
        tensorList.emplace_back(xt::random::rand<double>(shape));

        // legs
        std::vector<int> tlegs = {};
        for (std::size_t d = 0; d < shape.size(); ++d) {
            tlegs.push_back(leg);
            leg--;
        }

        legs.emplace_back(tlegs);
    }

    return std::make_pair(tensorList, legs);
}

TEST(TensorNetworkTest, explicitCopyConstructor) {
    auto vals = createTensorList({{2, 3, 1}, {3, 1, 2}});

    TensorNetwork<> tn(vals.first, vals.second);

    ASSERT_THAT(tn.DanglingLegs(), ElementsAre(-6, -5, -4, -3, -2, -1));
    ASSERT_THAT(tn.Legs(), ElementsAre());
    ASSERT_THAT(tn.TensorShapes(),
                ElementsAre(npp::shape_type({2, 3, 1}), npp::shape_type({3, 1, 2})));
}

TEST(TensorNetworkTest, copyConstructor) {
    auto vals = createTensorList({{2, 3, 1}, {3, 1, 2}});

    TensorNetwork<> tn1(vals.first, vals.second);

    // call copy constructor
    TensorNetwork<> tn2(tn1);

    ASSERT_EQ(tn1.DanglingLegs(), tn2.DanglingLegs());
    ASSERT_EQ(tn1.Legs(), tn2.Legs());
    ASSERT_EQ(tn1.TensorShapes(), tn2.TensorShapes());
}

TEST(TensorNetworkTest, copyAssignment) {
    auto vals = createTensorList({{2, 3, 1}, {3, 1, 2}});
    auto other_vals = createTensorList({{1, 2}, {2, 1}});

    TensorNetwork<> tn1(vals.first, vals.second);
    TensorNetwork<> tn2(other_vals.first, other_vals.second);

    // call copy assignment
    tn2 = tn1;

    ASSERT_EQ(tn1.DanglingLegs(), tn2.DanglingLegs());
    ASSERT_EQ(tn1.Legs(), tn2.Legs());
    ASSERT_EQ(tn1.TensorShapes(), tn2.TensorShapes());
}

TEST(TensorNetworkTest, moveConstructor) {
    auto vals = createTensorList({{2, 3, 1}, {3, 1, 2}});

    TensorNetwork<> tn1(vals.first, vals.second);

    ASSERT_THAT(tn1.DanglingLegs(), ElementsAre(-6, -5, -4, -3, -2, -1));
    ASSERT_THAT(tn1.Legs(), ElementsAre());
    ASSERT_THAT(tn1.TensorShapes(),
                ElementsAre(npp::shape_type({2, 3, 1}), npp::shape_type({3, 1, 2})));

    // call move constructor
    TensorNetwork<> tn2(std::move(tn1));

    ASSERT_THAT(tn2.DanglingLegs(), ElementsAre(-6, -5, -4, -3, -2, -1));
    ASSERT_THAT(tn2.Legs(), ElementsAre());
    ASSERT_THAT(tn2.TensorShapes(),
                ElementsAre(npp::shape_type({2, 3, 1}), npp::shape_type({3, 1, 2})));

    ASSERT_THAT(tn1.DanglingLegs(), ElementsAre());
    ASSERT_THAT(tn1.Legs(), ElementsAre());
    ASSERT_THAT(tn1.TensorShapes(), ElementsAre());
}

TEST(TensorNetworkTest, moveAssignment) {
    auto vals = createTensorList({{2, 3, 1}, {3, 1, 2}});
    auto other_vals = createTensorList({{1, 2}, {2, 1}});

    TensorNetwork<> tn1(vals.first, vals.second);

    ASSERT_THAT(tn1.DanglingLegs(), ElementsAre(-6, -5, -4, -3, -2, -1));
    ASSERT_THAT(tn1.Legs(), ElementsAre());
    ASSERT_THAT(tn1.TensorShapes(),
                ElementsAre(npp::shape_type({2, 3, 1}), npp::shape_type({3, 1, 2})));

    TensorNetwork<> tn2(other_vals.first, other_vals.second);

    // call move assignment
    tn2 = std::move(tn1);

    ASSERT_THAT(tn2.DanglingLegs(), ElementsAre(-6, -5, -4, -3, -2, -1));
    ASSERT_THAT(tn2.Legs(), ElementsAre());
    ASSERT_THAT(tn2.TensorShapes(),
                ElementsAre(npp::shape_type({2, 3, 1}), npp::shape_type({3, 1, 2})));

    ASSERT_THAT(tn1.DanglingLegs(), ElementsAre());
    ASSERT_THAT(tn1.Legs(), ElementsAre());
    ASSERT_THAT(tn1.TensorShapes(), ElementsAre());
}

TEST(TensorNetworkTest, explicitMoveConstructor) {
    auto vals = createTensorList({{2, 3, 1}, {3, 1, 2}});

    // call explicit move constructor
    TensorNetwork<> tn(std::move(vals.first), std::move(vals.second));

    ASSERT_THAT(tn.DanglingLegs(), ElementsAre(-6, -5, -4, -3, -2, -1));
    ASSERT_THAT(tn.Legs(), ElementsAre());
    ASSERT_THAT(tn.TensorShapes(),
                ElementsAre(npp::shape_type({2, 3, 1}), npp::shape_type({3, 1, 2})));
}

TEST(TensorNetworkTest, logicError_MoreThanTwoLegs) {
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
        try {
            TensorNetwork tn(std::move(tensorList), legLinks);
        } catch (const std::invalid_argument &ex) {
            EXPECT_STREQ(ErrorMessages::ERROR_EDGEID_PRESENT, ex.what());
            throw;
        },
        std::invalid_argument);
}

TEST(TensorNetworkTest, copy_constructed_contract) {
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

TEST(TensorNetworkTest, move_constructed_contract) {
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

TEST(TensorNetworkTest, connect) {
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

TEST(TensorNetworkTest, split) {
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

    // recontract split network
    tn.contract();

    nt = tn.NumTensors();
    ASSERT_EQ(nt, 1);

    ASSERT_EQ(tn.vertices[0].tensor.shape(), npp::shape_type({4, 2, 9}));
    ASSERT_TRUE(npp::allclose(tensor, tn.vertices[0].tensor));
}

TEST(TensorNetworkTest, multi_index_contract_matches_single_index) {
    std::vector<npp::tensor_type<std::complex<double>>> tensorList = {
        xt::random::rand<double>({3, 4, 5}), xt::random::rand<double>({5, 3, 6, 7, 6}),
        xt::random::rand<double>({7, 2}),    xt::random::rand<double>({8}),
        xt::random::rand<double>({8, 9}),    xt::random::rand<double>({9, 9}),
    };

    std::vector<std::vector<int>> legLinks = {
        {3, -2, 2}, {2, 3, 1, 4, 1}, {4, -1}, {5}, {5, -3}, {6, 6},
    };

    TensorNetwork tn_single(tensorList, legLinks);
    TensorNetwork tn_multi(tensorList, legLinks);

    tn_single.contract({}, {}, false);
    tn_multi.contract({}, {}, true);

    ASSERT_EQ(tn_multi.NumTensors(), tn_single.NumTensors());
    ASSERT_EQ(tn_multi.getLegs(), tn_single.getLegs());

    for (std::size_t v : tn_single.getVertices()) {
        ASSERT_EQ(tn_multi.vertices[v].tensor.shape(), tn_single.vertices[v].tensor.shape());
        ASSERT_TRUE(npp::allclose(tn_multi.vertices[v].tensor, tn_single.vertices[v].tensor));
    }
}

TEST(TensorNetworkTest, multi_index_contract_full_scalar) {
    npp::tensor_type<std::complex<double>> A = xt::random::rand<double>({4, 2, 3});
    npp::tensor_type<std::complex<double>> B = xt::random::rand<double>({3, 4, 2});

    std::vector<npp::tensor_type<std::complex<double>>> tensorList = {A, B};
    std::vector<std::vector<int>> legLinks = {{3, 1, 2}, {2, 3, 1}};

    TensorNetwork tn_single(tensorList, legLinks);
    TensorNetwork tn_multi(tensorList, legLinks);

    tn_single.contract({}, {}, false);
    tn_multi.contract({}, {}, true);

    ASSERT_EQ(tn_multi.NumTensors(), 1);
    ASSERT_EQ(tn_multi.vertices[0].tensor.shape(), npp::shape_type({}));
    ASSERT_TRUE(npp::allclose(tn_multi.vertices[0].tensor, tn_single.vertices[0].tensor));

    auto expected = npp::linalg::tensordot(A, B, {1, 2, 0}, {2, 0, 1});
    ASSERT_TRUE(npp::allclose(tn_multi.vertices[0].tensor, expected));
}

TEST(TensorNetworkTest, multi_index_contract_partial_sequence) {
    std::vector<npp::tensor_type<std::complex<double>>> tensorList = {
        xt::random::rand<double>({3, 4, 5}),
        xt::random::rand<double>({5, 3, 2}),
    };
    std::vector<std::vector<int>> legLinks = {{2, -1, 1}, {1, 2, -2}};

    TensorNetwork tn(tensorList, legLinks);

    // Only contract leg 1; leg 2 is not in the sequence and must remain uncontracted.
    tn.contract({1}, {}, true);

    ASSERT_EQ(tn.NumTensors(), 1);
    ASSERT_THAT(tn.Legs(), ElementsAre(2));
    ASSERT_EQ(tn.vertices[0].tensor.shape(), npp::shape_type({3, 4, 3, 2}));
}

TEST(TensorNetworkTest, multi_index_contract_with_singular_values) {
    npp::tensor_type<std::complex<double>> A = xt::random::rand<double>({3, 4, 5});
    npp::tensor_type<std::complex<double>> B = xt::random::rand<double>({5, 3, 2});
    npp::tensor_type<std::complex<double>> sv1 = xt::random::rand<double>({5});
    npp::tensor_type<std::complex<double>> sv2 = xt::random::rand<double>({3});

    std::vector<npp::tensor_type<std::complex<double>>> tensorList = {A, B};
    std::vector<std::vector<int>> legLinks = {{2, -1, 1}, {1, 2, -2}};

    TensorNetwork tn_multi(tensorList, legLinks);
    tn_multi.edges[1].singular_values = sv1;
    tn_multi.edges[2].singular_values = sv2;

    tn_multi.contract({}, {}, true);

    // Compute reference: scale A along axis 2 by sv1 and axis 0 by sv2, then contract
    auto A_scaled = A;
    auto sv1_r = sv1;
    sv1_r.reshape({1, 1, 5});
    auto sv2_r = sv2;
    sv2_r.reshape({3, 1, 1});
    A_scaled *= sv1_r;
    A_scaled *= sv2_r;
    auto expected = npp::linalg::tensordot(A_scaled, B, {2, 0}, {0, 1});

    ASSERT_EQ(tn_multi.NumTensors(), 1);
    ASSERT_EQ(tn_multi.vertices[0].tensor.shape(), npp::shape_type({4, 2}));
    ASSERT_TRUE(npp::allclose(tn_multi.vertices[0].tensor, expected));
}
