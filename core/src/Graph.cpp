#include "Graph.h"

#include "Container.h"

using namespace std;

Graph::Graph(size_t N) : mVertexIndices(Container::createRangeSet(N)) {
    for (int i = 0; i < N; i++) {
        Vertex vertex{i};
        mVertices.emplace_back(vertex);
    }

    mAdjacencyList = vector<vector<int>>(N);
}

void Graph::addEdge(int src, int dest) {
    if ((size_t) src < mVertexIndices.size() && ((size_t) dest < mVertexIndices.size())) {
        mAdjacencyList[src].emplace_back(dest);
        mAdjacencyList[dest].emplace_back(src);
    }
}

void Graph::addEdge(const Vertex &src, const Vertex &dest) {
    Edge edge{src, dest};
    mEdges.emplace_back(edge);
    addEdge(edge.src.index, edge.dest.index);
}

void Graph::addEdge(const Edge &edge) {
    mEdges.emplace_back(edge);
    addEdge(edge.src.index, edge.dest.index);
}

void Graph::removeEdgeByIndices(int src, int dest) {
    if ((size_t) src < mVertexIndices.size()) {
        auto end = mAdjacencyList[src].end();
        auto pos = find(mAdjacencyList[src].begin(), end, dest);
        if (pos != end)
            mAdjacencyList[src].erase(pos);
    }
}

void Graph::removeEdge(const Edge &edge) {
    removeEdgeByIndices(edge.src.index, edge.dest.index);
}

void Graph::addVertexIndex(int vertexIndex) {
    mVertexIndices.insert(vertexIndex);
}

void Graph::addVertex(const Vertex &vertex) {
    mVertices.emplace_back(vertex);
    addVertexIndex(vertex.index);
}

void Graph::removeVertexByIndex(int index) {
    auto pos = mVertexIndices.find(index);
    mVertexIndices.erase(pos);
}

void Graph::removeVertex(const Vertex &vertex) {
    removeVertexByIndex(vertex.index);
}

const set<int> &Graph::getVertexIndices() {
    return mVertexIndices;
}

const std::vector<std::vector<int>> &Graph::getAdjacencyList() {
    return mAdjacencyList;
}

const std::vector<Vertex> &Graph::getVertices() {
    return mVertices;
}

const std::vector<Edge> &Graph::getEdges() {
    return mEdges;
}
