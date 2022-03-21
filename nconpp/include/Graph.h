#pragma once

#include <vector>
#include <set>

struct Edge {
    int src;
    int dest;
};

struct Vertex {
    int index;

    friend bool operator<(const Vertex &lhs, const Vertex &rhs) {
        return lhs.index < rhs.index;
    };

    friend bool operator>(const Vertex &lhs, const Vertex &rhs) {
        return lhs.index > rhs.index;
    };
};

class Graph {
public:
    explicit Graph(std::size_t N);

    ~Graph() = default;

    virtual void addEdge(int src, int dest);

    virtual void addEdge(const Edge &edge);

    virtual void removeEdgeByIndices(int src, int dest);

    virtual void removeEdge(const Edge &edge);

    void addVertexIndex(int vertexIndex);

    virtual void addVertex(const Vertex &vertex);

    const std::set<Vertex> &getVertices();

    void removeVertexByIndex(int index);

    void removeVertex(const Vertex &vertex);

    const std::set<int> &getVertexIndices();

    const std::vector<std::vector<int>> &getAdjacencyList();

private:
    std::set<int> mVertexIndices;
    std::set<Vertex> mVertices;
    std::vector<std::vector<int>> mAdjacencyList;
};
