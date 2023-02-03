#pragma once

#include <algorithm>
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
    /**
     * Constructor for a graph with n nodes
     *
     * @param N
     */
    explicit Graph(std::size_t N) {
        for (int i = 0; i < N; i++) {
            Vertex vertex{i};
            mVertices.emplace_back(vertex);
        }
    };

    ~Graph() = default;

    /**
     * Construct an undirected edge between a source node src and a destination node dest.
     *
     * @param src
     * @param dest
     */
    virtual void constructEdge(const Vertex &src, const Vertex &dest) {
        if (std::find(mVertices.begin(), mVertices.end(), src) != mVertices.end() &&
            std::find(mVertices.begin(), mVertices.end(), dest) != mVertices.end()) {
            Edge edge{src, dest};
            mEdges.emplace_back(edge);
        }
    };

    /**
     * Remove all edges between nodes.
     *
     * @param src
     * @param dest
     */
    virtual void removeAllEdges(const Vertex &src, const Vertex &dest) {
        auto pos = std::find_if(mEdges.begin(),
                                mEdges.end(),
                                [&csrc = src, &cdest = dest]
                                        (const Edge &edge) -> bool {
                                    return (csrc == edge.src && cdest == edge.dest);
                                });
        mEdges.erase(pos);
    };

    /**
     * Add a new node to the graph.
     *
     * @param vertex
     */
    virtual void addVertex(const Vertex &vertex) {
        mVertices.emplace_back(vertex);
    };

    /**
     * Remove a node from the graph and all corresponding edges with the node.
     *
     * @param vertex
     */
    virtual void removeVertex(const Vertex &vertex) {
        for (auto dest: mVertices)
            removeAllEdges(vertex, dest);
        auto pos = std::find_if(mVertices.begin(),
                                mVertices.end(),
                                [vertex]
                                        (const Vertex &cvertex) -> bool {
                                    return (cvertex.index == vertex.index);
                                });
        mVertices.erase(pos);
    };

    /**
     * Return all edges.
     *
     * @return
     */
    const std::vector<Edge> &getEdges() {
        return mEdges;
    };

    /**
     * Return all nodes.
     *
     * @return
     */
    const std::vector<Vertex> &getVertices() {
        return mVertices;
    };

    /**
     * Calculate the adjacency list for the graph from the edges.
     *
     * @return
     */
    std::vector<std::vector<int>> calculateAdjacencyList() {
        std::vector<std::vector<int>> adjacencyList(mVertices.size());
        for (auto e: mEdges) {
            adjacencyList[e.src.index].emplace_back(e.dest.index);
            adjacencyList[e.dest.index].emplace_back(e.src.index);
        }

        return std::move(adjacencyList);
    };

    // TODO maybe there is a way to update this on the fly?
    /**
     * Calculate connected components within the graph and returns them.
     *
     * @return
     */
    std::vector<std::vector<int>> calculateConnectedComponents() {
        return std::move(getConnectedComponents(mVertices.size(), calculateAdjacencyList()));
    };

    /**
     * Returns all unique node indices.
     *
     * @return
     */
    std::vector<int> getVertexIndices() {
        std::vector<int> indices;
        for (Vertex vertex: mVertices)
            indices.emplace_back(vertex.index);
        return std::move(indices);
    };

protected:
    std::vector<Vertex> mVertices;
    std::vector<Edge> mEdges;

private:
    /**
     * Returns current connected components within the graph.
     *
     * The connected components are discovered via depth first search.
     *
     * @param size
     * @param adjacencyList
     * @return
     */
    std::vector<std::vector<int>> getConnectedComponents(std::size_t size,
                                                         const std::vector<std::vector<int>> &adjacencyList) {
        std::vector<std::vector<int>> connectedComponents = {};

        std::vector<bool> visited(size);
        for (int v = 0; v < size; v++)
            visited[v] = false;

        std::vector<int> component;
        for (int v = 0; v < size; v++) {
            if (!visited[v]) {
                // depth first search via recursion
                // TODO add a threshold for the depth?
                // Remarks: Why DFS and not BFS?
                // https://cs.stackexchange.com/questions/73686/why-do-we-prefer-dfs-to-find-connected-components
                // That is why we use Depth-First Search Mostly because:
                //  1. there is no need to find an optimal solution
                //  2. memory matters!
                DFSVisit(v, visited, component, adjacencyList);
                connectedComponents.emplace_back(component);
                component.clear();
            }
        }

        return connectedComponents;
    };

    /**
     * Recursive depth first search to mark visited nodes.
     *
     * @param v
     * @param visited
     * @param component
     * @param adjacencyList
     */
    void DFSVisit(int v,
                  std::vector<bool> &visited,
                  std::vector<int> &component,
                  const std::vector<std::vector<int>> &adjacencyList) {
        visited[v] = true;
        component.emplace_back(v);

        for (auto i = adjacencyList[v].begin(); i != adjacencyList[v].end(); ++i)
            if (!visited[*i])
                DFSVisit(*i, visited, component, adjacencyList);
    };

    /**
     * TODO
     * Recursive breath first search to mark visited nodes.
     *
     * @param v
     * @param visited
     * @param component
     * @param adjacencyList
     */
    void BFSVisit(int v,
                  std::vector<bool> &visited,
                  std::vector<int> &component,
                  const std::vector<std::vector<int>> &adjacencyList) {
        visited[v] = true;
        component.emplace_back(v);
    };
};
