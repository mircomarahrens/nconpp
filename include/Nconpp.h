#pragma once

#include <optional>
#include <set>
#include <vector>

#include "LogMessages.h"
#include "Tensor.h"
#include "TensorNetwork.h"

namespace Utils {
    template<typename D>
    std::vector<D> getIntersection(std::vector<D> vec1,
                                   std::vector<D> vec2) {
        std::sort(vec1.begin(), vec1.end());
        std::sort(vec2.begin(), vec2.end());

        std::vector<D> intersec = {};
        std::set_intersection(vec1.begin(), vec1.end(),
                              vec2.begin(), vec2.end(),
                              std::back_inserter(intersec));
        return intersec;
    }
}

class Nconpp {
public:
    Nconpp() = default;

    ~Nconpp() = default;


    template<class T>
    static npp::tensor<T> contract(std::vector<npp::array_type<T>> &tensorList,
                                   std::vector<std::vector<int>> subscriptVectorList,
                                   std::vector<int> contractionSequence = {},
                                   std::vector<int> finalOrder = {}) {

//        // retrieve unique values for legs
//        std::set<int> negLegs = {};
//        std::set<int> posLegs = {};
//
//        std::unordered_map<int, std::pair<std::size_t, std::size_t>> legsTensorPair;
//        std::pair<std::size_t, std::size_t> tensorPair(0,0);
//        std::size_t index = 0;
//        while (index < subscriptVectorList.size()) {
//            auto data = subscriptVectorList.at(index);
//            for (int ledId: data) {
//                // store negative and positive values in sets
//                if (ledId < 0) {
//                    negLegs.insert(ledId);
//                }
//                if (ledId > 0) {
//                    posLegs.insert(ledId);
//
//                    if(legsTensorPair.find(ledId) != legsTensorPair.end()) {
////                        if (legsTensorPair[ledId].second != 0) {
////                            throw std::invalid_argument(ERROR_MESSAGES::CONSTRAINT_LEGPAIRS);
////                        }
//                        legsTensorPair[ledId].second = index;
//                        tensorPair.first = 0; tensorPair.second = 0;
//                    } else {
//                        tensorPair.first = index;
//                        legsTensorPair[ledId] = tensorPair;
//                    }
//
//                }
//            }
//            index++;
//        }
//
//        // fill contraction sequence with positive legs if initially empty
//        if(contractionSequence.empty()) {
//            contractionSequence.insert(contractionSequence.begin(), posLegs.begin(), posLegs.end());
//        }
//
//        // fill final order with negative legs if initially empty
//        if (finalOrder.empty()) {
//            finalOrder.insert(finalOrder.end(), negLegs.begin(), negLegs.end());
//        }

        // TODO refactor this method into ??? --> tensorNetwork
        connectDisconnectedComponents(tensorList, subscriptVectorList, contractionSequence);

        TensorNetwork<T> tensorNetwork{tensorList, subscriptVectorList};

        auto connectedComponents = tensorNetwork.getConnectedComponents();

        return tensorNetwork.contract(contractionSequence);
    };

    template<class T>
    static void connectDisconnectedComponents(std::vector<T> &tensorList,
                                              std::vector<std::vector<int>> subscriptVectorList,
                                              std::vector<int> contractionSequence) {
//        Graph graph{tensorList.size()};
//
//        int i = 0;
//        int j = 0;
//        for (const auto &i_legs: subscriptVectorList) {
//            for (const auto &j_legs: subscriptVectorList) {
//                if (!Utils::getIntersection(i_legs, j_legs).empty())
//                    graph.addEdge(graph.getVertices()[i], graph.getVertices()[j]);
//                j++;
//            }
//            i++;
//        }
//
//        std::vector<std::vector<Vertex>> connectedComponents = graph.calculateConnectedComponents();
//
//        auto vertices = graph.getVertices();
//
//        if (connectedComponents.size() > 1u) {
//            int newLeg;
//            Vertex currentVertex, lastVertex;
//            enum class st {
//                EVEN, ODD
//            };
//            st s = st::EVEN;
//            auto it = connectedComponents.begin();
//            while (it != connectedComponents.end()) {
//                int index = getShortestOfSubscriptVectorList(*it, subscriptVectorList);
//                currentVertex = vertices[index];
//                switch (s) {
//                    case (st::EVEN):
//                        newLeg = getNewLeg(contractionSequence);
//                        contractionSequence.push_back(newLeg);
//                        s = st::ODD;
//                        break;
//                    case (st::ODD):
//                        graph.addEdge(lastVertex, currentVertex);
//                        s = st::EVEN;
//                        break;
//                }
//
//                expandRawTensorNetwork(currentVertex.index, newLeg, tensorList, subscriptVectorList);
//
//                lastVertex = currentVertex;
//                it++;
//            }
//
//            if (s == st::ODD) {
//                // don't consider the last element, as this needs to be connected
//                std::vector<std::vector<int>> legsSubList(
//                        subscriptVectorList.begin(), subscriptVectorList.end() - 1);
//
//                graph.removeVertex(currentVertex);
//
//                std::vector<int> subComponents = graph.getVertexIndices();
//
//                int currentVertexIndex = getShortestOfSubscriptVectorList(
//                        subComponents,
//                        subscriptVectorList);
//
//                currentVertex = graph.getVertices()[currentVertexIndex];
//
//                graph.addEdge(lastVertex, currentVertex);
//
//                expandRawTensorNetwork(currentVertex.index, newLeg, tensorList, subscriptVectorList);
//            }
//        }
    };

private:
    // Network operations
    static int getShortestOfSubscriptVectorList(const std::vector<int> &indexSet,
                                                const std::vector<std::vector<int>> &subscriptVectorList) {
        int shortest = 0;
        for (int index: indexSet)
            if (subscriptVectorList[index].size() < subscriptVectorList[shortest].size())
                shortest = index;
        return shortest;
    };

    static int getLongestOfSubscriptVectorList(const std::vector<int> &indexSet,
                                               const std::vector<std::vector<int>> &subscriptVectorList) {
        int longest = 0;
        for (int index: indexSet)
            if (subscriptVectorList[index].size() > subscriptVectorList[longest].size())
                longest = index;
        return longest;
    };

    static int getNewLeg(const std::vector<int> &contractionSequenceLegs) {
        int newLeg = *std::max_element(
                contractionSequenceLegs.begin(),
                contractionSequenceLegs.end());
        newLeg += 1;
        return newLeg;
    };

    template<class T>
    static void expandRawTensorNetwork(int vertexIndex,
                                       int legIndex,
                                       std::vector<npp::tensor<T>> &tensorList,
                                       std::vector<std::vector<int>> &subscriptVectorList) {

        auto &container_type = tensorList[vertexIndex];

        std::size_t dim = npp::dimension(container_type);
        container_type = npp::expand_dims(container_type, dim);

        subscriptVectorList[vertexIndex].push_back(legIndex);

    };
};
