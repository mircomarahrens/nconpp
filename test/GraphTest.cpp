#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include "Graph.h"

using ::testing::ElementsAre;
using ::testing::UnorderedElementsAre;

class GraphTest : public testing::Test
{
    GraphTest() = default;

    ~GraphTest() override = default;
};

TEST(GraphTest, EmptyGraph)
{
    ASSERT_NO_THROW(Graph<> g);
}

TEST(GraphTest, FiniteGraph)
{
    ASSERT_NO_THROW(Graph<> g(123));
}

TEST(GraphTest, getVertices)
{
    Graph<> g(6);

    ASSERT_TRUE(g.NumVertices() == 6);

    ASSERT_THAT(g.getVertices(), UnorderedElementsAre(0, 1, 2, 3, 4, 5));
}

TEST(GraphTest, addVertex)
{
    Graph<> g(6);

    ASSERT_TRUE(g.NumVertices() == 6);

    ASSERT_THAT(g.getVertices(), UnorderedElementsAre(0, 1, 2, 3, 4, 5));

    EXPECT_THROW(
        try {
            g.addVertex(5);
        } catch (const std::invalid_argument &ex) {
            EXPECT_EQ(ERROR_MESSAGE::VERTEX_PRESENT, ex.what());
            throw;
        },
        std::invalid_argument);

    g.addVertex(6);

    ASSERT_TRUE(g.NumVertices() == 7);

    ASSERT_THAT(g.getVertices(), UnorderedElementsAre(0, 1, 2, 3, 4, 5, 6));
}

TEST(GraphTest, removeVertex)
{
    Graph<> g(6);

    ASSERT_TRUE(g.NumVertices() == 6);

    ASSERT_THAT(g.getVertices(), UnorderedElementsAre(0, 1, 2, 3, 4, 5));

    EXPECT_THROW(
        try {
            g.removeVertex(6);
        } catch (const std::invalid_argument &ex) {
            EXPECT_EQ(ERROR_MESSAGE::VERTEX_NOTPRESENT, ex.what());
            throw;
        },
        std::invalid_argument);

    g.removeVertex(5);

    ASSERT_TRUE(g.NumVertices() == 5);

    ASSERT_THAT(g.getVertices(), UnorderedElementsAre(0, 1, 2, 3, 4));
}

TEST(GraphTest, addEdge)
{
    Graph<> g(6);

    ASSERT_TRUE(g.NumVertices() == 6);

    ASSERT_THAT(g.getVertices(), UnorderedElementsAre(0, 1, 2, 3, 4, 5));

    EXPECT_THROW(
        try {
            g.addEdge(0, 6, 5);
        } catch (const std::invalid_argument &ex) {
            EXPECT_EQ(ERROR_MESSAGE::SOURCE_NOTPRESENT, ex.what());
            throw;
        },
        std::invalid_argument);

    EXPECT_THROW(
        try {
            g.addEdge(0, 5, 6);
        } catch (const std::invalid_argument &ex) {
            EXPECT_EQ(ERROR_MESSAGE::DEST_NOTPRESENT, ex.what());
            throw;
        },
        std::invalid_argument);

    g.addEdge(0, 1, 2);

    EXPECT_THROW(
        try {
            g.addEdge(0, 1, 2);
        } catch (const std::invalid_argument &ex) {
            EXPECT_EQ(ERROR_MESSAGE::EDGE_PRESENT, ex.what());
            throw;
        },
        std::invalid_argument);

    ASSERT_TRUE(g.NumEdges() == 1);

    ASSERT_THAT(g.getEdges(), UnorderedElementsAre(0));

    ASSERT_TRUE(g.edges[0].src == 1);
    ASSERT_TRUE(g.edges[0].dest == 2);
    ASSERT_TRUE(g.edges[0].directed == false);
}

TEST(GraphTest, removeEdge)
{
    Graph<> g(6);

    ASSERT_TRUE(g.NumVertices() == 6);

    ASSERT_THAT(g.getVertices(), UnorderedElementsAre(0, 1, 2, 3, 4, 5));

    g.addEdge(0, 1, 2);

    ASSERT_TRUE(g.NumEdges() == 1);

    ASSERT_THAT(g.getEdges(), UnorderedElementsAre(0));

    EXPECT_THROW(
        try {
            g.removeEdge(1);
        } catch (const std::invalid_argument &ex) {
            EXPECT_EQ(ERROR_MESSAGE::EDGE_NOTPRESENT, ex.what());
            throw;
        },
        std::invalid_argument);

    g.removeEdge(0);

    ASSERT_TRUE(g.NumEdges() == 0);
}

TEST(GraphTest, customVertexProperties)
{
    struct vertex_properties
    {
        int prop1 = 0;
        std::vector<int> prop2 = {1, 2, 3};
    };

    Graph<vertex_properties> g(6);

    g.vertices[3].prop1 = 1;
    g.vertices[3].prop2 = {4, 5, 6};

    for (auto i = 0; i < g.NumVertices(); i++)
    {
        if (i != 3)
        {
            ASSERT_TRUE(g.vertices[i].prop1 == 0);
            ASSERT_THAT(g.vertices[i].prop2, ElementsAre(1, 2, 3));
        }
        else
        {
            ASSERT_TRUE(g.vertices[i].prop1 == 1);
            ASSERT_THAT(g.vertices[i].prop2, ElementsAre(4, 5, 6));
        }
    }
}

TEST(GraphTest, customEdgeProperties)
{
    struct vertex_properties
    {
        int prop1 = 0;
        std::vector<int> prop2 = {1, 2, 3};
    };

    struct edge_properties
    {
        bool prop3 = true;
    };

    Graph<vertex_properties, edge_properties> g(6);

    g.vertices[3].prop1 = 1;
    g.vertices[3].prop2 = {4, 5, 6};

    for (auto i = 0; i < g.NumVertices(); i++)
    {
        if (i != 3)
        {
            ASSERT_TRUE(g.vertices[i].prop1 == 0);
            ASSERT_THAT(g.vertices[i].prop2, ElementsAre(1, 2, 3));
        }
        else
        {
            ASSERT_TRUE(g.vertices[i].prop1 == 1);
            ASSERT_THAT(g.vertices[i].prop2, ElementsAre(4, 5, 6));
        }
    }

    g.addEdge(0, 3, 4);

    ASSERT_TRUE(g.edges[0].prop3 == true);
}

TEST(GraphTest, mergeVertices)
{
    Graph<> g(6);

    g.addEdge(0, 1, 2);
    g.addEdge(1, 2, 3);

    // --> (x, 1, 3)

    ASSERT_TRUE(g.NumEdges() == 2);

    ASSERT_THAT(g.getEdges(), UnorderedElementsAre(0, 1));

    g.mergeVertices(1, 2);

    ASSERT_TRUE(g.NumEdges() == 1);

    ASSERT_THAT(g.getEdges(), UnorderedElementsAre(1));
}
