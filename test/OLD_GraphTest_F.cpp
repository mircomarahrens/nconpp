#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include "Graph.h"

using ::testing::ElementsAre;

class GraphTest_F : public testing::Test
{
protected:
    Graph<> graph;

    std::size_t number_of_vertices = 20;

    std::size_t number_of_edges = 30;

    std::list<std::tuple<std::pair<std::size_t, std::size_t>, int>> edge_list;

    void SetUp() override
    {
        graph = Graph<>(number_of_vertices);

        typedef std::tuple<std::pair<std::size_t, std::size_t>, int> Tuple;
        for (int n = 0; n != number_of_edges; ++n)
        {
            int x = number_of_vertices + 1, y = number_of_vertices + 1;
            while (x > number_of_vertices)
            {
                x = std::rand() / ((RAND_MAX + 1u) / number_of_vertices);
                y = std::rand() / ((RAND_MAX + 1u) / number_of_vertices);
            }
            edge_list.emplace_back(Tuple({x, y}, n));
        }

        graph.addEdges(edge_list);
    }
};

TEST_F(GraphTest_F, SetUp)
{
    ASSERT_EQ(graph.NumVertices(), number_of_vertices);
    ASSERT_EQ(graph.numEdges(), number_of_edges);

    for (auto e : edge_list)
    {
        auto ge = graph.getEdge(std::get<1>(e));
        ASSERT_EQ(std::get<0>(e), ge);
    }
}
