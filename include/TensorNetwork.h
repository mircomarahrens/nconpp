//
// Created by mirco on 2/25/2023.
//

#ifndef NCONPP_TENSORNETWORK_H
#define NCONPP_TENSORNETWORK_H

#include "Graph.h"
#include "Tensor.h"

#include <algorithm>
#include <complex>
#include <tuple>
#include <string>

namespace ERROR
{
    const static std::string CONSTRAINT_LEGPAIRS = "Only pairs of legs are allowed.";
    const static std::string CONSTRAINT_INVALIDLEG = "0 is not a valid leg index by convention.";
    const static std::string CONSTRAINT_UNIQUELEGS = "Only unique leg indices are allowed by convention.";
    const static std::string OUT_OF_SIZE = "The position to split is not within the amount of legs.";
}

namespace WARNING
{

}

namespace INFO
{
    const static std::string DISCONNECTED_NETWORKS = "The network is not continuously connected.";
}

template <typename T = std::complex<double>>
class TensorNetwork
{
private:
    // custom graph properties
    struct custom_vertex_properties
    {
        // place custom properties for vertices here
        std::vector<int> legs; // TODO should be a set
        npp::tensor_type<T> tensor;
        bool singular_values = false;
    };

    struct custom_edge_properties
    {
        // place custom properties for edges here
    };

    // TODO maybe inherit? TensorNetwork : public Graph
    // graph object for this class
    Graph<custom_vertex_properties, custom_edge_properties> mGraph;

    // custom typedefs
    typedef Graph<custom_vertex_properties, custom_edge_properties>::vertex_properties_t vertex_properties_t;
    typedef Graph<custom_vertex_properties, custom_edge_properties>::edge_properties_t edge_properties_t;

    // store negative and positive legs in separate sets
    std::set<int> mDanglingLegs = {}; // negative leg indices
    std::set<int> mLegs = {};         // positive leg indices

    /**
     * @brief Perform a trace on a specific vertex between axesA and axesB.
     *
     * @param vertex_index
     * @param axesA
     * @param axesB
     */
    void trace(std::size_t vertex_index, std::size_t axesA, std::size_t axesB)
    {
        auto vertex_properties = mGraph.getVertexProperties(vertex_index);

        auto newTensor = npp::linalg::trace(vertex_properties.tensor, 0, axesA, axesB);

        auto newLegs = vertex_properties.legs;
        newLegs.erase(newLegs.begin() + axesB);
        newLegs.erase(newLegs.begin() + axesA);

        mGraph.setVertexProperties(vertex_index,
                                   vertex_properties_t{std::move(newLegs), std::move(newTensor)});
    };

    /**
     * @brief Perform a tensordot on specific vertices between axesA and axesB.
     *
     * @param src
     * @param dest
     * @param axesA
     * @param axesB
     */
    void tensordot(std::size_t src, std::size_t dest, std::vector<std::size_t> axesA, std::vector<std::size_t> axesB)
    {
        auto legsA = mGraph.getVertexProperties(src).legs;
        auto legsB = mGraph.getVertexProperties(dest).legs;

        legsA.erase(legsA.begin() + axesA[0]);
        legsB.erase(legsB.begin() + axesB[0]);

        std::vector<int> newLegs = {};
        newLegs.insert(newLegs.end(), legsA.begin(), legsA.end());
        newLegs.insert(newLegs.end(), legsB.begin(), legsB.end());

        auto tensorA = mGraph.getVertexProperties(src).tensor;
        auto tensorB = mGraph.getVertexProperties(dest).tensor;

        auto newTensor = npp::linalg::tensordot(tensorA, tensorB, axesA, axesB);

        mGraph.setVertexProperties(src,
                                   vertex_properties_t{std::move(newLegs), std::move(newTensor)});
    };

    /**
     * @brief Perform an outer product between two vertices.
     *
     * @param src
     * @param dest
     */
    void outer(std::size_t src, std::size_t dest)
    {
        auto legsA = mGraph.getVertexProperties(src).legs;
        auto legsB = mGraph.getVertexProperties(dest).legs;

        auto tensorA = mGraph.getVertexProperties(src).tensor;
        auto tensorB = mGraph.getVertexProperties(dest).tensor;

        auto newTensor = npp::linalg::outer(tensorA, tensorB);

        std::vector<int> newLegs = {};
        newLegs.insert(newLegs.end(), legsA.begin(), legsA.end());
        newLegs.insert(newLegs.end(), legsB.begin(), legsB.end());

        mGraph.setVertexProperties(src,
                                   vertex_properties_t{std::move(newLegs), std::move(newTensor)});

        mGraph.removeVertex(dest);
    }

public:
    /**
     * @brief Construct a new Tensor Network object.
     *
     */
    TensorNetwork() = default;

    /**
     * @brief Copy constructor.
     *
     * @param other
     */
    TensorNetwork(const TensorNetwork &other) : mDanglingLegs(other.mDanglingLegs),
                                                mLegs(other.mLegs),
                                                mGraph(other.mGraph)
    {
    }

    /**
     * @brief Move constructor.
     *
     */
    TensorNetwork(TensorNetwork &&other) : mDanglingLegs(other.mDanglingLegs),
                                           mLegs(other.mLegs),
                                           mGraph(other.mGraph)
    {
        other.mDanglingLegs = {};
        other.mLegs = {};
        other.mGraph = {};
    }

    /**
     * Explicit copy constructor with given parameters.
     *
     * @param tensorList
     *  - a list of tensors
     * @param subscriptVectorList
     *  - aka LegsList
     *  - Nomenclature of the legs of the tensor in tensorList:
     *      - the legs are named by integers
     *      - 0 is not a valid leg identifier
     *      - contractible legs have the same positive integer as identifier, hence occurring in pairs
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

        std::size_t vertex_index = 0;
        // counting occurrence of legs to check constraints
        std::unordered_map<std::size_t, std::size_t> _vertex_leg_map;
        for (auto &subscriptVector : subscriptVectorList)
        {
            for (int leg_index : subscriptVector)
            {
                // 0 is an invalid leg index by convention
                if (leg_index == 0)
                {
                    throw std::invalid_argument(ERROR::CONSTRAINT_INVALIDLEG);
                }

                // store negative leg ids as dangling legs
                if (leg_index < 0)
                {
                    if (mDanglingLegs.contains(leg_index))
                    {
                        throw std::invalid_argument(ERROR::CONSTRAINT_UNIQUELEGS);
                    }
                    mDanglingLegs.insert(leg_index);
                }

                // store positive legs as edge legs
                if (leg_index > 0)
                {
                    // if leg_id has already been seen -> form an edge
                    if (mLegs.contains(leg_index))
                    {
                        // obtain previous src from map
                        std::size_t prev = _vertex_leg_map[leg_index];

                        // add edge between previous src and dest (the current src)
                        mGraph.addEdge(prev, vertex_index, leg_index);

                        // erase entry from map
                        _vertex_leg_map.erase(leg_index);
                    }
                    else
                    {
                        // store mapping leg to src
                        _vertex_leg_map[leg_index] = vertex_index;

                        // store edge leg
                        mLegs.insert(leg_index);
                    }
                }
            }

            mGraph.setVertexProperties(vertex_index,
                                       vertex_properties_t{subscriptVectorList[vertex_index], tensorList[vertex_index]});

            vertex_index++;
        }
    };

    /**
     * Explicit move constructor with given parameters.
     *
     * @param tensorList
     *  - a list of tensors
     * @param subscriptVectorList
     *  - aka LegsList
     *  - Nomenclature of the legs of the tensor in tensorList:
     *      - the legs are named by integers
     *      - 0 is not a valid leg identifier
     *      - contractible legs have the same positive integer as identifier, hence occurring in pairs
     *      - legs with negative integers won't be contracted, so-called dangling legs
     */
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
            for (int leg_index : subscriptVector)
            {
                // 0 is an invalid leg index by convention
                if (leg_index == 0)
                {
                    throw std::invalid_argument(ERROR::CONSTRAINT_INVALIDLEG);
                }

                // store negative leg ids as dangling legs
                if (leg_index < 0)
                {
                    if (mDanglingLegs.contains(leg_index))
                    {
                        throw std::invalid_argument(ERROR::CONSTRAINT_UNIQUELEGS);
                    }
                    mDanglingLegs.insert(leg_index);
                }

                // store positive legs as edge legs
                if (leg_index > 0)
                {
                    // if leg_id has already been seen -> form an edge
                    if (mLegs.contains(leg_index))
                    {
                        // obtain previous src from map
                        std::size_t prev = _vertex_leg_map[leg_index];

                        // add edge between previous src and dest (the current src)
                        mGraph.addEdge(prev, vertex_index, leg_index);

                        // erase entry from map
                        _vertex_leg_map.erase(leg_index);
                    }
                    else
                    {
                        // store mapping leg to src
                        _vertex_leg_map[leg_index] = vertex_index;

                        // store edge leg
                        mLegs.insert(leg_index);
                    }
                }
            }

            mGraph.setVertexProperties(vertex_index,
                                       vertex_properties_t{std::move(subscriptVectorList[vertex_index]), std::move(tensorList[vertex_index])});

            vertex_index++;
        }
    };

    /**
     * @brief Destroy the Tensor Network object
     *
     */
    ~TensorNetwork() = default;

    /**
     * @brief Retrieve current dangling legs (negative indices).
     *
     * @return const std::set<int>&
     */
    const std::set<int> &DanglingLegs()
    {
        return mDanglingLegs;
    }

    /**
     * @brief Retrieve current legs (positive indices).
     *
     */
    const std::set<int> &Legs()
    {
        return mLegs;
    }

    /**
     *
     * @param contractionSequence
     *  - order by legs in which the tensors shall be contracted
     * @param finalOrder
     *  - permutation of the legs of the final tensors
     */
    void contract(std::vector<int> contractionSequence = {}, std::vector<int> finalOrder = {})
    {
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

            int leg_index = *contractionSequence.begin();
            auto edge = mGraph.getEdge(leg_index);

            auto src = edge.first;
            auto dest = edge.second;

            if (src == dest)
            { // trace

                std::vector<std::size_t> axes = {};
                for (auto axis = 0; axis < mGraph.getVertexProperties(dest).legs.size(); axis++)
                {
                    if (mGraph.getVertexProperties(dest).legs[axis] == leg_index)
                    {
                        axes.emplace_back(axis);
                    }
                }

                trace(src, axes[0], axes[1]);

                mGraph.removeEdge(leg_index);
            }
            else
            { // tensordot

                // TODO add multi axis tensor contraction?
                std::vector<std::size_t> axesA = {};
                std::vector<std::size_t> axesB = {};

                for (auto axis = 0; axis < mGraph.getVertexProperties(src).legs.size(); axis++)
                {
                    if (mGraph.getVertexProperties(src).legs[axis] == leg_index)
                    {
                        axesA.emplace_back(axis);
                    }
                }
                for (auto axis = 0; axis < mGraph.getVertexProperties(dest).legs.size(); axis++)
                {
                    if (mGraph.getVertexProperties(dest).legs[axis] == leg_index)
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
     * @brief Connect all tensors into a single one by outer products.
     *
     */
    void connect()
    {
        auto nv = mGraph.NumVertices();
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
    std::size_t NumTensors()
    {
        return mGraph.NumVertices();
    }

    /**
     * @brief Retrieve a current view of tensors.
     *
     * @return const std::vector<npp::tensor_type<T>>&
     */
    std::vector<npp::tensor_type<T>> TensorList()
    {
        auto nv = mGraph.NumVertices();
        std::vector<npp::tensor_type<T>> result(nv);
        for (int i = 0; i < nv; i++)
        {
            result[i] = mGraph.getVertexProperties(i).tensor;
        }
        return result;
    }

    /**
     * @brief Retrieve a current view of the shapes of the tensors.
     *
     * @return std::vector<npp::shape_type>
     */
    std::vector<npp::shape_type> TensorShapes()
    {
        auto nv = mGraph.NumVertices();
        std::vector<npp::shape_type> result(nv);
        for (int i = 0; i < nv; i++)
        {
            result[i] = mGraph.getVertexProperties(i).tensor.shape();
        }
        return result;
    }

    /**
     * @brief Split a vertex (default 0) on a specific leg position.
     *
     * @param leg_pos
     * @param vertex_pos
     */
    void split(std::size_t leg_pos, std::size_t vertex_pos = 0)
    {
        auto vertex = mGraph[vertex_pos];
        auto legs = vertex.legs;
        auto tensor = vertex.tensor;

        // split legs
        std::vector<int>::const_iterator leg_pos_it(legs.cbegin());
        std::advance(leg_pos_it, leg_pos);

        std::vector<int> left_legs(legs.begin(), leg_pos_it);
        std::vector<int> right_legs(leg_pos_it, legs.end());

        // split tensor
        std::size_t len = legs.size();
        if (leg_pos < len)
        {
            auto shape = npp::shape(tensor);

            std::size_t left = 1, right = 1;
            npp::shape_type left_shape, right_shape;
            for (std::size_t s = 0; s < len; s++)
            {
                if (s < leg_pos)
                {
                    left *= shape[s];

                    left_shape.push_back(shape[s]);
                }
                else if (s == leg_pos)
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

            npp::tensor_type<T> U, s, V;
            std::tie(U, s, V) = npp::linalg::svd(tensor);

            // reshape U,V back to tensors with new shape
            npp::reshape(U, left_shape);
            npp::reshape(V, right_shape);

            // new leg ids
            int new_leg_left = *mLegs.rbegin() + 1;
            int new_leg_right = new_leg_left + 1;

            // add to the subscript vectors
            left_legs.emplace_back(new_leg_left);
            std::vector<int> s_legs{new_leg_left, new_leg_right};
            right_legs.emplace_back(new_leg_right);

            // add to list of current legs
            mLegs.insert(new_leg_left);
            mLegs.insert(new_leg_right);

            // update current vertex for U and create new vertices for s and V
            auto U_ver = mGraph.setVertexProperties(vertex_properties_t{std::move(U), std::move(left_legs)}, vertex_pos);
            auto s_ver = mGraph.addVertex(vertex_properties_t{std::move(s), std::move(s_legs)});
            auto V_ver = mGraph.addVertex(vertex_properties_t{std::move(U), std::move(right_legs)});

            // add edges to graph
            mGraph.addEdge(U_ver, s_ver, new_leg_left);
            mGraph.addEdge(s_ver, V_ver, new_leg_right);

            // only edges of rhs needs to be updated
            for (int i : right_legs)
            {
                if (i > 0)
                {
                    // pair(src, tar)
                    auto edge = mGraph.getEdge(i);

                    if (edge.first != V_ver)
                    {
                        mGraph.updateEdge(i, V_ver, edge.second);
                    }
                }
            }

            throw std::logic_error("Not finally implemented yet. Current TODO: update edges.");
        }
        else
        {
            throw std::invalid_argument(ERROR::OUT_OF_SIZE);
        }
    }
};

#endif // NCONPP_TENSORNETWORK_H
