#include "Graph.h"

#include "Container.h"

using namespace std;

Graph::Graph(size_t N) : mVertexIndices(Container::createRangeSet(N)) {
    mAdjacencyList = vector<vector<int>>(N);
}

void Graph::addEdge(int src, int dest) {
    if ((size_t) src < mVertexIndices.size() && ((size_t) dest < mVertexIndices.size())) {
        mAdjacencyList[src].emplace_back(dest);
        mAdjacencyList[dest].emplace_back(src);
    }
}

void Graph::addEdge(const Edge &edge) {
    addEdge(edge.src, edge.dest);
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
    removeEdgeByIndices(edge.src, edge.dest);
}

void Graph::addVertexIndex(int vertexIndex) {
    mVertexIndices.insert(vertexIndex);
}

void Graph::addVertex(const Vertex &vertex) {
    mVertices.insert(vertex);
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

const std::set<Vertex> &Graph::getVertices() {
    return mVertices;
}
