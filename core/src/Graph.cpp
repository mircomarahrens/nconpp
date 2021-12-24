#include "utils/Container.h"
#include "Graph.h"

using namespace std;

Graph::Graph(size_t N) : mVertices(Container::createRangeSet(N))
{
    mAdjanceyList = vector<vector<int>>(N);
}

void Graph::addEdge(int src, int dest)
{
    if ((size_t)src < mVertices.size() && ((size_t)dest < mVertices.size()))
    {
        mAdjanceyList[src].emplace_back(dest);
        mAdjanceyList[dest].emplace_back(src);
    }
}

void Graph::removeEdge(int src, int dest)
{
    if ((size_t)src < mVertices.size())
    {
        auto end = mAdjanceyList[src].end();
        auto pos = find(mAdjanceyList[src].begin(), end, dest);
        if (pos != end)
            mAdjanceyList[src].erase(pos);
    }
}

void Graph::addVertex(int vertex)
{
    mVertices.insert(vertex);
}

void Graph::removeVertex(int vertex)
{
    auto pos = mVertices.find(vertex);
    mVertices.erase(pos);
}

const vector<vector<int>>& Graph::getAdjanceyList()
{
    return mAdjanceyList;
}

const set<int>& Graph::getVertices()
{
    return mVertices;
}
