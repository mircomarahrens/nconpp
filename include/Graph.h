#pragma once

#include <optional>
#include <set>
#include <vector>

enum Color {
    white, gray, black
};

struct Vertex {
    int index{0};
    std::optional<Color> color{white};
    std::optional<int> distance{-1};
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

    virtual void constructEdge(const Vertex &src, const Vertex &dest);

    virtual void addEdge(const Edge &edge);

    virtual void removeAllEdges(const Vertex &src, const Vertex &dest);

    virtual void removeEdge(const Edge &edge);

    virtual void addVertex(const Vertex &vertex);

    void removeVertex(const Vertex &vertex);

    const std::vector<Edge> &getEdges();

    const std::vector<Vertex> &getVertices();

    const std::vector<std::vector<int>> &getAdjacencyList();

    const std::vector<std::set<int>> &getConnectedComponentsIndices();

protected:
    std::vector<Vertex> mVertices;

    std::vector<Edge> mEdges;

private:
    std::vector<std::set<int>> getConnectedComponents(std::size_t size,
                                                      const std::vector<std::vector<int>> &adjacencyList);

    void DFSVisit(int v,
                  std::vector<bool> &visited,
                  std::set<int> &component,
                  const std::vector<std::vector<int>> &adjacencyList);

    void BFSVisit(int v,
                  std::vector<bool> &visited,
                  std::set<int> &component,
                  const std::vector<std::vector<int>> &adjacencyList);
};
