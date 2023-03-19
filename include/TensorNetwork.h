//
// Created by mirco on 2/25/2023.
//

#ifndef NCONPP_TENSORNETWORK_H
#define NCONPP_TENSORNETWORK_H

#include "Error.h"
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
    explicit TensorNetwork(std::vector<npp::array_type<T>> &tensorList,
                           std::vector<std::vector<int>> &subscriptVectorList) :
            Graph(tensorList.size()), mTensorList(tensorList) {

        if (tensorList.size() != subscriptVectorList.size()) {
            throw std::invalid_argument(
                    "The number of tensors, which is " +
                    std::to_string(tensorList.size()) +
                    ", does not match the number of legs, which is " +
                    std::to_string(subscriptVectorList.size()) + ".");
        }

        std::size_t src_id = 0; std::size_t dest_id = 0;
        // counting occurrence of legs to check constraints
        std::unordered_map<std::size_t, std::size_t> _vertex_leg_map;
        for (const auto& led_ids: subscriptVectorList) {
            // set stable vertex properties
            graph[src_id].id = src_id;
            graph[src_id].leg_ids = led_ids;

            for (int leg_id: led_ids) {
                // 0 is an invalid leg index by convention
                if (leg_id == 0) {
                    throw std::invalid_argument(ERROR::CONSTRAINT_INVALIDLEG);
                }

                // store negative leg ids as dangling legs
                if (leg_id < 0) {
                    if (mDanglingLegs.contains(leg_id)) {
                        throw std::invalid_argument(ERROR::CONSTRAINT_UNIQUELEGS);
                    }
                    mDanglingLegs.insert(leg_id);
                }

                // store positive legs as edge legs
                if (leg_id > 0) {
                    // if leg_id has already been seen -> form an edge
                    if (mEdgeLegs.contains(leg_id)) {
                        // obtain previous src_id from map
                        auto prev_id = _vertex_leg_map[leg_id];

                        // add edge between previous src_id and dest_id (the current src_id)
                        auto edge = addEdge(prev_id, src_id);

                        // set stable edge properties
                        graph[edge].leg_id = leg_id;

                        // store to leg-to-edge map
                        mLegToEdge[leg_id] = edge;

                        // erase entry from map
                        _vertex_leg_map.erase(leg_id);
                    } else {
                        // store mapping leg_id to src_id
                        _vertex_leg_map[leg_id] = src_id;

                        // store edge leg
                        mEdgeLegs.insert(leg_id);
                    }
                }
            }
            src_id++;
        }

        // if the map is not empty we have an error by our restrictions
        if (!_vertex_leg_map.empty()) {
            throw std::invalid_argument(ERROR::CONSTRAINT_LEGPAIRS);
        }

        mConnectedComponents = getConnectedComponents();
    };

    ~TensorNetwork() = default;

    /**
     * Trivially connect disconnected components by taking the shortest elements of the sub vector index lists in the
     * components and expand them by a new index of dimension one.
     */
    void connectDisconnectedComponents() {
        std::size_t _num_components = mConnectedComponents.size();
        if (_num_components > 1) {
            for (std::size_t c = 0; c != _num_components; c++) {
                // TODO
                // odd or even number of components
                // get new legs aka extend mEdgeLegs
                // extend "shortest" tensors
                // extend vertices corresponding to tensor
                // add new leg to mEdgeLegs
            }
        }
    }

    /**
     * TODO add comment
     *
     * @param contractionSequence
     * @return
     */
    npp::array_type<T> contract(std::vector<int> contractionSequence = {}, std::vector<int> finalOrder = {}) {

        // throw error if the tensor network consists of multiple disconnected components
        if (mConnectedComponents.size() != 1) {
            throw std::logic_error(ERROR::DISCONNECTED_NETWORKS);
        }

        // fill contraction sequence with positive legs if initially empty
        if (contractionSequence.empty()) {
            contractionSequence.insert(contractionSequence.begin(), mEdgeLegs.begin(), mEdgeLegs.end());
        }

        // fill final order with negative legs if initially empty
        if (finalOrder.empty()) {
            finalOrder.insert(finalOrder.end(), mDanglingLegs.begin(), mDanglingLegs.end());
        }

        for (int leg_id: contractionSequence) {
            auto edge = mLegToEdge[leg_id];
            if (graph[edge.m_source].id == graph[edge.m_target].id) {
                trace(leg_id);
            } else {
                tensordot(leg_id);
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
    std::vector<npp::array_type<T>> mTensorList;

    // store negative and positive legIds in separate sets
    std::set<int> mDanglingLegs = {};
    std::set<int> mEdgeLegs = {};

    // store mapping legId to edge
    std::unordered_map<std::size_t, graph_t::edge_descriptor> mLegToEdge = {};

    std::vector<std::vector<std::size_t>> mConnectedComponents = {};

    /**
     * TODO add comment
     * inplace manipulation of tensors
     *
     */
    void trace(int legId) {
        auto src = graph[mLegToEdge[legId].m_source];

        auto &legs = src.leg_ids;

        std::vector<std::size_t> axes(2);
        int c = 0;
        for (std::size_t axis = 0; axis < legs.size(); axis++) {
            if (legs[axis] == legId) {
                axes[c] = axis; c++;
            }
        }
        assert(axes.size() == 2);

        npp::linalg::trace(mTensorList[src.id], 0, axes[0], axes[1]);

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
    void tensordot(int legId) {
        auto src = graph[mLegToEdge[legId].m_source];
        auto dest = graph[mLegToEdge[legId].m_target];

        std::size_t indexA = src.id;
        std::size_t indexB = dest.id;

        auto &legsA = src.leg_ids;
        auto &legsB = dest.leg_ids;

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

        auto newTensor = npp::linalg::tensordot(tensorA, tensorB, axesA, axesB);

        mTensorList.erase(mTensorList.begin() + indexA);
        mTensorList.erase(mTensorList.begin() + indexB);

        mTensorList.emplace_back(newTensor);

        std::vector<int> newLegs = {};
        newLegs.insert(newLegs.begin(), legsA.begin(), legsA.end());
        newLegs.insert(newLegs.end(), legsB.begin(), legsB.end());

//        mSubscriptVectorList.erase(mSubscriptVectorList.begin() + (int) indexA);
//        mSubscriptVectorList.erase(mSubscriptVectorList.begin() + (int) indexB + 1);
//
//        mSubscriptVectorList.emplace_back(newLegs);

        // TODO update graph
//        mVertices.erase(mVertices.begin() + axesA[0]);
//        mVertices.erase(mVertices.begin() + axesB[0]);
    };
};

#endif //NCONPP_TENSORNETWORK_H
