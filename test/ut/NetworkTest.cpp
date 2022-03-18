#include <gtest/gtest.h>

#include "Network.h"

#include <vector>
#include <set>
#include <iostream>

class NetworkTest : public testing::Test
{
    NetworkTest()= default;;
    ~NetworkTest() override= default;;
};

TEST(NetworkTest, graph_test)
{
    // 5 vertices numbered from 0 to 4 without legs
    Network network({{}, {}, {}, {}, {}});

    // create edges
    network.addEdge(1, 0);
    network.addEdge(2, 3);
    network.addEdge(3, 4);

    network.calculateConnectedComponents();
    std::vector<std::set<int>> connectedComponents =
        network.getConnectedComponents();

    ASSERT_TRUE(std::find(connectedComponents.begin(),
                          connectedComponents.end(),
                          std::set<int>{0, 1}) != connectedComponents.end());
    ASSERT_TRUE(std::find(connectedComponents.begin(),
                          connectedComponents.end(),
                          std::set<int>{2, 3, 4}) != connectedComponents.end());
}
