#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include "Graph.h"

using ::testing::ElementsAre;

class GraphTest : public testing::Test
{
    GraphTest() = default;

    ~GraphTest() override = default;
};

TEST(GraphTest, Boost)
{
    int N = 6;

    struct default_vertex_properties
    {
    };

    struct default_edge_properties
    {
        std::size_t edge_index_t;
    };

    typedef typename boost::adjacency_list<
        boost::multisetS,
        boost::vecS, boost::undirectedS,
        default_vertex_properties,
        default_edge_properties>
        graph_template;

    graph_template g(N);

    auto v = g[0];

    auto pm = boost::get(boost::vertex_index, g);

    for (std::size_t i = 0; i < N; i++)
    {
        ASSERT_TRUE(pm[i] == i);
    }
}

TEST(GraphTest, baseline)
{
    Graph<> g(6);

    ASSERT_TRUE(g.numVertices() == 6);

    // identifiers in g are in ascending contiguous order starting with 0
    ASSERT_THAT(g.getVertices(), ElementsAre(0, 1, 2, 3, 4, 5));

    auto nv = g.addVertex();

    ASSERT_TRUE(nv == 6);

    ASSERT_TRUE(g.numVertices() == 7);

    g.removeVertex(2);

    ASSERT_TRUE(g.numVertices() == 6);

    // identifiers in graph are always in ascending contiguous order starting with 0
    ASSERT_THAT(g.getVertices(), ElementsAre(0, 1, 2, 3, 4, 5));

    // add one new edge between vertices 0 and 1 with edge index 0
    g.addEdge(0, 1, 0);

    // or add many of them via a list
    typedef std::tuple<std::pair<std::size_t, std::size_t>, int> Tuple;
    std::list<Tuple> edgeList = {
        Tuple({0, 2}, 1),
        Tuple({0, 3}, 2),
        Tuple({2, 0}, 3),
        Tuple({3, 4}, 4),
        Tuple({4, 1}, 5),
    };

    ASSERT_TRUE(edgeList.size() == 5);

    // add edges to Graph object
    g.addEdges(edgeList);

    ASSERT_TRUE(g.numEdges() == 6);

    // in our case one always have to provide a unique edge index
    EXPECT_THROW(
        try {
            g.addEdge(0, 1, 5);
        } catch (const std::invalid_argument &ex) {
            EXPECT_EQ(ERROR_MESSAGE::EDGE_INDEX_PRESENT, ex.what());
            throw;
        },
        std::invalid_argument);

    // Graph allows parallel edges per default, known as multigraph
    g.addEdge(2, 0, 6);

    ASSERT_TRUE(g.numEdges() == 7);

    ASSERT_THAT(g.getEdges(), ElementsAre(0, 1, 2, 3, 4, 5, 6));

    // Edge ids are ints
    g.addEdge(0, 4, -3);

    ASSERT_THAT(g.getEdges(), ElementsAre(-3, 0, 1, 2, 3, 4, 5, 6));

    // Retrieve edge by index
    std::pair<std::size_t, std::size_t> edge = g.getEdge(2);

    ASSERT_TRUE(edge.first == 0);  // source index
    ASSERT_TRUE(edge.second == 3); // target index

    g.removeEdge(3);

    ASSERT_TRUE(g.numEdges() == 7);
    ASSERT_THAT(g.getEdges(), ElementsAre(-3, 0, 1, 2, 4, 5, 6));
}

TEST(GraphTest, custom_properties)
{
    struct custom_vertex_properties
    {
        std::string name = "default constructed name for vertices";
    };

    struct custom_edge_properties
    {
        std::string name = "default constructed name for edges";
    };

    Graph<custom_vertex_properties, custom_edge_properties> g(6);

    ASSERT_TRUE(g.numVertices() == 6);

    // access vertices through index
    for (std::size_t i = 0; i < 6; i++) {
        ASSERT_TRUE(g[i].name == "default constructed name for vertices");
    }

    // access vertices and out edges through iterator
    auto vs = g.vertices();
    for(auto v = vs.first; v != vs.second; v++) {
        ASSERT_TRUE(g[*v].name == "default constructed name for vertices");
        auto oes = g.outEdges(*v);
        for(auto e = oes.first; e != oes.second; e++) {
            ASSERT_TRUE(g[*e].name == "default constructed name for edges");
        }
    }

    custom_vertex_properties prop{"manual constructed name for a vertex"};

    g.addVertex(prop);

    ASSERT_TRUE(g.numVertices() == 7);

    ASSERT_TRUE(g[6].name == "manual constructed name for a vertex");

    g.setVertexProperties(2, prop);

    ASSERT_TRUE(g[2].name == "manual constructed name for a vertex");

    // g.addEdge(0 , 1, 2);
}

TEST(GraphTest, mergeVertices)
{
    Graph<> g(6);

    typedef std::tuple<std::pair<std::size_t, std::size_t>, int> Tuple;
    std::list<Tuple> edgeList = {
        Tuple({0, 2}, 1),
        Tuple({1, 3}, 2),
        Tuple({2, 0}, 3),
        Tuple({3, 4}, 4),
        Tuple({4, 1}, 5),
    };   

    g.addEdges(edgeList);

    g.mergeVertices(0,1);

    ASSERT_TRUE(g.numVertices() == 5);

    //ASSERT_THAT(g.getVertices(), ElementsAre(0, 2, 3, 4, 5));
}
