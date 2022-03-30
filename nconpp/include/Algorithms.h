#pragma once

#include "Graph.h"

class Algorithms {
public:
    static std::vector<std::set<int>> getConnectedComponents(std::size_t size,
                                                             const std::vector<std::vector<int>> &adjacencyList);

private:
    static void DFSVisit(int v,
                         std::vector<bool> &visited,
                         std::set<int> &component,
                         const std::vector<std::vector<int>> &adjacencyList);

    static void BFSVisit(int v,
                         std::vector<int> &visited,
                         std::set<int> &component,
                         const std::vector<std::vector<int>> &adjacencyList) {
        // TODO
    };
};
