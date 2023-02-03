#pragma once

#include <optional>
#include <set>
#include <vector>

#include "Tensor.h"
#include "TensorNetwork.h"

class Nconpp {
public:
    Nconpp() = default;

    ~Nconpp() = default;

    // contracts multiple tensors to one tensor, if possible.
    //
    // @params:
    //  tensorList:
    //      a list of tensors
    //  subscriptVectorList:
    //      - aka LegsList
    //      - Nomenclature of the legs of the tensor in tensorList:
    //          - the legs are named by integers
    //          - contractable legs have the same positive integer as name,
    //            hence occuring in pairs
    //          - legs with negative integers won't be contracted, so called
    //            dangling legs
    //  contractionSequenceLegs (optional):
    //      order by legs in which the tensors shall be contracted
    //  finalOrder (optional):
    //      Permutation of the legs of the final tensor.
    //
    // @return:
    //  the final contracted tensor
    template<class T>
    static Tensor<T> contract(
            std::vector<Tensor<T>> &tensorList,
            std::vector<std::vector<int>> subscriptVectorList,
            std::vector<int> contractionSequence = {},
            std::vector<int> finalOrder = {}) {

        auto legIndices = retrieveLegIndices(subscriptVectorList);

        if (contractionSequence.empty())
            fillContractionSequence(contractionSequence, legIndices);

        if (finalOrder.empty())
            fillFinalOrder(finalOrder, legIndices);

        // TODO refactor this method into ???
        connectDisconnectedComponents(tensorList, subscriptVectorList, contractionSequence);

        TensorNetwork tensorNetwork{tensorList, subscriptVectorList};

        return tensorNetwork.contract(contractionSequence);
    };

    template<class T>
    static void connectDisconnectedComponents(
            std::vector<T> &tensorList,
            std::vector<std::vector<int>> subscriptVectorList,
            std::vector<int> contractionSequence) {
        Graph graph{tensorList.size()};

        int i = 0;
        int j = 0;
        for (const auto &i_legs: subscriptVectorList) {
            for (const auto &j_legs: subscriptVectorList) {
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
    };

private:
    static void fillContractionSequence(std::vector<int> &contractionSequence, const std::vector<int> &legIndices) {
        contractionSequence.assign(legIndices.begin(), legIndices.end());

        contractionSequence.erase(
                std::remove_if(
                        contractionSequence.begin(), contractionSequence.end(), isNegative),
                contractionSequence.end());

    };

    static void fillFinalOrder(std::vector<int> &finalOrder, const std::vector<int> &legIndices) {
        finalOrder.assign(legIndices.begin(), legIndices.end());

        finalOrder.erase(
                std::remove_if(
                        finalOrder.begin(), finalOrder.end(), isPositive),
                finalOrder.end());

        std::reverse(finalOrder.begin(), finalOrder.end());
    };

    static std::vector<int> retrieveLegIndices(const std::vector<std::vector<int>> &subscriptVectorList) {

        std::vector<int> legIndices = {};
        for (std::vector<int> data: subscriptVectorList) {
            std::sort(data.begin(), data.end());
            std::set_union(data.begin(), data.end(),
                           legIndices.begin(), legIndices.end(),
                           std::inserter(legIndices, legIndices.end()));
        }

        return std::move(legIndices);
    };

    static bool isNegative(int i) {
        return i < 0;
    };

    static bool isPositive(int i) {
        return i > 0;
    };

    // Network operations
    static int getShortestOfSubscriptVectorList(
            const std::vector<int> &indexSet,
            const std::vector<std::vector<int>> &subscriptVectorList) {
        int shortest = 0;
        for (int index: indexSet)
            if (subscriptVectorList[index].size() < subscriptVectorList[shortest].size())
                shortest = index;
        return shortest;
    };

    static int getLongestOfSubscriptVectorList(
            const std::vector<int> &indexSet,
            const std::vector<std::vector<int>> &subscriptVectorList) {
        int longest = 0;
        for (int index: indexSet)
            if (subscriptVectorList[index].size() > subscriptVectorList[longest].size())
                longest = index;
        return longest;
    };

    static int getNewLeg(
            const std::vector<int> &contractionSequenceLegs) {
        int newLeg = *std::max_element(
                contractionSequenceLegs.begin(),
                contractionSequenceLegs.end());
        newLeg += 1;
        return newLeg;
    };

    template<class T>
    static void expandRawTensorNetwork(
            int vertexIndex,
            int legIndex,
            std::vector<T> &tensorList,
            std::vector<std::vector<int>> &subscriptVectorList
    ) {

//    T &container_type = tensorList[vertexIndex];
//    size_t dim = container_type.dimension(container_type);
//    container_type = container_type.expand_dims(dim);
//    subscriptVectorList[vertexIndex].push_back(legIndex);
    };
};
