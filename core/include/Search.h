#pragma once

#include <vector>
#include <tuple>
#include <set>

struct Search
{
public:
    static void connectedComponents(const size_t size,
        const std::vector<std::vector<int>>& adjanceyList,
        std::vector<std::set<int>>& connectedComponents);

private:
    static void DFSUtil(int v,
        std::vector<bool>& visited,
        std::set<int>& component,
        const std::vector<std::vector<int>>& adjanceyList);
};
