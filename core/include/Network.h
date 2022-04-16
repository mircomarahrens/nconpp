#pragma once

#include "Algorithms.h"
#include "Graph.h"

#include <set>
#include <vector>

class Network : public Graph {
public:
    explicit Network(const std::vector<std::vector<int>>& vertexLegs);

    ~Network() = default;

    void addEdge(int src, int dest) override;

    void removeEdgeByIndices(int src, int dest) override;

    void addLeg(int newLeg, int node);

    const std::vector<std::vector<int>> &getVertexLegs();

private:
    std::vector<std::vector<int>> mVertexLegs;

    void generateEdges(const std::vector<std::vector<int>> &vertexLegs);
};
