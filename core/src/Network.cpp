#include "Network.h"

#include "Algorithms.h"
#include "Container.h"

using namespace std;

Network::Network(const vector<vector<int>>& vertexLegs)
        : mVertexLegs{vertexLegs},
          Graph(vertexLegs.size()) {
    generateEdges(vertexLegs);
}

void Network::addEdge(int src, int dest) {
    Graph::addEdge(src, dest);

    vector<int> &vertexSrc = mVertexLegs[src];
    vector<int> &vertexDest = mVertexLegs[dest];

    if (Container::getIntersection(vertexSrc, vertexDest).empty()) {
        auto legs = Container::allUniqueIntegersSorted(mVertexLegs);
        int newLegIndex = 0;
        if (!legs.empty()) {
            auto ind = max_element(legs.begin(), legs.end());
            newLegIndex = *ind + 1;
        }
        vertexSrc.push_back(newLegIndex);
        vertexDest.push_back(newLegIndex);
    }
}

void Network::removeEdgeByIndices(int src, int dest) {
    Graph::removeEdgeByIndices(src, dest);

    auto &vertexSrc = mVertexLegs[src];
    auto &vertexDest = mVertexLegs[dest];

    auto intersection = Container::getIntersection(vertexSrc, vertexDest);
    for (int val: intersection) {
        auto pos_src = find(vertexSrc.begin(), vertexSrc.end(), val);
        vertexSrc.erase(pos_src);
        auto pos_dest = find(vertexDest.begin(), vertexDest.end(), val);
        vertexSrc.erase(pos_dest);
    }
}

void Network::addLeg(int newLeg, int node) {
    mVertexLegs[node].push_back(newLeg);
}

void Network::addLeg(const Leg& leg) {
    mVertexLegs[leg.vertex->index].push_back(leg.index);
}

const vector<vector<int>> &Network::getVertexLegs() {
    return mVertexLegs;
}

void Network::generateEdges(const vector<vector<int>> &vertexLegs) {
    for (auto &i_legs: vertexLegs) {
        int i_node = *Container::getIndexToElement(vertexLegs, i_legs);
        for (auto &j_legs: vertexLegs) {
            int j_node = *Container::getIndexToElement(vertexLegs, j_legs);
            if (i_node != j_node) {
                const auto vec1 = vertexLegs[i_node];
                const auto vec2 = vertexLegs[j_node];
                if (!Container::getIntersection(vec1, vec2).empty())
                    Graph::addEdge(i_node, j_node);
            }
        }
    }
}

void Network::addEdge(const Edge &edge) {
    Graph::addEdge(edge);
}
