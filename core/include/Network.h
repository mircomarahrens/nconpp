#pragma once

#include "Algorithms.h"
#include "Graph.h"

#include <set>
#include <vector>

struct Leg {
    int index{0};
    int dim{0};
    struct Vertex *vertex{nullptr};
};

struct NetworkVertex : Vertex {
    std::set<Leg> legs{};
};

class Network : public Graph {
public:
    explicit Network(const std::vector<std::vector<int>> &vertexLegs);

    //explicit Network(const std::set<NetworkVertex> &networkVertices);

    ~Network() = default;

    void addEdge(int src, int dest) override;

    void addEdge(const Edge &edge) override;

    void removeEdgeByIndices(int src, int dest) override;

    void addLeg(const Leg &leg);

    const std::vector<std::vector<int>> &getVertexLegs();

private:
    std::vector<std::vector<int>> mVertexLegs;

    std::vector<Leg> mLegsList;

    void generateEdges(const std::vector<std::vector<int>> &vertexLegs);

    std::set<NetworkVertex> mNetworkVertices;
};
