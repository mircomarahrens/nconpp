//
// Created by mirco on 2/25/2023.
//

#ifndef NCONPP_TENSORNETWORK_H
#define NCONPP_TENSORNETWORK_H

#include "ErrorMessages.h"
#include "Graph.h"
#include "Tensor.h"

#include <algorithm>
#include <complex>


template<typename T>
class TensorNetwork : public Graph {
public:
    /**
     * TODO add comment
     *
     * @param tensorList
     * @param subscriptVectorList
     */
    explicit TensorNetwork(std::vector<nc::tensor<T>> &tensorList,
                           std::vector<std::vector<int>> &subscriptVectorList) :
            Graph() {

        if (tensorList.size() != subscriptVectorList.size()) {
            throw std::invalid_argument(
                    "The number of tensors, which is " +
                    std::to_string(tensorList.size()) +
                    ", does not match the number of legs, which is " +
                    std::to_string(subscriptVectorList.size()) + ".");
        }

        std::size_t dest = 0; std::unordered_map<std::size_t, std::size_t> counts;
        for (auto sv : subscriptVectorList) {
            // create a new vertex
            auto src = addVertex();

            for (int legId: sv) {
                // 0 is an invalid leg index by convention
                if (legId == 0) {
                    throw std::invalid_argument(ERROR_MESSAGES::CONSTRAINT_INVALIDLEG);
                }

                // store negative and positive legIds in sets
                if (legId < 0) {
                    if (mNegLegs.contains(legId)) {
                        throw std::invalid_argument(ERROR_MESSAGES::CONSTRAINT_UNIQUELEGS);
                    }
                    mNegLegs.insert(legId);
                    counts[legId] += 1;
                }

                if (legId > 0) {
                    if (mPosLegs.contains(legId) && counts[legId] < 2) {
                        mLegToEdge[legId] = addEdge(src, dest);
                        counts[legId] += 1;
                    } else if (counts[legId] > 1) {
                        throw std::invalid_argument(ERROR_MESSAGES::CONSTRAINT_LEGPAIRS);
                    } else {
                        src = dest;
                        mPosLegs.insert(legId);
                        counts[legId] += 1;
                    }
                }
            }
            dest++;
        }

        mTensorList = tensorList;
        mSubscriptVectorList = subscriptVectorList;
    };

    ~TensorNetwork() = default;

    /**
     * TODO add comment
     *
     * @param contractionSequence
     * @return
     */
    nc::tensor<T> contract(std::vector<int> contractionSequence = {}, std::vector<int> finalOrder = {}) {

        // fill contraction sequence with positive legs if initially empty
        if(contractionSequence.empty()) {
            contractionSequence.insert(contractionSequence.begin(), mPosLegs.begin(), mPosLegs.end());
        }

        // fill final order with negative legs if initially empty
        if (finalOrder.empty()) {
            finalOrder.insert(finalOrder.end(), mNegLegs.begin(), mNegLegs.end());
        }

        for (int li : contractionSequence) {
            const auto& edge = mLegToEdge[li];
            if(edge.first == edge.second) {
                trace(li, edge);
            } else {
                tensordot(li, edge);
            }
        }

        if (!contractionSequence.empty()) {
            throw std::invalid_argument(
                    "The contraction sequence vector, which size is " +
                    std::to_string(contractionSequence.size()) +
                    ", is not empty.");
        }

        return mTensorList[0];
    };

private:
    std::vector<nc::tensor<T>> mTensorList;
    std::vector<std::vector<int>> mSubscriptVectorList;

    // store negative and positive legIds in separate sets
    std::set<std::size_t> mNegLegs = {};
    std::set<std::size_t> mPosLegs = {};

    // store mapping legId to Edge, only one Edge per legId is possible by convention
    std::unordered_map<std::size_t, std::pair<std::size_t, std::size_t>> mLegToEdge;

    /**
     * TODO add comment
     * inplace manipulation of tensors
     *
     */
    void trace(int legId, std::pair<std::size_t, std::size_t> edge) {
        std::size_t src = edge.first; std::vector<std::size_t> axes;

        auto& legs = mSubscriptVectorList[src];

        for (std::size_t axis = 0; axis < legs.size(); axis++) {
            if (legs[axis] == legId) {
                axes.emplace_back(axis);
            }
        }

        assert(axes.size() == 2);

        auto &tensor = mTensorList[src];
        nc::linalg::trace(tensor, axes[0], axes[1]);

        legs.erase(legs.begin() + axes[0]);
        legs.erase(legs.begin() + axes[1]);
    };

    /**
     * TODO add comment
     * inplace manipulation of tensors
     *
     * @param indexA
     * @param indexB
     * @param axisA
     * @param axisB
     */
    void tensordot(int legId, std::pair<std::size_t, std::size_t> edge) {
        std::size_t indexA = edge.first;
        std::size_t indexB = edge.second;

        auto& legsA = mSubscriptVectorList[indexA];
        auto& legsB = mSubscriptVectorList[indexB];

        std::vector<std::size_t> axesA;
        std::vector<std::size_t> axesB;

        for (auto axis = 0; axis < legsA.size(); axis++) {
            if (legsA[axis] == legId) {
                axesA.emplace_back(axis);
                legsA.erase(legsA.begin() + axis);
            }
        }

        for (auto axis = 0; axis < legsB.size(); axis++) {
            if (legsB[axis] == legId) {
                axesB.emplace_back(axis);
                legsB.erase(legsB.begin() + axis);
            }
        }

        auto &tensorA = mTensorList[indexA];
        auto &tensorB = mTensorList[indexB];

        auto newTensor = nc::linalg::tensordot(tensorA, tensorB, axesA, axesB);

        mTensorList.erase(mTensorList.begin() + indexA);
        mTensorList.erase(mTensorList.begin() + indexB);

        mTensorList.emplace_back(newTensor);

        std::vector<int> newLegs = {};
        newLegs.insert(newLegs.begin(), legsA.begin(), legsA.end());
        newLegs.insert(newLegs.end(), legsB.begin(), legsB.end());

        mSubscriptVectorList.erase(mSubscriptVectorList.begin() + (int)indexA);
        mSubscriptVectorList.erase(mSubscriptVectorList.begin() + (int)indexB + 1);

        mSubscriptVectorList.emplace_back(newLegs);

        // TODO update graph
//        mVertices.erase(mVertices.begin() + axesA[0]);
//        mVertices.erase(mVertices.begin() + axesB[0]);
    };
};

#endif //NCONPP_TENSORNETWORK_H
