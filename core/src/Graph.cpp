#include "Graph.h"

#include "Container.h"

using namespace std;

Graph::Graph(size_t N) {
    for (int i = 0; i < N; i++) {
        Vertex vertex{i};
        mVertices.emplace_back(vertex);
    }
}

void Graph::addEdge(const Vertex &src, const Vertex &dest) {
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

void Graph::removeEdge(int src, int dest) {
    auto pos = std::find_if(mEdges.begin(),
                            mEdges.end(),
                            [&csrc = src, &cdest = dest]
                                    (const Edge &edge) -> bool {
                                return (csrc == edge.src.index && cdest == edge.dest.index);
                            });
    mEdges.erase(pos);
}

void Graph::removeEdge(const Vertex &src, const Vertex &dest) {
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
                                return (cedge.src.index == edge.src.index && edge.dest.index == edge.dest.index);
                            });
    mEdges.erase(pos);
}

void Graph::addVertex(const Vertex &vertex) {
    mVertices.emplace_back(vertex);
}

void Graph::removeVertex(int index) {
    auto pos = std::find_if(mVertices.begin(),
                            mVertices.end(),
                            [&cindex = index]
                                    (const Vertex &vertex) -> bool {
                                return (cindex == vertex.index);
                            });
    mVertices.erase(pos);
}

void Graph::removeVertex(const Vertex &vertex) {
    removeVertex(vertex.index);
}

const std::vector<Edge> &Graph::getEdges() {
    return mEdges;
}

const std::vector<Vertex> &Graph::getVertices() {
    return mVertices;
}

const set<int> &Graph::getVertexIndices() {
    std::set<int> vertexIndices = {};
    for (Vertex vertex: mVertices)
        vertexIndices.insert(vertex.index);
    return move(vertexIndices);
}
