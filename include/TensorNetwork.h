//
// Created by mirco on 2/25/2023.
//

#ifndef NCONPP_TENSORNETWORK_H
#define NCONPP_TENSORNETWORK_H

#include "LogMessages.h"
#include "Tensor.h"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/properties.hpp>
#include <boost/property_map/property_map.hpp>

#include <algorithm>
#include <complex>

template<typename T>
class TensorNetwork {
private:
    struct vertex_properties {
        std::vector<int> legs;
        npp::tensor<T> tensor;
    };

    struct edge_properties {
        int leg;
    };

    typedef typename boost::adjacency_list
            <boost::setS, boost::setS, boost::undirectedS, vertex_properties, edge_properties> graph_t;

    graph_t mGraph;

    // store negative and positive legs in separate sets
    std::set<int> mDanglingLegs = {};
    std::set<int> mLegs = {};

    // store mapping leg to edge
    std::unordered_map<std::size_t, typename graph_t::edge_descriptor> mLegToEdge = {};

    // store connected components
    std::vector<std::vector<std::size_t>> mConnectedComponents = {};

    /**
     * Add a new vertex to the graph.
     */
    std::size_t addVertex() {
        auto v = boost::add_vertex(mGraph);
        return v;
    }

    /**
     * Remove a vertex from the graph.  All edges corresponding to the vertex are removed beforehand.
     * @param index
     */
    void removeVertex(boost::graph_traits<graph_t>::vertex_descriptor v) {
        boost::clear_vertex(v, mGraph); // ensure all edges to vertex are removed beforehand
        boost::remove_vertex(v, mGraph);
    }

    /**
     * Remove all vertices from the graph. All edges corresponding to the vertex are removed beforehand.
     */
    void removeAllVertices() {
        typename boost::graph_traits<graph_t>::vertex_iterator vi, vi_end, next;
        boost::tie(vi, vi_end) = boost::vertices(mGraph);
        for (next = vi; vi != vi_end; vi = next) {
            ++next;
            boost::clear_vertex(*vi, mGraph); // ensure all edges to vertex are removed beforehand
            boost::remove_vertex(*vi, mGraph);
        }
    }

    /**
     * Retrieve all current vertex indices.
     * @return
     */
    std::vector<std::size_t> getVertices() {
        std::vector<std::size_t> result = {};

        // get the property map for vertex id
        typedef typename boost::property_map<graph_t, boost::vertex_index_t>::type InternalVertexIdMap;
        InternalVertexIdMap vertex_indices = boost::get(boost::vertex_index, mGraph);

        // tie vertex iterators
        typename boost::graph_traits<graph_t>::vertex_iterator vi, vi_end, next;
        boost::tie(vi, vi_end) = boost::vertices(mGraph);

        for (next = vi; vi != vi_end; vi = next) {
            ++next;
            result.emplace_back(vertex_indices[*vi]);
        }

        return std::move(result);
    }

    /**
     * Add a new edge to the graph.
     * @param src
     * @param dest
     */
    auto addEdge(std::size_t src, std::size_t dest) {
        auto s = boost::vertex(src, mGraph);
        auto d = boost::vertex(dest, mGraph);
        auto e = boost::add_edge(s, d, mGraph);
        assert(e.second == true);
        return e.first;
    }

    /**
     * Remove an edge between source and destination vertices.
     * @param src
     * @param dest
     */
    void removeEdge(std::size_t src, std::size_t dest) {
        auto s = boost::vertex(src, mGraph);
        auto d = boost::vertex(dest, mGraph);
        boost::remove_edge(s, d, mGraph);
    }

    /**
     * Retrieve all edges as pair of vertex indices.
     * @return
     */
    std::vector<std::pair<std::size_t, std::size_t>> getEdges() {
        // get the property map for vertex id
        typedef typename boost::property_map<graph_t, boost::vertex_index_t>::type VertexIdMap;
        VertexIdMap vertex_id = boost::get(boost::vertex_index, mGraph);

        // tie vertex iterators
        typename boost::graph_traits<graph_t>::vertex_iterator vi, vi_end, next;
        boost::tie(vi, vi_end) = boost::vertices(mGraph);

        std::vector<std::pair<std::size_t, std::size_t>> result(boost::num_vertices(mGraph));

        // iterate through vertices
        for (next = vi; vi != vi_end; vi = next) {
            ++next;

            // iterate through adjacent vertices
            typename boost::graph_traits<graph_t>::adjacency_iterator ai, ai_end;
            for (boost::tie(ai, ai_end) = boost::adjacent_vertices(*vi, mGraph); ai != ai_end; ++ai) {
                result.emplace_back(vertex_id[*vi], vertex_id[*ai]);
            }
        }

        return std::move(result);
    }

    /**
     * Retrieve current connected components of the (undirected) graph.
     * @return
     */
    std::vector<std::vector<std::size_t>>
    getConnectedComponents() {
        // Assume graph is a undirected graph object of type boost::adjacency_list
        std::vector<std::size_t> component(boost::num_vertices(mGraph));
        std::size_t num_components = boost::connected_components(mGraph, &component[0]);
        std::vector<std::vector<std::size_t>> result(num_components);

        for (std::size_t i = 0; i < component.size(); ++i) { // Loop over the component array
            result[component[i]].push_back(i); // Push the vertex index into the corresponding vector
        }

        return result;
    }

    /**
     *
     * @param u
     * @return
     */
    std::vector<typename graph_t::vertex_descriptor> getNeighbours(typename graph_t::vertex_descriptor u) {
        std::vector<typename graph_t::vertex_descriptor> result = {};

        // iterate through adjacent vertices
        typename boost::graph_traits<graph_t>::adjacency_iterator vi, vi_end;
        for (boost::tie(vi, vi_end) = boost::adjacent_vertices(u, mGraph); vi != vi_end; ++vi) {
            result.emplace_back(vi);
        }

        return result;
    }

    /**
     * TODO add comment
     * inplace manipulation of tensors
     *
     * @param leg_id
     *
     */
    void trace(int leg_id) {
        auto src = mGraph[mLegToEdge[leg_id].m_source];

        auto &legs = src.legs;

        std::vector<std::size_t> axes(2);
        int i = 0;
        for (std::size_t axis = 0; axis < legs.size(); axis++) {
            if (legs[axis] == leg_id) {
                axes[i] = axis;
                i++;
            }
        }

        assert(axes.size() == 2);

        npp::linalg::trace(src.tensor, 0, axes[0], axes[1]);

        legs.erase(legs.begin() + axes[0]);
        legs.erase(legs.begin() + axes[1]);
    };

    /**
     * TODO add comment
     * inplace manipulation of tensors
     *
     * @param leg_id
     */
    void tensordot(int leg_id) {
        auto src = mGraph[mLegToEdge[leg_id].m_source];
        auto dest = mGraph[mLegToEdge[leg_id].m_target];

        auto &legsA = src.legs;
        auto &legsB = dest.legs;

        auto &tensorA = src.tensor;
        auto &tensorB = dest.tensor;

        std::vector<std::size_t> axesA;
        std::vector<std::size_t> axesB;

        for (auto axis = 0; axis < legsA.size(); axis++) {
            if (legsA[axis] == leg_id) {
                axesA.emplace_back(axis);
                legsA.erase(legsA.begin() + axis);
            }
        }
        for (auto axis = 0; axis < legsB.size(); axis++) {
            if (legsB[axis] == leg_id) {
                axesB.emplace_back(axis);
                legsB.erase(legsB.begin() + axis);
            }
        }

        auto newTensor = npp::linalg::tensordot(tensorA, tensorB, axesA, axesB);

        legsA.erase(legsA.begin() + axesA[0]);
        legsB.erase(legsB.begin() + axesB[0]);

        std::vector<int> newLegs = {};
        newLegs.insert(newLegs.end(), legsA.begin(), legsA.end());
        newLegs.insert(newLegs.end(), legsB.begin(), legsB.end());

        auto nv = addVertex();
        mGraph[nv].tensor = std::move(newTensor);
        mGraph[nv].legs = std::move(newLegs);

        auto nsrc = getNeighbours(src);
        auto ndest = getNeighbours(dest);

        // TODO update edges

        removeVertex(mLegToEdge[leg_id].m_source);
        removeVertex(mLegToEdge[leg_id].m_target);
    };

public:
    /**
     * Explicit constructor with given parameters.
     *
     * @param tensorList
     * @param subscriptVectorList
     */
    explicit TensorNetwork(std::vector<npp::array_type<T>> &tensorList,
                           std::vector<std::vector<int>> &subscriptVectorList) :
            mGraph(tensorList.size()) {

        if (tensorList.size() != subscriptVectorList.size()) {
            throw std::invalid_argument(
                    "The number of tensors, which is " +
                    std::to_string(tensorList.size()) +
                    ", does not match the number of legs, which is " +
                    std::to_string(subscriptVectorList.size()) + ".");
        }

        std::size_t index = 0;
        // counting occurrence of legs to check constraints
        std::unordered_map<std::size_t, std::size_t> _vertex_leg_map;
        for (auto &legs: subscriptVectorList) {
            // set stable vertex properties
            mGraph[index].legs = std::move(legs);
            mGraph[index].tensor = std::move(tensorList[index]);

            for (int leg: mGraph[index].legs) {
                // 0 is an invalid leg index by convention
                if (leg == 0) {
                    throw std::invalid_argument(ERROR::CONSTRAINT_INVALIDLEG);
                }

                // store negative leg ids as dangling legs
                if (leg < 0) {
                    if (mDanglingLegs.contains(leg)) {
                        throw std::invalid_argument(ERROR::CONSTRAINT_UNIQUELEGS);
                    }
                    mDanglingLegs.insert(leg);
                }

                // store positive legs as edge legs
                if (leg > 0) {
                    // if leg_id has already been seen -> form an edge
                    if (mLegs.contains(leg)) {
                        // obtain previous src from map
                        std::size_t prev = _vertex_leg_map[leg];

                        // add edge between previous src and dest (the current src)
                        auto edge = addEdge(prev, index);

                        // set stable edge properties
                        mGraph[edge].leg = leg;

                        // store to leg-to-edge map
                        mLegToEdge[leg] = edge;

                        // erase entry from map
                        _vertex_leg_map.erase(leg);
                    } else {
                        // store mapping leg to src
                        _vertex_leg_map[leg] = index;

                        // store edge leg
                        mLegs.insert(leg);
                    }
                }
            }
            index++;
        }

        // if the map is not empty we have an error by our restrictions
        if (!_vertex_leg_map.empty()) {
            throw std::invalid_argument(ERROR::CONSTRAINT_LEGPAIRS);
        }

        mConnectedComponents = getConnectedComponents();
    };

    ~TensorNetwork() = default;

    /**
     * TODO add comment
     *
     * @param contractionSequence
     * @return
     */
    void contract(std::vector<int> contractionSequence = {}, std::vector<int> finalOrder = {}) {

        // fill contraction sequence with positive legs if initially empty
        if (contractionSequence.empty()) {
            contractionSequence.insert(contractionSequence.begin(), mLegs.begin(), mLegs.end());
        }

        // fill final order with negative legs if initially empty
        if (finalOrder.empty()) {
            finalOrder.insert(finalOrder.end(), mDanglingLegs.begin(), mDanglingLegs.end());
        }

        // while edges are present iterate over components and contract the tensor(s) corresponding to the vertices
        // within regarding the given sequence of leg identifiers for the contraction
        while (!mLegs.empty()) {
            for (std::vector<std::size_t> &cc: mConnectedComponents) {
                for (std::size_t i: cc) {
                    // retrieve vertex in component
                    auto& v = mGraph[i];
                    // current leg to contract
                    int leg = *contractionSequence.begin();
                    // check if leg_id is in vertex leg_ids
                    auto pos = std::find(v.legs.begin(), v.legs.end(), leg);
                    if (pos != v.legs.end()) {
                        auto edge = mLegToEdge[leg];
                        if (edge.m_source == edge.m_target) {
                            trace(leg);
                        } else {
                            tensordot(leg);
                        }
                        contractionSequence.erase(contractionSequence.begin());
                        mLegToEdge.erase(leg);
                    }
                }
            }
        }

        if (!contractionSequence.empty()) {
            throw std::invalid_argument(
                "The contraction sequence vector, which size is " +
                std::to_string(contractionSequence.size()) +
                ", is not empty.");
        }
    };
};

#endif //NCONPP_TENSORNETWORK_H
