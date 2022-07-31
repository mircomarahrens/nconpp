#include "Nconpp.h"

#include <vector>

template<class T>
T Nconpp::contract(
        std::vector<T> &tensorList,
        std::vector<std::vector<int>> subscriptVectorList,
        std::vector<int> contractionSequence,
        std::vector<int> finalOrder) {

    auto legIndices = retrieveLegIndices(subscriptVectorList);

    if (contractionSequence.empty())
        fillContractionSequence(contractionSequence, legIndices);

    if (finalOrder.empty())
        fillFinalOrder(finalOrder, legIndices);

    // TODO refactor this method into ???
    connectDisconnectedComponents(tensorList, subscriptVectorList, contractionSequence);

    TensorNetwork tensorNetwork{tensorList, subscriptVectorList};

    return tensorNetwork.contract(contractionSequence);
}

void Nconpp::fillContractionSequence(std::vector<int> &contractionSequence, const std::vector<int> &legIndices) {
    contractionSequence.assign(legIndices.begin(), legIndices.end());

    contractionSequence.erase(
            std::remove_if(
                    contractionSequence.begin(), contractionSequence.end(), isNegative),
            contractionSequence.end());

}

void Nconpp::fillFinalOrder(std::vector<int> &finalOrder, const std::vector<int> &legIndices) {
    finalOrder.assign(legIndices.begin(), legIndices.end());

    finalOrder.erase(
            std::remove_if(
                    finalOrder.begin(), finalOrder.end(), isPositive),
            finalOrder.end());

    std::reverse(finalOrder.begin(), finalOrder.end());
}

std::vector<int> Nconpp::retrieveLegIndices(const std::vector <std::vector<int>> &subscriptVectorList) {

    std::vector<int> legIndices;
    for (std::vector<int> data: subscriptVectorList) {
        std::sort(data.begin(), data.end());
        std::set_union(data.begin(), data.end(),
                       legIndices.begin(), legIndices.end(),
                       std::inserter(legIndices, legIndices.end()));
    }

    return std::move(legIndices);
}

template<class T>
void
Nconpp::connectDisconnectedComponents(std::vector<T> &tensorList, std::vector<std::vector<int>> subscriptVectorList,
                                      std::vector<int> contractionSequence) {
    Graph graph{tensorList.size()};

    int i = 0;
    int j = 0;
    for (const auto& i_legs: subscriptVectorList) {
        for (const auto& j_legs: subscriptVectorList) {
            if (!Utils::getIntersection(i_legs, j_legs).empty())
                graph.constructEdge(graph.getVertices()[i], graph.getVertices()[j]);
            j++;
        }
        i++;
    }

    std::vector<std::vector<int>> connectedComponents = graph.calculateConnectedComponents();

    auto vertices = graph.getVertices();

    if (connectedComponents.size() > 1u) {
        int newLeg;
        Vertex currentVertex, lastVertex;
        enum class st {
            EVEN, ODD
        };
        st s = st::EVEN;
        auto it = connectedComponents.begin();
        while (it != connectedComponents.end()) {
            int index = getShortestOfSubscriptVectorList(*it, subscriptVectorList);
            currentVertex = vertices[index];
            switch (s) {
                case (st::EVEN):
                    newLeg = getNewLeg(contractionSequence);
                    contractionSequence.push_back(newLeg);
                    s = st::ODD;
                    break;
                case (st::ODD):
                    graph.constructEdge(lastVertex, currentVertex);
                    s = st::EVEN;
                    break;
            }

            expandRawTensorNetwork(currentVertex.index, newLeg, tensorList, subscriptVectorList);

            lastVertex = currentVertex;
            it++;
        }

        if (s == st::ODD) {
            // don't consider the last element, as this needs to be connected
            std::vector<std::vector<int>> legsSubList(
                    subscriptVectorList.begin(), subscriptVectorList.end() - 1);

            graph.removeVertex(currentVertex);

            std::vector<int> subComponents = graph.getVertexIndices();

            int currentVertexIndex = getShortestOfSubscriptVectorList(
                    subComponents,
                    subscriptVectorList);

            currentVertex = graph.getVertices()[currentVertexIndex];

            graph.constructEdge(lastVertex, currentVertex);

            expandRawTensorNetwork(currentVertex.index, newLeg, tensorList, subscriptVectorList);
        }
    }
}

int Nconpp::getShortestOfSubscriptVectorList(const std::vector<int> &indexSet,
                                             const std::vector<std::vector<int>> &subscriptVectorList) {
    int shortest = 0;
    for (int index: indexSet)
        if (subscriptVectorList[index].size() < subscriptVectorList[shortest].size())
            shortest = index;
    return shortest;
}

int Nconpp::getLongestOfSubscriptVectorList(const std::vector<int> &indexSet,
                                            const std::vector<std::vector<int>> &subscriptVectorList) {
    int longest = 0;
    for (int index: indexSet)
        if (subscriptVectorList[index].size() > subscriptVectorList[longest].size())
            longest = index;
    return longest;
}

int Nconpp::getNewLeg(const std::vector<int> &contractionSequenceLegs) {
    int newLeg = *std::max_element(
            contractionSequenceLegs.begin(),
            contractionSequenceLegs.end());
    newLeg += 1;
    return newLeg;
}

template<class T>
void Nconpp::expandRawTensorNetwork(
        int vertexIndex,
        int legIndex,
        std::vector<T> &tensorList,
        std::vector<std::vector<int>> &subscriptVectorList) {

    T &container_type = tensorList[vertexIndex];
    size_t dim = TensorOperations::dimension(container_type);
    container_type =
            TensorOperations::expand_dims(container_type, dim);

    subscriptVectorList[vertexIndex].push_back(legIndex);
}
