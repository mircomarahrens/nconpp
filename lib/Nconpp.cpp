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

    // TODO connect disconnected components

    TensorNetwork tensorNetwork{tensorList, subscriptVectorList};

    while (!contractionSequence.empty()) {
        auto it = contractionSequence.begin();

        for (int i = 0; i < subscriptVectorList.size(); i++) {
            std::vector<int> ilegs = subscriptVectorList[i];
            std::vector<size_t> results;
            auto pos = std::find_if(std::begin(ilegs), std::end(ilegs), [it](int i){return i == *it;});
            while (pos != std::end(ilegs)) {
                results.emplace_back(std::distance(std::begin(ilegs), pos));
                pos = std::find_if(std::next(pos), std::end(ilegs), [](int i){return i > 5;});
            }
            if (results.size() != 1) {
                // doTrace
                tensorNetwork.doTrace(indexA, axisA, axisB);
            } else {
                // doTensorProduct
                for (int j = i; j < subscriptVectorList.size(); j++) {
                    auto& jlegs = subscriptVectorList[j];

                    tensorNetwork.doTensorProduct(indexA, indexB, axisA, axisB);
                }
            }
        }

        contractionSequence.erase(contractionSequence.begin());
    }

    return tensorNetwork.getTensor();
}
