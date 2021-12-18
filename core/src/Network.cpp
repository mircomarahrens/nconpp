#include "Network.h"
#include "Search.h"

Network::Network(std::vector<std::vector<int>> vertexLegs)
    : mVertexLegs{ vertexLegs },
    Graph(vertexLegs.size())
{
    generateEdges(vertexLegs);
}

void Network::addEdge(int src, int dest)
{
    Graph::addEdge(src, dest);

    std::vector<int>& vertexSrc = mVertexLegs[src];
    std::vector<int>& vertexDest = mVertexLegs[dest];

    if (Container::getIntersection(vertexSrc, vertexDest).empty())
    {
        auto legs = Container::allUniqueIntegersSorted(mVertexLegs);
        int newLegIndex = 0;
        if (!legs.empty())
        {
            auto ind = std::max_element(legs.begin(), legs.end());
            newLegIndex = *ind + 1;
        }
        vertexSrc.push_back(newLegIndex);
        vertexDest.push_back(newLegIndex);
    }
}

void Network::removeEdge(int src, int dest)
{
    Graph::removeEdge(src, dest);

    auto& vertexSrc = mVertexLegs[src];
    auto& vertexDest = mVertexLegs[dest];

    auto intersec = Container::getIntersection(vertexSrc, vertexDest);
    for (int val : intersec)
    {
        auto pos_src = std::find(vertexSrc.begin(), vertexSrc.end(), val);
        vertexSrc.erase(pos_src);
        auto pos_dest = std::find(vertexDest.begin(), vertexDest.end(), val);
        vertexSrc.erase(pos_dest);
    }
}

void Network::addLeg(int newLeg, int node)
{
    mVertexLegs[node].push_back(newLeg);
}

const std::vector<std::vector<int>>& Network::getVertexLegs()
{
    return mVertexLegs;
}

void Network::calculateConnectedComponents()
{
    Search::connectedComponents(Graph::getVertices().size(),
        Graph::getAdjanceyList(), mConnectedComponents);
}

const std::vector<std::set<int>>& Network::getConnectedComponents()
{
    if (mConnectedComponents.empty())
        calculateConnectedComponents();
    return mConnectedComponents;
}

void Network::generateEdges(const std::vector<std::vector<int>>& vertexLegs)
{
    for (auto& i_legs : vertexLegs)
    {
        int i_node = *Container::getIndexToElement(vertexLegs, i_legs);
        for (auto& j_legs : vertexLegs)
        {
            int j_node = *Container::getIndexToElement(vertexLegs, j_legs);
            if (i_node != j_node)
            {
                const auto vec1 = vertexLegs[i_node];
                const auto vec2 = vertexLegs[j_node];
                if (!Container::getIntersection(vec1, vec2).empty())
                    Graph::addEdge(i_node, j_node);
            }
        }
    }
}
