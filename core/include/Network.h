#pragma once

#include "Graph.h"

#include <vector>
#include <set>

class Network : public Graph
{
public:
    Network(std::vector<std::vector<int>> vertexLegs);
    ~Network() = default;

    void addEdge(int src, int dest) override;
    void removeEdge(int src, int dest) override;
    void addLeg(int newLeg, int node);

    const std::vector<std::vector<int>>& getVertexLegs();

    void calculateConnectedComponents(); // TODO to own class
    const std::vector<std::set<int>>& getConnectedComponents();

private:
    std::vector<std::vector<int>> mVertexLegs;
    std::vector<std::set<int>> mConnectedComponents;

    void generateEdges(const std::vector<std::vector<int>>& vertexLegs);

    struct Search
    {
        static void connectedComponents(const std::size_t size,
            const std::vector<std::vector<int>>& adjanceyList,
            std::vector<std::set<int>>& connectedComponents)
        {
            std::vector<bool> visited(size);
            for (int v = 0; v < size; v++)
                visited[v] = false;

            std::set<int> component;
            for (int v = 0; v < size; v++)
            {
                if (visited[v] == false)
                {
                    // depth first search
                    DFSUtil(v, visited, component, adjanceyList);
                    connectedComponents.emplace_back(component);
                    component.clear();
                }
            }
        };

        static void DFSUtil(int v,
            std::vector<bool>& visited,
            std::set<int>& component,
            const std::vector<std::vector<int>>& adjanceyList)
        {
            visited[v] = true;
            component.insert(v);

            for (auto i = adjanceyList[v].begin(); i != adjanceyList[v].end(); ++i)
                if (!visited[*i])
                    DFSUtil(*i, visited, component, adjanceyList);
        };
    };
};
