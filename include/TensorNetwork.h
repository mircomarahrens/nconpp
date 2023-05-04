//
// Created by mirco on 2/25/2023.
//

#ifndef NCONPP_TENSORNETWORK_H
#define NCONPP_TENSORNETWORK_H

#include "LogMessages.h"
#include "Graph.h"
#include "Tensor.h"

#include <algorithm>
#include <complex>
#include <tuple>

template <typename T>
class TensorNetwork
{
private:
    struct custom_vertex_properties
    {
        std::vector<int> legs;
        npp::tensor_type<T> tensor;
    };

    struct custom_edge_properties
    {
    };

    Graph<custom_vertex_properties, custom_edge_properties> mGraph;

    typedef Graph<custom_vertex_properties, custom_edge_properties>::vertex_properties_t vertex_properties;
    typedef Graph<custom_vertex_properties, custom_edge_properties>::edge_properties_t edge_properties;

    // store negative and positive legs in separate sets
    std::set<int> mDanglingLegs = {};
    std::set<int> mLegs = {};

    /**
     * TODO add comment
     *
     * @param leg_id
     *
     */
    void trace(std::size_t vertex, std::size_t axesA, std::size_t axesB)
    {
        auto newTensor = npp::linalg::trace(mGraph[vertex].tensor, 0, axesA, axesB);

        auto newLegs = mGraph[vertex].legs;
        newLegs.erase(newLegs.begin() + axesB);
        newLegs.erase(newLegs.begin() + axesA);

        mGraph.setVertexProperties(vertex, 
            vertex_properties{std::move(newLegs), std::move(newTensor)});
    };

    /**
     * TODO add comment
     *
     * @param leg_id
     */
    void tensordot(std::size_t src, std::size_t dest, std::vector<std::size_t> axesA, std::vector<std::size_t> axesB)
    {
        auto legsA = mGraph[src].legs;
        auto legsB = mGraph[dest].legs;

        legsA.erase(legsA.begin() + axesA[0]);
        legsB.erase(legsB.begin() + axesB[0]);

        std::vector<int> newLegs = {};
        newLegs.insert(newLegs.end(), legsA.begin(), legsA.end());
        newLegs.insert(newLegs.end(), legsB.begin(), legsB.end());

        auto tensorA = mGraph[src].tensor;
        auto tensorB = mGraph[dest].tensor;

        auto newTensor = npp::linalg::tensordot(tensorA, tensorB, axesA, axesB);

        mGraph.setVertexProperties(src, 
            vertex_properties{std::move(newLegs), std::move(newTensor)});
    };

    /**
     * Perform an outer product of two vertices.
     * @param src
     * @param dest
     */
    void outer(std::size_t src, std::size_t dest)
    {
        auto legsA = mGraph[src].legs;
        auto legsB = mGraph[dest].legs;

        auto tensorA = mGraph[src].tensor;
        auto tensorB = mGraph[dest].tensor;

        auto newTensor = npp::linalg::outer(tensorA, tensorB);

        std::vector<int> newLegs = {};
        newLegs.insert(newLegs.end(), legsA.begin(), legsA.end());
        newLegs.insert(newLegs.end(), legsB.begin(), legsB.end());

        mGraph[src].tensor = std::move(newTensor);
        mGraph[src].legs = std::move(newLegs);

        // clear and remove dest vertex
        mGraph.removeVertex(dest);
    }

public:
    /**
     * Explicit copy constructor with given parameters.
     *
     * @param tensorList
     *  - a list of tensors
     * @param subscriptVectorList
     *  - aka LegsList
     *  - Nomenclature of the legs of the tensor in tensorList:
     *      - the legs are named by integers
     *      - contractible legs have the same positive integer as name, hence occurring in pairs
     *      - legs with negative integers won't be contracted, so-called dangling legs
     */
    explicit TensorNetwork(const std::vector<npp::tensor_type<T>> &tensorList,
                           const std::vector<std::vector<int>> &subscriptVectorList) : mGraph(tensorList.size())
    {
        if (tensorList.size() != subscriptVectorList.size())
        {
            throw std::invalid_argument(
                "The number of tensors, which is " +
                std::to_string(tensorList.size()) +
                ", does not match the number of legs, which is " +
                std::to_string(subscriptVectorList.size()) + ".");
        }

        std::size_t index = 0;
        // counting occurrence of legs to check constraints
        std::unordered_map<std::size_t, std::size_t> _vertex_leg_map;
        for (auto &subscriptVector : subscriptVectorList)
        {
            for (int leg : subscriptVector)
            {
                // 0 is an invalid leg index by convention
                if (leg == 0)
                {
                    throw std::invalid_argument(ERROR::CONSTRAINT_INVALIDLEG);
                }

                // store negative leg ids as dangling legs
                if (leg < 0)
                {
                    if (mDanglingLegs.contains(leg))
                    {
                        throw std::invalid_argument(ERROR::CONSTRAINT_UNIQUELEGS);
                    }
                    mDanglingLegs.insert(leg);
                }

                // store positive legs as edge legs
                if (leg > 0)
                {
                    // if leg_id has already been seen -> form an edge
                    if (mLegs.contains(leg))
                    {
                        // obtain previous src from map
                        std::size_t prev = _vertex_leg_map[leg];

                        // add edge between previous src and dest (the current src)
                        mGraph.addEdge(prev, index, leg);

                        // erase entry from map
                        _vertex_leg_map.erase(leg);
                    }
                    else
                    {
                        // store mapping leg to src
                        _vertex_leg_map[leg] = index;

                        // store edge leg
                        mLegs.insert(leg);
                    }
                }
            }

            mGraph.setVertexProperties(index, 
                vertex_properties{subscriptVector, tensorList[index]});

            index++;
        }
    };

    explicit TensorNetwork(std::vector<npp::tensor_type<T>> &&tensorList,
                           std::vector<std::vector<int>> &&subscriptVectorList) : mGraph(tensorList.size())
    {
        if (tensorList.size() != subscriptVectorList.size())
        {
            throw std::invalid_argument(
                "The number of tensors, which is " +
                std::to_string(tensorList.size()) +
                ", does not match the number of legs, which is " +
                std::to_string(subscriptVectorList.size()) + ".");
        }

        std::size_t vertex_index = 0;
        // counting occurrence of legs to check constraints
        std::unordered_map<std::size_t, std::size_t> _vertex_leg_map;
        for (auto &subscriptVector : subscriptVectorList)
        {
            for (int leg : subscriptVector)
            {
                // 0 is an invalid leg index by convention
                if (leg == 0)
                {
                    throw std::invalid_argument(ERROR::CONSTRAINT_INVALIDLEG);
                }

                // store negative leg ids as dangling legs
                if (leg < 0)
                {
                    if (mDanglingLegs.contains(leg))
                    {
                        throw std::invalid_argument(ERROR::CONSTRAINT_UNIQUELEGS);
                    }
                    mDanglingLegs.insert(leg);
                }

                // store positive legs as edge legs
                if (leg > 0)
                {
                    // if leg_id has already been seen -> form an edge
                    if (mLegs.contains(leg))
                    {
                        // obtain previous src from map
                        std::size_t prev = _vertex_leg_map[leg];

                        // add edge between previous src and dest (the current src)
                        mGraph.addEdge(prev, vertex_index, leg);

                        // erase entry from map
                        _vertex_leg_map.erase(leg);
                    }
                    else
                    {
                        // store mapping leg to src
                        _vertex_leg_map[leg] = vertex_index;

                        // store edge leg
                        mLegs.insert(leg);
                    }
                }
            }

            mGraph.setVertexProperties(vertex_index, 
               vertex_properties{std::move(subscriptVector), std::move(tensorList[vertex_index])});

            vertex_index++;
        }
    };

    ~TensorNetwork() = default;

    /**
     *
     * @param contractionSequence
     *  - order by legs in which the tensors shall be contracted
     * @param finalOrder
     *  - permutation of the legs of the final tensors
     */
    void contract(std::vector<int> contractionSequence = {}, std::vector<int> finalOrder = {})
    {
        // TODO optimization:
        //  - flatten edges

        // fill contraction sequence with positive legs if initially empty
        if (contractionSequence.empty())
        {
            contractionSequence.insert(contractionSequence.begin(), mLegs.begin(), mLegs.end());
        }

        // fill final order with negative legs if initially empty
        if (finalOrder.empty())
        {
            finalOrder.insert(finalOrder.end(), mDanglingLegs.begin(), mDanglingLegs.end());
        }

        while (!contractionSequence.empty())
        {

            int legId = *contractionSequence.begin();
            auto edge = mGraph.getEdge(legId);

            auto src = edge.first;
            auto dest = edge.second;

            if (src == dest)
            { // trace

                std::vector<std::size_t> axes = {};
                for (auto axis = 0; axis < mGraph[dest].legs.size(); axis++)
                {
                    if (mGraph[dest].legs[axis] == legId)
                    {
                        axes.emplace_back(axis);
                    }
                }

                trace(src, axes[0], axes[1]);
                
                mGraph.removeEdge(legId);
            }
            else
            { // tensordot

                // TODO add multi axis tensor contraction?
                std::vector<std::size_t> axesA = {};
                std::vector<std::size_t> axesB = {};

                for (auto axis = 0; axis < mGraph[src].legs.size(); axis++)
                {
                    if (mGraph[src].legs[axis] == legId)
                    {
                        axesA.emplace_back(axis);
                    }
                }
                for (auto axis = 0; axis < mGraph[dest].legs.size(); axis++)
                {
                    if (mGraph[dest].legs[axis] == legId)
                    {
                        axesB.emplace_back(axis);
                    }
                }

                tensordot(src, dest, axesA, axesB);
                
                mGraph.mergeVertices(src, dest);
            }
            contractionSequence.erase(contractionSequence.begin());
        }

        if (!contractionSequence.empty())
        {
            throw std::invalid_argument(
                "The contraction sequence vector, which size is " +
                std::to_string(contractionSequence.size()) +
                ", is not empty.");
        }
    }

    /**
     * @brief Connect all remaining tensors into a single one by outer products.
     * 
     */
    void connect()
    {
        auto nv = mGraph.numVertices();
        while (nv > 1)
        {
            outer(0, 1);
            nv--;
        }
    }

    /**
     *
     * @return
     */
    std::size_t numTensors()
    {
        return mGraph.numVertices();
    }

    /**
     *
     * @return
     */
    std::vector<npp::tensor_type<T>> getTensorList()
    {
        std::vector<npp::tensor_type<T>> result = {};
        auto nv = mGraph.numVertices();
        for (int i = 0; i < nv; i++)
        {
            result.emplace_back(mGraph[i].tensor);
        }
        return result;
    }

    void split(std::size_t pos, std::size_t vertex_pos = 0)
    {
        auto vertex = mGraph[vertex_pos];
        auto tensor = vertex.tensor;
        auto legs = vertex.legs;

        std::size_t len = legs.size();
        if (pos < len)
        {

            auto shape = npp::shape(tensor);

            std::size_t left = 1, right = 1;
            npp::shape_type left_shape, right_shape;
            for (std::size_t s = 0; s < len; s++)
            {
                if (s < pos)
                {
                    left *= shape[s];

                    left_shape.push_back(shape[s]);
                }
                else if (s == pos)
                {
                    right *= shape[s];

                    // new shape after svd
                    left_shape.push_back(left);
                    right_shape.push_back(left);

                    right_shape.push_back(shape[s]);
                }
                else
                {
                    right *= shape[s];

                    right_shape.push_back(shape[s]);
                }
            }

            npp::reshape(tensor, npp::shape_type(left, right));

            auto res = npp::linalg::svd(tensor);

            npp::tensor_type<T> U, s, V;
            std::tie(U, s, V) = res;

            npp::reshape(U, left_shape);
            npp::reshape(V, right_shape);

            // TODO
            // - add new vertex
            // - split legs
            // - add new leg
            throw std::logic_error("Not implemented yet.");
        }
        else
        {
            throw std::invalid_argument(ERROR::OUT_OF_SIZE);
        }
    }
};

#endif // NCONPP_TENSORNETWORK_H
