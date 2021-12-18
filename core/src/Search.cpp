#include "Search.h"

using namespace std;

void Search::connectedComponents(const size_t size,
    const vector<vector<int>>& adjanceyList,
    vector<set<int>>& connectedComponents)
{
    vector<bool> visited(size);
    for (int v = 0; v < size; v++)
        visited[v] = false;

    set<int> component;
    for (int v = 0; v < size; v++)
    {
        if (visited[v] == false)
        {
            // depth first search
            DFSUtil(v, visited, component, adjanceyList);
            connectedComponents.emplace_back(component);
            component.clear();
        }
    }
};

void Search::DFSUtil(int v,
    vector<bool>& visited,
    set<int>& component,
    const vector<vector<int>>& adjanceyList)
{
    visited[v] = true;
    component.insert(v);

    for (auto i = adjanceyList[v].begin(); i != adjanceyList[v].end(); ++i)
        if (!visited[*i])
            DFSUtil(*i, visited, component, adjanceyList);
};
