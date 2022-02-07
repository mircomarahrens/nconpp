#pragma once

#include <vector>
#include <set>

class Graph
{
public:
    Graph(std::size_t N);
    ~Graph() = default;

    virtual void addEdge(int src, int dest);
    virtual void removeEdge(int src, int dest);

    void addVertex(int vertex);
    void removeVertex(int vertex);

    const std::set<int>& getVertices();
    const std::vector<std::vector<int>>& getAdjanceyList();

private:
    std::set<int> mVertices;
    std::vector<std::vector<int>> mAdjanceyList;
};
