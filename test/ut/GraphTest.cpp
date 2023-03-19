#include <gtest/gtest.h>
#include <numeric>

#include "Graph.h"

class GraphTest : public testing::Test {
    GraphTest() = default;

    ~GraphTest() override = default;

public:
    static std::set<std::size_t> createRangeSet(std::size_t size) {
        std::vector<std::size_t> vec(size);
        std::iota(vec.begin(), vec.end(), 0);
        return {vec.begin(), vec.end()};
    }
};

TEST(GraphTest, Graph) {
    size_t N = 5;
    Graph graph(N);
    ASSERT_EQ(N, graph.getVertices().size());

    auto rangeSet = GraphTest::createRangeSet(N);
    for (auto v_id: graph.getVertices()) {
        ASSERT_TRUE(rangeSet.find(v_id) != rangeSet.end());
    }
}

TEST(GraphTest, addVertex) {
    size_t N = 5;
    Graph graph(N);
    ASSERT_EQ(N, graph.getVertices().size());

    auto rangeSet1 = GraphTest::createRangeSet(N);
    for (auto v_id: graph.getVertices()) {
        ASSERT_TRUE(rangeSet1.find(v_id) != rangeSet1.end());
    }

    auto v1 = graph.addVertex();
    ASSERT_TRUE(v1 == 5);
    auto v2 = graph.addVertex();
    ASSERT_TRUE(v2 == 6);

    auto rangeSet2 = GraphTest::createRangeSet(N+2);
    for (auto v_id: graph.getVertices()) {
        ASSERT_TRUE(rangeSet2.find(v_id) != rangeSet2.end());
    }
}

TEST(GraphTest, removeVertex) {
    size_t N = 5;
    Graph graph(N);
    ASSERT_EQ(N, graph.getVertices().size());

    auto rangeSet1 = GraphTest::createRangeSet(N);
    for (auto v_id: graph.getVertices()) {
        ASSERT_TRUE(rangeSet1.find(v_id) != rangeSet1.end());
    }

    // remove the vertex with id 2
    graph.removeVertex(2);

    auto vs = graph.getVertices();

    ASSERT_TRUE(vs.size() == N-1);

    // boost ensures that the identifiers of the vertices in the graph are still in ascending order [0, num_vertices)
    auto rangeSet2 = GraphTest::createRangeSet(N-1);
    for (auto v_id: graph.getVertices()) {
        ASSERT_TRUE(rangeSet2.find(v_id) != rangeSet2.end());
    }
}

TEST(GraphTest, addEdges) {
    size_t N = 5;
    Graph graph(N);
    auto vertices = graph.getVertices();
    ASSERT_EQ(N, vertices.size());

    auto rangeSet = GraphTest::createRangeSet(N);
    for (auto v_id: vertices) {
        ASSERT_TRUE(rangeSet.find(v_id) != rangeSet.end());
    }

    std::vector<std::pair<std::size_t, std::size_t>> edges = {};
    std::pair<std::size_t, std::size_t> edge1{vertices[0], vertices[1]};
    std::pair<std::size_t, std::size_t> edge2{vertices[1], vertices[3]};
    std::pair<std::size_t, std::size_t> edge3{vertices[2], vertices[4]};
    std::pair<std::size_t, std::size_t> edge4{vertices[1], vertices[4]};
    std::pair<std::size_t, std::size_t> edge5{vertices[3], vertices[4]};

    edges.emplace_back(edge1);
    edges.emplace_back(edge2);
    edges.emplace_back(edge3);
    edges.emplace_back(edge4);
    edges.emplace_back(edge5);

    graph.addEdge(vertices[0], vertices[1]);
    graph.addEdge(vertices[1], vertices[3]);
    graph.addEdge(vertices[2], vertices[4]);
    graph.addEdge(vertices[1], vertices[4]);
    graph.addEdge(vertices[3], vertices[4]);

    auto graphEdges = graph.getEdges();
    for (auto edge: edges) {
        ASSERT_TRUE(std::find(vertices.begin(),
                              vertices.end(),
                              edge.first) != vertices.end());

        ASSERT_TRUE(std::find(vertices.begin(),
                              vertices.end(),
                              edge.second) != vertices.end());

        ASSERT_TRUE(std::find(graphEdges.begin(),
                              graphEdges.end(),
                              edge) != graphEdges.end());
    }
}

TEST(GraphTest, removeEdges) {
    // TODO
//    size_t N = 5;
//    Graph graph(N);
//    ASSERT_EQ(N, graph.getVertices().size());
//    auto rangeSet = GraphTest::createRangeSet(N);
//    for (auto v_id: graph.getVertices()) {
//        ASSERT_TRUE(rangeSet.find(v_id) != rangeSet.end());
//    }
//
//    auto graphVertices = graph.getVertices();
//
//    graph.addEdge(graphVertices[0], graphVertices[1]);
//    graph.addEdge(graphVertices[1], graphVertices[3]);
//    graph.addEdge(graphVertices[2], graphVertices[4]);
//    graph.addEdge(graphVertices[1], graphVertices[4]);
//    graph.addEdge(graphVertices[3], graphVertices[4]);
//
//
//    auto graphEdges = graph.getEdges();
//    for (auto edge: graphEdges) {
//        ASSERT_TRUE(std::find(graphVertices.begin(),
//                              graphVertices.end(),
//                              edge.first) != graphVertices.end());
//
//        ASSERT_TRUE(std::find(graphVertices.begin(),
//                              graphVertices.end(),
//                              edge.second) != graphVertices.end());
//    }
}

TEST(GraphTest, ConnectedComponents) {
    // Graph with 5 nodes numbered from 0 to 4
    Graph graph{5};

    // create edges
    graph.addEdge(1, 0);
    graph.addEdge(2, 3);
    graph.addEdge(3, 4);

    std::vector<std::vector<std::size_t>> connectedComponents = graph.getConnectedComponents();

    ASSERT_TRUE(std::find(connectedComponents.begin(),
                          connectedComponents.end(),
                          std::vector<std::size_t>{0, 1}) != connectedComponents.end());
    ASSERT_TRUE(std::find(connectedComponents.begin(),
                          connectedComponents.end(),
                          std::vector<std::size_t>{2, 3, 4}) != connectedComponents.end());
}
