#pragma once

#include "Graph.h"

class Algorithms {
public:
    static std::vector<std::set<int>> getConnectedComponents(const std::size_t size,
                                                             const std::vector<std::vector<int>> &adjacencyList) {
        std::vector<std::set<int>> connectedComponents = {};

        std::vector<bool> visited(size);
        for (int v = 0; v < size; v++)
            visited[v] = false;

        std::set<int> component;
        for (int v = 0; v < size; v++) {
            if (!visited[v]) {
                // depth first search
                DFSVisit(v, visited, component, adjacencyList);
                connectedComponents.emplace_back(component);
                component.clear();
            }
        }

        return connectedComponents;
    };

private:
    static void DFSVisit(int v,
                         std::vector<bool> &visited,
                         std::set<int> &component,
                         const std::vector<std::vector<int>> &adjacencyList) {
        visited[v] = true;
        component.insert(v);

        for (auto i = adjacencyList[v].begin(); i != adjacencyList[v].end(); ++i)
            if (!visited[*i])
                DFSVisit(*i, visited, component, adjacencyList);
    };

    static void BFSVisit(int v,
                         std::vector<int> &visited,
                         std::set<int> &component,
                         const std::vector<std::vector<int>> &adjacencyList) {
        // TODO
    };
};
