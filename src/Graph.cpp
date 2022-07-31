#include "Graph.h"

#include <algorithm>

using namespace std;

Graph::Graph(size_t N) {
    for (int i = 0; i < N; i++) {
        Vertex vertex{i};
        mVertices.emplace_back(vertex);
    }
}

void Graph::constructEdge(const Vertex &src, const Vertex &dest) {
    if (std::find(mVertices.begin(), mVertices.end(), src) != mVertices.end() &&
        std::find(mVertices.begin(), mVertices.end(), dest) != mVertices.end()) {
        Edge edge{src, dest};
        mEdges.emplace_back(edge);
    }
}

void Graph::removeAllEdges(const Vertex &src, const Vertex &dest) {
    auto pos = std::find_if(mEdges.begin(),
                            mEdges.end(),
                            [&csrc = src, &cdest = dest]
                                    (const Edge &edge) -> bool { return (csrc == edge.src && cdest == edge.dest); });
    mEdges.erase(pos);
}

void Graph::addVertex(const Vertex &vertex) {
    mVertices.emplace_back(vertex);
}

void Graph::removeVertex(const Vertex &vertex) {
    for (auto dest: mVertices)
        removeAllEdges(vertex, dest);
    auto pos = std::find_if(mVertices.begin(),
                            mVertices.end(),
                            [vertex]
                                    (const Vertex &cvertex) -> bool {
                                return (cvertex.index == vertex.index);
                            });
    mVertices.erase(pos);
}

const std::vector<Edge> &Graph::getEdges() {
    return mEdges;
}

const vector<Vertex> &Graph::getVertices() {
    return mVertices;
}

vector<vector<int>> Graph::calculateAdjacencyList() {
    vector<vector<int>> adjacencyList(mVertices.size());
    for (auto e: mEdges) {
        adjacencyList[e.src.index].emplace_back(e.dest.index);
        adjacencyList[e.dest.index].emplace_back(e.src.index);
    }

    return move(adjacencyList);
}

// TODO maybe there is a way to update this on the fly?
vector<vector<int>> Graph::calculateConnectedComponents() {
    return move(getConnectedComponents(mVertices.size(), calculateAdjacencyList()));
}

vector<std::vector<int>>
Graph::getConnectedComponents(const size_t size, const vector<vector<int>> &adjacencyList) {
    vector<vector<int>> connectedComponents = {};

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
}

void Graph::DFSVisit(int v, std::vector<bool> &visited, std::vector<int> &component,
                     const std::vector<std::vector<int>> &adjacencyList) {
    visited[v] = true;
    component.emplace_back(v);

    for (auto i = adjacencyList[v].begin(); i != adjacencyList[v].end(); ++i)
        if (!visited[*i])
            DFSVisit(*i, visited, component, adjacencyList);
}

void Graph::BFSVisit(int v, std::vector<bool> &visited, std::vector<int> &component,
                     const std::vector<std::vector<int>> &adjacencyList) {
    visited[v] = true;
    component.emplace_back(v);
}

std::vector<int> Graph::getVertexIndices() {
    std::vector<int> indices;
    for (Vertex vertex : mVertices)
        indices.emplace_back(vertex.index);
    return std::move(indices);
}
