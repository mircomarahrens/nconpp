// Copyright 2023 Mirco Marahrens

#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include "Graph.hpp"

using ::testing::ElementsAre;
using ::testing::Pair;
using ::testing::UnorderedElementsAre;

class GraphTest : public testing::Test {
  GraphTest() = default;

  ~GraphTest() override = default;
};

TEST(GraphTest, EmptyGraph) { ASSERT_NO_THROW(Graph<> g); }

TEST(GraphTest, FiniteGraph) { ASSERT_NO_THROW(Graph<> g(123)); }

TEST(GraphTest, DefaultInit) {
  Graph<> g;

  ASSERT_FALSE(g.graph_properties.directed_edges);
  ASSERT_TRUE(g.graph_properties.parallel_edges);

  ASSERT_EQ(g.NumVertices(), 0);
  ASSERT_EQ(g.NumEdges(), 0);
}

TEST(GraphTest, getVertices) {
  Graph<> g(6);

  ASSERT_EQ(g.NumVertices(), 6);

  ASSERT_THAT(g.getVertices(), UnorderedElementsAre(0, 1, 2, 3, 4, 5));
}

TEST(GraphTest, addVertex) {
  Graph<> g(6);

  ASSERT_EQ(g.NumVertices(), 6);

  ASSERT_THAT(g.getVertices(), UnorderedElementsAre(0, 1, 2, 3, 4, 5));

  EXPECT_THROW(
      try { g.addVertex(5); } catch (const std::invalid_argument &ex) {
        EXPECT_STREQ(ErrorMessages::ERROR_VERTEXID_PRESENT, ex.what());
        throw;
      },
      std::invalid_argument);

  g.addVertex(6);

  ASSERT_EQ(g.NumVertices(), 7);

  ASSERT_THAT(g.getVertices(), UnorderedElementsAre(0, 1, 2, 3, 4, 5, 6));
}

TEST(GraphTest, removeVertex) {
  Graph<> g(6);

  ASSERT_EQ(g.NumVertices(), 6);

  ASSERT_THAT(g.getVertices(), UnorderedElementsAre(0, 1, 2, 3, 4, 5));

  EXPECT_THROW(
      try { g.removeVertex(6); } catch (const std::invalid_argument &ex) {
        EXPECT_STREQ(ErrorMessages::ERROR_VERTEXID_NOTPRESENT, ex.what());
        throw;
      },
      std::invalid_argument);

  g.removeVertex(5);

  ASSERT_EQ(g.NumVertices(), 5);

  ASSERT_THAT(g.getVertices(), UnorderedElementsAre(0, 1, 2, 3, 4));
}

TEST(GraphTest, addEdge) {
  Graph<> g(6);

  ASSERT_EQ(g.NumVertices(), 6);

  ASSERT_THAT(g.getVertices(), UnorderedElementsAre(0, 1, 2, 3, 4, 5));

  EXPECT_THROW(
      try { g.addEdge(0, 6, 5); } catch (const std::invalid_argument &ex) {
        EXPECT_STREQ(ErrorMessages::ERROR_SOURCEID_NOTPRESENT, ex.what());
        throw;
      },
      std::invalid_argument);

  EXPECT_THROW(
      try { g.addEdge(0, 5, 6); } catch (const std::invalid_argument &ex) {
        EXPECT_STREQ(ErrorMessages::ERROR_DESTID_NOTPRESENT, ex.what());
        throw;
      },
      std::invalid_argument);

  g.addEdge(0, 1, 2);

  EXPECT_THROW(
      try { g.addEdge(0, 1, 2); } catch (const std::invalid_argument &ex) {
        EXPECT_STREQ(ErrorMessages::ERROR_EDGEID_PRESENT, ex.what());
        throw;
      },
      std::invalid_argument);

  ASSERT_EQ(g.NumEdges(), 1);

  ASSERT_THAT(g.getEdges(), UnorderedElementsAre(0));

  ASSERT_EQ(g.edges[0].src, 1);
  ASSERT_EQ(g.edges[0].dest, 2);
}

TEST(GraphTest, removeEdge) {
  Graph<> g(6);

  ASSERT_EQ(g.NumVertices(), 6);

  ASSERT_THAT(g.getVertices(), UnorderedElementsAre(0, 1, 2, 3, 4, 5));

  g.addEdge(0, 1, 2);

  ASSERT_EQ(g.NumEdges(), 1);

  ASSERT_THAT(g.getEdges(), UnorderedElementsAre(0));

  EXPECT_THROW(
      try { g.removeEdge(1); } catch (const std::invalid_argument &ex) {
        EXPECT_STREQ(ErrorMessages::ERROR_EDGEID_NOTPRESENT, ex.what());
        throw;
      },
      std::invalid_argument);

  g.removeEdge(0);

  ASSERT_EQ(g.NumEdges(), 0);
}

TEST(GraphTest, customVertexProperties) {
  struct graph_properties {};

  struct vertex_properties {
    int prop1 = 0;
    std::vector<int> prop2 = {1, 2, 3};
  };

  struct edge_properties {};

  Graph<graph_properties, vertex_properties, edge_properties> g(6);

  g.vertices[3].prop1 = 1;
  g.vertices[3].prop2 = {4, 5, 6};

  for (auto i = 0; i < g.NumVertices(); i++) {
    if (i != 3) {
      ASSERT_EQ(g.vertices[i].prop1, 0);
      ASSERT_THAT(g.vertices[i].prop2, ElementsAre(1, 2, 3));
    } else {
      ASSERT_EQ(g.vertices[i].prop1, 1);
      ASSERT_THAT(g.vertices[i].prop2, ElementsAre(4, 5, 6));
    }
  }
}

TEST(GraphTest, customEdgeProperties) {
  struct graph_properties {};

  struct vertex_properties {
    int prop1 = 0;
    std::vector<int> prop2 = {1, 2, 3};
  };

  struct edge_properties {
    bool prop3 = true;
  };

  Graph<graph_properties, vertex_properties, edge_properties> g(6);

  g.vertices[3].prop1 = 1;
  g.vertices[3].prop2 = {4, 5, 6};

  for (auto i = 0; i < g.NumVertices(); i++) {
    if (i != 3) {
      ASSERT_EQ(g.vertices[i].prop1, 0);
      ASSERT_THAT(g.vertices[i].prop2, ElementsAre(1, 2, 3));
    } else {
      ASSERT_EQ(g.vertices[i].prop1, 1);
      ASSERT_THAT(g.vertices[i].prop2, ElementsAre(4, 5, 6));
    }
  }

  g.addEdge(0, 3, 4);

  ASSERT_TRUE(g.edges[0].prop3 == true);
}

TEST(GraphTest, mergeVertices) {
  Graph<> g(6);

  g.addEdge(0, 1, 2);
  g.addEdge(1, 2, 3);
  g.addEdge(2, 2, 4);
  g.addEdge(3, 3, 4);

  // AFTER MERGE
  // --> (., 1, 1), (., 1, 3), (., 1, 4), (., 3, 4)

  ASSERT_EQ(g.NumEdges(), 4);

  ASSERT_THAT(g.getEdges(), UnorderedElementsAre(0, 1, 2, 3));

  g.mergeVertices(1, 2);

  ASSERT_EQ(g.NumEdges(), 4);

  ASSERT_THAT(g.getEdges(), UnorderedElementsAre(0, 1, 2, 3));

  ASSERT_EQ(g.edges[0].src, 1);
  ASSERT_EQ(g.edges[0].dest, 1);
  ASSERT_EQ(g.edges[1].src, 1);
  ASSERT_EQ(g.edges[1].dest, 3);
  ASSERT_EQ(g.edges[2].src, 1);
  ASSERT_EQ(g.edges[2].dest, 4);
  ASSERT_EQ(g.edges[3].src, 3);
  ASSERT_EQ(g.edges[3].dest, 4);
}

TEST(GraphTest, parallelEdgePresent) {
  Graph<> g(3, false);

  g.addEdge(0, 1, 2);

  EXPECT_THROW(
      try { g.addEdge(1, 2, 1); } catch (const std::invalid_argument &ex) {
        EXPECT_STREQ(ErrorMessages::ERROR_PARALLEL_EDGE_PRESENT, ex.what());
        throw;
      },
      std::invalid_argument);

  EXPECT_THROW(
      try { g.addEdge(2, 1, 2); } catch (const std::invalid_argument &ex) {
        EXPECT_STREQ(ErrorMessages::ERROR_PARALLEL_EDGE_PRESENT, ex.what());
        throw;
      },
      std::invalid_argument);
}

TEST(GraphTest, largeCustomGraph) {
  struct graph_properties {};

  struct vertex_properties {
    int vp1 = 0;
    std::vector<int> vp2 = {1, 2, 3, 4, 6};
    bool vp3 = false;
  };

  struct edge_properties {
    bool ep1 = true;
    double ep2 = 0.55;
  };

  Graph<graph_properties, vertex_properties, edge_properties> g(1000);

  for (std::size_t id = 0; id < 2000; id++) {
    g.addEdge(id, std::rand() % 1000, std::rand() % 1000);
  }

  ASSERT_EQ(g.NumEdges(), 2000);

  for (auto el : g.edges) {
    auto edge = el.second;
    ASSERT_TRUE(g.adjacency_list[edge.src].find(edge.dest) !=
                g.adjacency_list[edge.src].end());
    ASSERT_TRUE(g.adjacency_list[edge.dest].find(edge.src) !=
                g.adjacency_list[edge.dest].end());
  }
}
