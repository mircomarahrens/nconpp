#include <gtest/gtest.h>

#include "TensorNetwork.h"

static auto createTensorList(const std::vector<npp::shape_type> &shapes)
{
        std::vector<npp::tensor<std::complex<double>>> tensorList = {};
        std::vector<std::vector<int>> legs = {};
        int leg = -1;
        for (auto shape : shapes)
        {
                // tensor
                tensorList.emplace_back(xt::random::rand<double>(shape));

                // legs
                std::vector<int> tlegs = {}; 
                for (auto i: shapes) {
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

TEST(TensorNetworkTest, Graph)
{
        struct vertex_properties
        {
                std::vector<int> legs = {1, 2, 3};
        };

        typedef boost::adjacency_list<
            boost::vecS,
            boost::vecS,
            boost::undirectedS,
            vertex_properties,
            boost::property<boost::edge_index_t, std::size_t>>
            graph_t;

        typedef boost::graph_traits<graph_t>::edge_descriptor edge_d;
        typedef boost::property_map<graph_t, boost::edge_index_t>::type edge_index_pm;
        typedef boost::graph_traits<graph_t>::edge_iterator edge_it;
        typedef boost::graph_traits<graph_t>::vertex_descriptor vertex_d;
        typedef boost::graph_traits<graph_t>::vertex_iterator vertex_it;
        typedef boost::graph_traits<graph_t>::adjacency_iterator adjacency_it;

        std::size_t N = 5;
        graph_t G(N);

        ASSERT_TRUE(boost::num_vertices(G) == N);

        typedef std::pair<int, int> Pair;
        Pair edge_array[11] = {
            Pair(0, 1),
            Pair(0, 2),
            Pair(0, 3),
            Pair(0, 4),
            Pair(2, 0),
            Pair(3, 0),
            Pair(2, 4),
            Pair(3, 1),
            Pair(3, 4),
            Pair(4, 0),
            Pair(4, 1),
        };

        int NE = 11;
        for (int i = 0; i < NE; ++i)
        {
                auto e = boost::add_edge(edge_array[i].first, edge_array[i].second, i + 1, G);
                ASSERT_TRUE(e.second == true);
        }
        ASSERT_TRUE(boost::num_edges(G) == NE);
}

TEST(TensorNetworkTest, logicError_MoreThanTwoLegs)
{
        std::vector<npp::tensor<std::complex<double>>> tensorList =
            {
                npp::tensor<std::complex<double>>({3, 4, 5}),
                npp::tensor<std::complex<double>>({5, 3, 6, 7, 6}),
                npp::tensor<std::complex<double>>({7, 2}),
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
            } catch (const std::logic_error &ex) {
                    EXPECT_EQ(ERROR::CONSTRAINT_LEGPAIRS, ex.what());
                    throw;
            },
            std::logic_error);
}

TEST(TensorNetworkTest, copy_constructed_contract)
{
        std::vector<npp::tensor<std::complex<double>>> tensorList =
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

        auto nt = tn.num_tensors();

        ASSERT_TRUE(nt == 3);

        tn.connect();

        nt = tn.num_tensors();

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

        auto nt = tn.num_tensors();

        ASSERT_TRUE(nt == 3);

        tn.connect();

        nt = tn.num_tensors();

        ASSERT_TRUE(nt == 1);

        auto tensorList = tn.getTensorList();

        npp::shape_type shape = {4, 2, 9};
        ASSERT_EQ(tensorList[0].shape(), shape);
}

TEST(TensorTest, svd)
{
        //    // from https://github.com/xtensor-stack/xtensor-blas/blob/master/test/test_linalg.cpp
        //    xt::xarray<double> arg_0 = {{0, 1, 2},
        //                                {3, 4, 5},
        //                                {6, 7, 8}};
        
        //    auto res = TensorOperations::svd(arg_0);
        
        //    xt::xarray<double, xt::layout_type::column_major> expected_0 = {{-0.13511895, 0.90281571,  0.40824829},
        //                                                                    {-0.49633514, 0.29493179,  -0.81649658},
        //                                                                    {-0.85755134, -0.31295213, 0.40824829}};
        //    xt::xarray<double, xt::layout_type::column_major> expected_1 = {1.42267074e+01, 1.26522599e+00, 5.89938022e-16};
        //    xt::xarray<double, xt::layout_type::column_major> expected_2 = {{-0.4663281,  -0.57099079, -0.67565348},
        //                                                                    {-0.78477477, -0.08545673, 0.61386131},
        //                                                                    {-0.40824829, 0.81649658,  -0.40824829}};
        
        //    EXPECT_TRUE(allclose(std::get<0>(res), expected_0));
        //    EXPECT_TRUE(allclose(std::get<1>(res), expected_1));
        //    EXPECT_TRUE(allclose(std::get<2>(res), expected_2));
}
