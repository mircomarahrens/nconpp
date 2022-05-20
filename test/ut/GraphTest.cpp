#include <gtest/gtest.h>

#include "Container.h"
#include "Graph.h"

class GraphTest : public testing::Test {
    GraphTest() = default;

    ~GraphTest() override = default;
};

TEST(GraphTest, withVertices) {
    size_t N = 5;
    Graph graph(N);
    ASSERT_EQ(N, graph.getVertices().size());
    auto rangeSet = Container::createRangeSet(N);
    for (Vertex v: graph.getVertices()) {
        ASSERT_TRUE(std::find(rangeSet.begin(),
                              rangeSet.end(),
                              v.index) != rangeSet.end());
    }
}

TEST(GraphTest, withEdges) {
    size_t N = 5;
    Graph graph(N);

}

TEST(GraphTest, removeEdge) {

}
