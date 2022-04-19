#pragma once

#include "Algorithms.h"
#include "Graph.h"

#include <set>
#include <vector>

struct Leg {
    int id;
    int dim;
    int node;
};

class Network : public Graph {
public:
    explicit Network(const std::vector<std::vector<int>> &vertexLegs);

    ~Network() = default;

    void addEdge(int src, int dest) override;

    void removeEdgeByIndices(int src, int dest) override;

    void addLeg(int newLeg, int node);

    void addLeg(const Leg &leg);

    const std::vector<std::vector<int>> &getVertexLegs();

private:
    std::vector<std::vector<int>> mVertexLegs;

    std::vector<Leg> mLegsList;

    void generateEdges(const std::vector<std::vector<int>> &vertexLegs);
};
