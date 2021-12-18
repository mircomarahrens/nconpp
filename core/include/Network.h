#pragma once

#include "utils/Container.h"
#include "Graph.h"

#include <vector>

class Network : public Graph
{
public:
    Network(std::vector<std::vector<int>> vertexLegs);
    ~Network() = default;

    void addEdge(int src, int dest) override;
    void removeEdge(int src, int dest) override;
    void addLeg(int newLeg, int node);

    const std::vector<std::vector<int>>& getVertexLegs();

    void calculateConnectedComponents();
    const std::vector<std::set<int>>& getConnectedComponents();

private:
    std::vector<std::vector<int>> mVertexLegs;
    std::vector<std::set<int>> mConnectedComponents;

    void generateEdges(const std::vector<std::vector<int>>& vertexLegs);
};
