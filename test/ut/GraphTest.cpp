#include <gtest/gtest.h>

#include "Utils.h"
#include "Graph.h"

using namespace std;

class GraphTest : public testing::Test {
    GraphTest() = default;

    ~GraphTest() override = default;
};

TEST(GraphTest, withVertices) {
    size_t N = 5;
    Graph graph(N);
    ASSERT_EQ(N, graph.getVertices().size());
    auto rangeSet = Utils::createRangeSet(N);
    for (Vertex v: graph.getVertices()) {
        ASSERT_TRUE(rangeSet.find(v.index) != rangeSet.end());
    }
}

TEST(GraphTest, edgeComparisonOperator) {
    vector<Edge> edges = {};
    Vertex v0{0}; Vertex v1{1};
    Edge edge1{v0, v1};
    Edge edge2{v1, v0};
    ASSERT_TRUE(edge1 == edge2);
    ASSERT_TRUE(edge2 == edge1);
    Vertex v2{2};
    Edge edge3{v0,v2};
    ASSERT_TRUE(edge1 != edge3);
    ASSERT_TRUE(edge3 != edge1);
}

TEST(GraphTest, withEdges_1) {
    size_t N = 5;
    Graph graph(N);
    ASSERT_EQ(N, graph.getVertices().size());
    auto rangeSet = Utils::createRangeSet(N);
    for (Vertex v: graph.getVertices()) {
        ASSERT_TRUE(rangeSet.find(v.index) != rangeSet.end());
    }

    auto graphVertices = graph.getVertices();

    vector<Edge> edges = {};
    Edge edge1{graphVertices[0], graphVertices[1]};
    Edge edge2{graphVertices[1], graphVertices[3]};
    Edge edge3{graphVertices[2], graphVertices[4]};
    Edge edge4{graphVertices[1], graphVertices[4]};
    Edge edge5{graphVertices[3], graphVertices[4]};

    edges.emplace_back(edge1);
    edges.emplace_back(edge2);
    edges.emplace_back(edge3);
    edges.emplace_back(edge4);
    edges.emplace_back(edge5);

    graph.addEdge(edge1);
    graph.addEdge(edge2);
    graph.addEdge(edge3);
    graph.addEdge(edge4);
    graph.addEdge(edge5);

    auto graphEdges = graph.getEdges();
    for (auto edge: edges) {
        ASSERT_TRUE(std::find(graphVertices.begin(),
                              graphVertices.end(),
                              edge.src) != graphVertices.end());

        ASSERT_TRUE(std::find(graphVertices.begin(),
                              graphVertices.end(),
                              edge.dest) != graphVertices.end());

        ASSERT_TRUE(std::find(graphEdges.begin(),
                              graphEdges.end(),
                              edge) != graphEdges.end());
    }
}

TEST(GraphTest, withEdges_2) {
    size_t N = 5;
    Graph graph(N);
    ASSERT_EQ(N, graph.getVertices().size());
    auto rangeSet = Utils::createRangeSet(N);
    for (Vertex v: graph.getVertices()) {
        ASSERT_TRUE(rangeSet.find(v.index) != rangeSet.end());
    }

    auto graphVertices = graph.getVertices();

    vector<Edge> edges = {};
    Edge edge1{graphVertices[0], graphVertices[1]};
    Edge edge2{graphVertices[1], graphVertices[3]};
    Edge edge3{graphVertices[2], graphVertices[4]};
    Edge edge4{graphVertices[1], graphVertices[4]};
    Edge edge5{graphVertices[3], graphVertices[4]};

    edges.emplace_back(edge1);
    edges.emplace_back(edge2);
    edges.emplace_back(edge3);
    edges.emplace_back(edge4);
    edges.emplace_back(edge5);

    graph.constructEdge(graphVertices[0], graphVertices[1]);
    graph.constructEdge(graphVertices[1], graphVertices[3]);
    graph.constructEdge(graphVertices[2], graphVertices[4]);
    graph.constructEdge(graphVertices[1], graphVertices[4]);
    graph.constructEdge(graphVertices[3], graphVertices[4]);

    auto graphEdges = graph.getEdges();
    for (auto edge: edges) {
        ASSERT_TRUE(std::find(graphVertices.begin(),
                              graphVertices.end(),
                              edge.src) != graphVertices.end());

        ASSERT_TRUE(std::find(graphVertices.begin(),
                              graphVertices.end(),
                              edge.dest) != graphVertices.end());

        ASSERT_TRUE(std::find(graphEdges.begin(),
                              graphEdges.end(),
                              edge) != graphEdges.end());
    }
}

TEST(GraphTest, removeEdge) {

}

TEST(GraphTest, ConnectedComponents) {
    // 5 vertices numbered from 0 to 4 without legs
    Graph graph{5};

    auto vertices = graph.getVertices();

    // create edges
    graph.constructEdge(vertices[1], vertices[0]);
    graph.constructEdge(vertices[2], vertices[3]);
    graph.constructEdge(vertices[3], vertices[4]);

    std::vector<std::set<int>> connectedComponents = graph.getConnectedComponentsIndices();

    ASSERT_TRUE(std::find(connectedComponents.begin(),
                          connectedComponents.end(),
                          std::set<int>{0, 1}) != connectedComponents.end());
    ASSERT_TRUE(std::find(connectedComponents.begin(),
                          connectedComponents.end(),
                          std::set<int>{2, 3, 4}) != connectedComponents.end());
}
