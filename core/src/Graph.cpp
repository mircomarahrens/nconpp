#include "Graph.h"

#include "Utils.h"

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

void Graph::addEdge(const Edge &edge) {
    if (std::find(mVertices.begin(), mVertices.end(), edge.src) != mVertices.end() &&
        std::find(mVertices.begin(), mVertices.end(), edge.dest) != mVertices.end()) {
        mEdges.emplace_back(edge);
    }
}

void Graph::destructEdges(const Vertex &src, const Vertex &dest) {
    auto pos = std::find_if(mEdges.begin(),
                            mEdges.end(),
                            [&csrc = src, &cdest = dest]
                                    (const Edge &edge) -> bool { return (csrc == edge.src && cdest == edge.dest); });
    mEdges.erase(pos);
}

void Graph::removeEdge(const Edge &edge) {
    auto pos = std::find_if(mEdges.begin(),
                            mEdges.end(),
                            [&cedge = edge]
                                    (const Edge &edge) -> bool {
                                return (cedge.src.index == edge.dest.index);
                            });
    mEdges.erase(pos);
}

void Graph::addVertex(const Vertex &vertex) {
    mVertices.emplace_back(vertex);
}

void Graph::removeVertex(const Vertex &vertex) {
    auto pos = std::find_if(mVertices.begin(),
                            mVertices.end(),
                            [&cvertex = vertex]
                                    (const Vertex &vertex) -> bool {
                                return (cvertex == vertex);
                            });
    mVertices.erase(pos);
}

const std::vector<Edge> &Graph::getEdges() {
    return mEdges;
}

const vector<Vertex> &Graph::getVertices() {
    return mVertices;
}

// TODO I should store this directly from the beginning
const vector<std::vector<int>> &Graph::getAdjacencyList() {
    std::vector<std::vector<int>> adjacencyList(mVertices.size());
    for (auto e: mEdges)
        adjacencyList[e.src.index].emplace_back(e.dest.index);
    return std::move(adjacencyList);
}

// TODO maybe there is a way to update this on the fly?
const std::vector<std::set<int>> &Graph::getConnectedComponentsIndices() {
    return std::move(getConnectedComponents(mVertices.size(), getAdjacencyList()));
}

std::vector<std::set<int>>
Graph::getConnectedComponents(const std::size_t size, const std::vector<std::vector<int>> &adjacencyList) {
    std::vector<std::set<int>> connectedComponents = {};

    std::vector<bool> visited(size);
    for (int v = 0; v < size; v++)
        visited[v] = false;

    std::set<int> component;
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

void Graph::DFSVisit(int v, std::vector<bool> &visited, std::set<int> &component,
                     const std::vector<std::vector<int>> &adjacencyList) {
    visited[v] = true;
    component.insert(v);

    for (auto i = adjacencyList[v].begin(); i != adjacencyList[v].end(); ++i)
        if (!visited[*i])
            DFSVisit(*i, visited, component, adjacencyList);
}

void Graph::BFSVisit(int v, std::vector<bool> &visited, std::set<int> &component,
                     const std::vector<std::vector<int>> &adjacencyList) {
    visited[v] = true;
    component.insert(v);
}
