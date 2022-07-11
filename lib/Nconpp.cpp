#include "Nconpp.h"

#include "ErrorMessages.h"
#include "Utils.h"
#include "TensorNetwork.h"

#include <set>
#include <stdexcept>
#include <vector>

using namespace std;

template<class T>
T Nconpp::contract(
        vector<T> &tensorList,
        vector<vector<int>> subscriptVectorList,
        std::vector<int> contractionSequence,
        std::vector<int> finalOrder) {

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

    TensorNetwork tensorNetwork{tensorList, subscriptVectorList};

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
            tensorNetwork.doTrace(indexA, axisA, axisB);
        } else // do tensor product
        {
            tensorNetwork.doTensorProduct(indexA, indexB, axisA, axisB);
        }
        contractionSequence.erase(contractionSequence.begin());
    }

    return std::move(tensorList[0]);
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
