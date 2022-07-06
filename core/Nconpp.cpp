#include "Nconpp.h"

#include "include/Utils.h"
#include "include/ErrorMessages.h"

#include <set>
#include <stdexcept>
#include <vector>

using namespace std;

template<class T>
T Nconpp::contract(
        vector<T> &tensorList,
        vector<vector<int>> subscriptVectorList,
        bool skipInputValidation,
        std::vector<int> contractionSequence,
        std::vector<int> finalOrder) {

    if (!skipInputValidation)
        validateInput(
                tensorList,
                subscriptVectorList,
                contractionSequence,
                finalOrder
        );

    // TODO move this to Network or maybe to NetworkAlgorithms?
    connectDisconnectedComponents(
            tensorList,
            subscriptVectorList,
            contractionSequence
    );

    while (!contractionSequence.empty()) {
        auto it = contractionSequence.begin();
        // 1. get contraction parameters:
        // TODO add multi index contraction
        auto contractionParams = findContractionParameters(
                *it,
                subscriptVectorList);

        size_t indexA = get<0>(contractionParams).first;
        const auto &axisA = get<0>(contractionParams).second;
        size_t indexB = get<1>(contractionParams).first;
        const auto &axisB = get<1>(contractionParams).second;

        // 2. perform contraction of tensors
        if (indexA == indexB) // do trace
        {
            doTrace(
                    indexA,
                    indexB,
                    axisA,
                    axisB,
                    tensorList,
                    subscriptVectorList);
        } else // do tensor product
        {
            doTensorProduct(
                    indexA,
                    indexB,
                    axisA,
                    axisB,
                    tensorList,
                    subscriptVectorList);
        }
        contractionSequence
                .erase(contractionSequence.begin());
    }

    return std::move(tensorList[0]);
}

template<class T>
void Nconpp::validateInputData(
        const vector<T> &tensorList,
        vector<vector<int>> &subscriptVectorList,
        std::vector<int> &contractionSequence,
        std::vector<int> &finalOrder) {
    if (subscriptVectorList.empty())
        throw invalid_argument("LegsList empty. You need to specify a list of legs corresponding to your network.");

    // check sizes of container
    if (tensorList.size() != subscriptVectorList.size()) {
        throw invalid_argument(
                "The number of tensors, which is " +
                to_string(tensorList.size()) +
                ", does not match the number of legs, which is " +
                to_string(subscriptVectorList.size()) + ".");
    }

    // check subscript vectors

    // if empty fill with defaults
    if (contractionSequence.empty()) {
        auto conSet = Utils::allUniqueSorted(subscriptVectorList);
        contractionSequence.assign(conSet.begin(), conSet.end());
        Utils::removeNegatives(contractionSequence);
    }

    if (finalOrder.empty()) {
        auto conSet = Utils::allUniqueSorted(subscriptVectorList);
        finalOrder.assign(conSet.begin(), conSet.end());
        Utils::removePositives(finalOrder);
        reverse(finalOrder.begin(), finalOrder.end());
    }
}

template<class T>
void Nconpp::connectDisconnectedComponents(
        vector<T> &tensorList,
        vector<vector<int>> &subscriptVectorList,
        vector<int> &contractionSequence) {

    TensorNetwork network{subscriptVectorList};
    auto connectedComponents = network.getConnectedComponentsIndices();

    // TODO connect components
    if (connectedComponents.size() > 1u) {
        Vertex lastVertex{0}, currentVertex;
        int newLeg;
        enum class parity {
            EVEN, ODD
        };
        parity p = parity::EVEN;
        auto it = connectedComponents.begin();
        while (it != connectedComponents.end()) {
            int tmp = getShortestOfLegsList(*it, subscriptVectorList);
            currentVertex = Vertex{tmp};
            switch (p) {
                case (parity::EVEN):
                    newLeg = getNewLeg(contractionSequence);
                    contractionSequence.push_back(newLeg);
                    p = parity::ODD;
                    break;
                case (parity::ODD):
                    network.constructEdge(lastVertex, currentVertex);
                    p = parity::EVEN;
                    break;
            }

            expandNetwork(currentVertex, newLeg, tensorList, subscriptVectorList);

            lastVertex = currentVertex;
            it++;
        }

        if (p == parity::ODD) {
            // don't consider the last element, as this needs to be connected
            vector<vector<int>> legsSubList(
                    subscriptVectorList.begin(), subscriptVectorList.end() - 1);
            set<int> subComponents = network.getVertexIndices();
            subComponents.erase(currentVertex);

            currentVertex = getShortestOfLegsList(
                    subComponents,
                    legsSubList);

            network.constructEdge(lastVertex, currentVertex);

            expandNetwork(currentVertex, newLeg, tensorList, subscriptVectorList);
        }
    }
}


template<class T>
void Nconpp::expandNetwork(
        int node,
        int leg,
        vector<T> &containerList,
        vector<vector<int>> &legsList) {
    T &container_type = containerList[node];
    size_t dim = Tensor::dimension(container_type);
    container_type =
            Tensor::expand_dims(container_type, dim);
    legsList[node].push_back(leg);
}

template<class T>
void Nconpp::doTrace(
        size_t indexA,
        size_t indexB,
        const vector<size_t> &axisA,
        const vector<size_t> &axisB,
        // TODO network
        vector<T> &containerList,
        vector<vector<int>> &legsList) {
    auto newTensor = Tensor::trace(
            containerList[indexA], 0, axisA[0], axisB[0]);

    containerList.erase(containerList.begin() + indexA);
    containerList.emplace_back(newTensor);

    auto legs = legsList[indexA];

    // remove indices from legs
    const vector<size_t> indices{axisA[0], axisB[0]};
    // TODO recheck
    //  -> remove elements by reverse iteration
    //  -> and by iterator not by index
    Utils::removeIndicesFromVector(legs, indices);

    // remove old legs
    legsList.erase(legsList.begin() + indexA);
    legsList.emplace_back(legs);
}

template<class T>
void Nconpp::doTensorProduct(
        size_t indexA,
        size_t indexB,
        const vector<size_t> &axisA,
        const vector<size_t> &axisB,
        // TODO network
        vector<T> &containerList,
        vector<vector<int>> &legsList) {
    const auto &newTensor = Tensor::tensordot(
            containerList[indexA], containerList[indexB], axisA, axisB);

    const vector<size_t> indices{indexA, indexB};

    Utils::removeIndicesFromVector(containerList, indices);
    containerList.emplace_back(newTensor);

    auto legsA = legsList[indexA];
    auto legsB = legsList[indexB];

    Utils::removeIndicesFromVector(legsA, axisA);
    Utils::removeIndicesFromVector(legsB, axisB);

    legsA.insert(legsA.end(), legsB.begin(), legsB.end());

    Utils::removeIndicesFromVector(legsList, indices);

    legsList.emplace_back(legsA);
}

auto Nconpp::findContractionParameters(
        int contractionLeg,
        vector<vector<int>> &legsList) {
    // index to tensor, position of legs to contract
    pair<size_t, vector<size_t>> tensorParamsA, tensorParamsB;
    tensorParamsA.second = {};
    tensorParamsB.second = {};
    bool visitA = false, visitB = false;
    for (int ind = 0; ind < legsList.size(); ++ind) {
        for (int leg: legsList[ind]) {
            if (contractionLeg == leg) {
                int legIndex;
                if (!visitA) {
                    tensorParamsA.first = ind;
                    legIndex = *Utils::getIndexToElement(legsList[ind], leg);
                    tensorParamsA.second.emplace_back((size_t) legIndex);
                    visitA = true;
                } else if (visitA && !visitB) {
                    tensorParamsB.first = ind;
                    if (tensorParamsB.first == tensorParamsA.first) // trace
                        legIndex = *Utils::getIndexToElement(legsList[ind], leg, (int) tensorParamsA.second[0] + 1);
                    else // tensor product
                        legIndex = *Utils::getIndexToElement(legsList[ind], leg);
                    tensorParamsB.second.emplace_back((size_t) legIndex);
                    visitB = true;
                } else if (visitA && visitB)
                    throw logic_error(ERROR_MESSAGES::MISMATCH);
            }
        }
    }
    return make_tuple(tensorParamsA, tensorParamsB);
}

template<class T>
void Nconpp::decompose(
        T &tensor,
        int decompositionIndex,
        bool skipInputValidation,
        std::vector<int> decompositionSequenceLegs,
        std::vector<int> finalOrder) {

    auto sh = Tensor::shape(tensor);
    // TODO
    //auto left = sh[:decompositionIndex];
    //auto right = sh[decompositionIndex:];
    //auto mat = Tensor::reshape(tensor, {Tensor::prod(left), Tensor::prod(right)});
    //Tensor::svd();
}


// Network operations
int Nconpp::getShortestOfLegsList(const set<int> &indexSet,
                                  const vector<vector<int>> &legsList) {
    int shortest = 0;
    for (int index: indexSet)
        if (legsList[index].size() < legsList[shortest].size())
            shortest = index;
    return shortest;
}

int Nconpp::getLongestOfLegsList(const set<int> &indexSet,
                                 const vector<vector<int>> &legsList) {
    int longest = 0;
    for (int index: indexSet)
        if (legsList[index].size() > legsList[longest].size())
            longest = index;
    return longest;
}

int Nconpp::getNewLeg(const vector<int> &contractionSequenceLegs) {
    int newLeg = *max_element(
            contractionSequenceLegs.begin(),
            contractionSequenceLegs.end());
    newLeg += 1;
    return newLeg;
}

void Nconpp::addEdge(
        vector<vector<int>> &legsList,
        int src,
        int dest) {
    vector<int> &vertexSrc = legsList[src];
    vector<int> &vertexDest = legsList[dest];

    if (Utils::getIntersection(vertexSrc, vertexDest).empty()) {
        auto legs = Utils::allUniqueSorted(legsList);
        int newLegIndex = 0;
        if (!legs.empty()) {
            auto ind = max_element(legs.begin(), legs.end());
            newLegIndex = *ind + 1;
        }
        vertexSrc.push_back(newLegIndex);
        vertexDest.push_back(newLegIndex);
    }
}
