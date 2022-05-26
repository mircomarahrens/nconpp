#pragma once

#include <set>
#include <vector>

enum Color {
    white, gray, black
};

struct Vertex {
    int index{0};
    Color color{white};
    int distance{-1};
    struct Vertex *parent{nullptr};

    friend bool operator<(const Vertex &lhs, const Vertex &rhs) {
        return lhs.index < rhs.index;
    };

    friend bool operator>(const Vertex &lhs, const Vertex &rhs) {
        return lhs.index > rhs.index;
    };

    friend bool operator==(const Vertex &lhs, const Vertex &rhs) {
        return lhs.index == rhs.index;
    };

    friend bool operator!=(const Vertex &lhs, const Vertex &rhs) {
        return lhs.index != rhs.index;
    };
};

struct Edge {
    Vertex src;
    Vertex dest;
    bool directed{false};

    friend bool operator==(const Edge &lhs, const Edge &rhs) {
        if (!lhs.directed && !rhs.directed)
            return (lhs.src == rhs.src && lhs.dest == rhs.dest) || (lhs.dest == rhs.src && lhs.src == rhs.dest);
        else
            return (lhs.src == rhs.src && lhs.dest == rhs.dest);
    };

    friend bool operator!=(const Edge &lhs, const Edge &rhs) {
        if (!lhs.directed && !rhs.directed)
            return (lhs.src != rhs.src || lhs.dest != rhs.dest) || (lhs.dest != rhs.src || lhs.src != rhs.dest);
        else
            return (lhs.src != rhs.src || lhs.dest != rhs.dest);
    };
};

class Graph {
public:
    explicit Graph(std::size_t N);

    ~Graph() = default;

    virtual void addEdge(int src, int dest);

    virtual void addEdge(const Vertex &src, const Vertex &dest);

    virtual void addEdge(const Edge &edge);

    virtual void removeEdgeByIndices(int src, int dest);

    virtual void removeEdge(const Edge &edge);

    virtual void addVertex(const Vertex &vertex);

    const std::vector<Vertex> &getVertices();

    const std::vector<Edge> &getEdges();

    void removeVertexByIndex(int index);

    void removeVertex(const Vertex &vertex);

    const std::set<int> &getVertexIndices();

    const std::vector<std::vector<int>> &getAdjacencyList();

private:
    std::set<int> mVertexIndices;
    std::vector<Vertex> mVertices;
    std::vector<Edge> mEdges;

    std::vector<std::vector<int>> mAdjacencyList;

    void addVertexIndex(int vertexIndex);
};
