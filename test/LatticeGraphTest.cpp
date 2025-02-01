// Copyright 2023 Mirco Marahrens

#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include "LatticeGraph.hpp"

using ::testing::ElementsAre;
using ::testing::Pair;
using ::testing::UnorderedElementsAre;

class LatticeGraphTest : public testing::Test
{
    LatticeGraphTest() = default;

    ~LatticeGraphTest() override = default;
};

TEST(LatticeGraphTest, Honeycomb)
{
    std::string name = "Honeycomb";
    std::vector<std::size_t> grid_shape = {4, 4};
    std::vector<std::vector<std::vector<int>>> directions = {{{0, -1}, {0, +1}, {+1, 0}}, {{-1, 0}, {0, -1}, {0, +1}}};
    std::vector<std::string> bcs = {"pbc", "pbc"};

    LatticeGraph lattice_graph(name, grid_shape, directions, bcs);

    ASSERT_THAT(lattice_graph.getGridShape(), ElementsAre(4, 4));
    auto bgs = lattice_graph.getBoundaryGridShape();
    ASSERT_THAT(bgs, ElementsAre(6, 6));
    auto _directions = lattice_graph.getDirections();

    for (auto v : lattice_graph.vertices)
    {
        auto cc = npp::unravel_index(v.first, bgs);
        ASSERT_THAT(v.second.coordinate, cc);
        ASSERT_THAT(npp::ravel_index(cc, bgs), v.first);
    }

    ASSERT_THAT(lattice_graph.vertices.size(), 36);
    ASSERT_THAT(lattice_graph.edges.size(), 24);
}

TEST(LatticeGraphTest, default_sameas_Honeycomb)
{
    LatticeGraph lattice_graph;

    ASSERT_THAT(lattice_graph.getGridShape(), ElementsAre(4, 4));
    auto bgs = lattice_graph.getBoundaryGridShape();
    ASSERT_THAT(bgs, ElementsAre(6, 6));
    auto _directions = lattice_graph.getDirections();

    for (auto v : lattice_graph.vertices)
    {
        auto cc = npp::unravel_index(v.first, bgs);
        ASSERT_THAT(v.second.coordinate, cc);
        ASSERT_THAT(npp::ravel_index(cc, bgs), v.first);
    }

    ASSERT_THAT(lattice_graph.vertices.size(), 36);
    ASSERT_THAT(lattice_graph.edges.size(), 24);
}
