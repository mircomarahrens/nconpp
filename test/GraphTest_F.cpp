#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include "Graph.h"

using ::testing::ElementsAre;

class GraphTest_F : public testing::Test
{
protected:
    std::size_t number_of_vertices = 20;
    Graph<> graph_template;

    void SetUp() override
    {
        graph_template = Graph<>(20);
    }
};

TEST_F(GraphTest_F, SetUp)
{
    ASSERT_TRUE(graph_template.numVertices() == number_of_vertices);
}
