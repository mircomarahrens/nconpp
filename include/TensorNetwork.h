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

    typedef typename boost::adjacency_list
            <boost::vecS, boost::vecS, boost::undirectedS, vertex_properties, boost::property<boost::edge_index_t, std::size_t>> graph_t;

    typedef typename boost::graph_traits<graph_t>::edge_descriptor edge;
    typedef typename boost::property_map<graph_t, boost::edge_index_t>::type edge_index_pm;
    typedef typename boost::graph_traits<graph_t>::edge_iterator edge_it;
    typedef typename boost::graph_traits<graph_t>::vertex_descriptor vertex;
    typedef typename boost::graph_traits<graph_t>::adjacency_iterator Adjacency_Iterator;

    graph_t mGraph;

    // store negative and positive legs in separate sets
    std::set<int> mDanglingLegs = {};
    std::set<int> mLegs = {};

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
     *
     *
     * @param src
     * @param dest
     * @return
     */
    void contractVertices(vertex src, vertex dest) {

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
     * Add a new edge to the graph.
     * @param src
     * @param dest
     * @param leg
     */
    auto addEdge(std::size_t src, std::size_t dest, std::size_t leg) {
        auto s = boost::vertex(src, mGraph);
        auto d = boost::vertex(dest, mGraph);
        auto e = boost::add_edge(s, d, leg, mGraph);
        assert(e.second == true);
        return e.first;
    }

    /**
     * Retrieve all edges as pair of vertex indices.
     * @return
     */
    std::vector<std::pair<std::size_t, std::size_t>> getEdgeVertices() {
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
    std::vector<std::vector<std::size_t>> getConnectedComponents() {
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
     * TODO add comment
     * inplace manipulation of tensors
     *
     * @param leg_id
     *
     */
    void trace(vertex vertex, std::size_t axesA, std::size_t axesB) {
        auto newTensor = npp::linalg::trace(mGraph[vertex].tensor, 0, axesA, axesB);
        mGraph[vertex].tensor = newTensor;

        auto legs = mGraph[vertex].legs;
        legs.erase(legs.begin() + axesB);
        legs.erase(legs.begin() + axesA);
        mGraph[vertex].legs = std::move(legs);
    };

    /**
     * TODO add comment
     * inplace manipulation of tensors
     *
     * @param leg_id
     */
    void tensordot(vertex src, vertex dest, std::vector<std::size_t> axesA, std::vector<std::size_t> axesB) {
        auto legsA = mGraph[src].legs;
        auto legsB = mGraph[dest].legs;

        auto tensorA = mGraph[src].tensor;
        auto tensorB = mGraph[dest].tensor;

        auto newTensor = npp::linalg::tensordot(tensorA, tensorB, axesA, axesB);

        legsA.erase(legsA.begin() + axesA[0]);
        legsB.erase(legsB.begin() + axesB[0]);

        std::vector<int> newLegs = {};
        newLegs.insert(newLegs.end(), legsA.begin(), legsA.end());
        newLegs.insert(newLegs.end(), legsB.begin(), legsB.end());

        mGraph[src].tensor = std::move(newTensor);
        mGraph[src].legs = std::move(newLegs);
    };

    /**
     * Perform an outer product of two vertices.
     * @param src
     * @param dest
     */
    void outer(vertex src, vertex dest) {
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
        boost::clear_vertex(dest, mGraph);
        boost::remove_vertex(dest, mGraph);
    }

public:
    /**
     * Explicit constructor with given parameters.
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
    explicit TensorNetwork(std::vector<npp::tensor<T>> &tensorList,
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
                        addEdge(prev, index, leg);

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

        if (!_vertex_leg_map.empty()) {
            throw std::invalid_argument(ERROR::CONSTRAINT_LEGPAIRS);
        }

        mConnectedComponents = getConnectedComponents();
    };

    ~TensorNetwork() = default;

    /**
     *
     * @param contractionSequence
     *  - order by legs in which the tensors shall be contracted
     * @param finalOrder
     *  - permutation of the legs of the final tensors
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

        while (!contractionSequence.empty()) {

            int leg = *contractionSequence.begin();

            edge_index_pm edge_id = boost::get(boost::edge_index_t(), mGraph);

            std::pair<edge_it, edge_it> edge_its = boost::edges(mGraph);

            edge_it first = edge_its.first;
            edge_it last = edge_its.second;

            while (first != last) {
                int _leg = edge_id[*first];

                if (leg == _leg) {
                    auto src = first->m_source;
                    auto dest = first->m_target;

                    if (src == dest) { // trace

                        std::vector<std::size_t> axes = {};
                        for (auto axis = 0; axis < mGraph[dest].legs.size(); axis++) {
                            if (mGraph[dest].legs[axis] == leg) {
                                axes.emplace_back(axis);
                            }
                        }

                        trace(src, axes[0], axes[1]);

                        boost::remove_edge(*first, mGraph);
                    } else { // tensordot

                        // TODO add multi axis tensor contraction?
                        std::vector<std::size_t> axesA = {};
                        std::vector<std::size_t> axesB = {};

                        for (auto axis = 0; axis < mGraph[src].legs.size(); axis++) {
                            if (mGraph[src].legs[axis] == leg) {
                                axesA.emplace_back(axis);
                            }
                        }
                        for (auto axis = 0; axis < mGraph[dest].legs.size(); axis++) {
                            if (mGraph[dest].legs[axis] == leg) {
                                axesB.emplace_back(axis);
                            }
                        }

                        tensordot(src, dest, axesA, axesB);

                        // add src edges to dest edges
                        auto dest_edges = boost::out_edges(dest, mGraph);
                        auto f_it = dest_edges.first; auto e_it = dest_edges.second;
                        while (f_it != e_it) {
                            int nleg = edge_id[*f_it];
                            boost::add_edge(src, f_it->m_target, nleg, mGraph);
                            f_it++;
                        }

                        // clear and remove dest vertex
                        boost::clear_vertex(dest, mGraph);
                        boost::remove_vertex(dest, mGraph);
                    }
                    contractionSequence.erase(contractionSequence.begin());
                    break;
                }
                first++;
            }
        }

        if (!contractionSequence.empty()) {
            throw std::invalid_argument(
                    "The contraction sequence vector, which size is " +
                    std::to_string(contractionSequence.size()) +
                    ", is not empty.");
        }
    }

    void connect() {
        auto nv = boost::num_vertices(mGraph);
        while (nv > 1) {
            auto src = boost::vertex(0, mGraph);
            auto dest = boost::vertex(1, mGraph);
            outer(src, dest);
            nv--;
        }
    }

    /**
     *
     * @return
     */
    std::size_t num_tensors() {
        return boost::num_vertices(mGraph);
    }

    /**
     *
     * @return
     */
    std::vector<npp::tensor<T>> getTensorList() {
        std::vector<npp::tensor<T>> result = {};
        typename graph_t::vertex_iterator v, vend;
        for (boost::tie(v, vend) = boost::vertices(mGraph); v != vend; ++v) {
            result.emplace_back(mGraph[*v].tensor);
        }
        return result;
    }
};

#endif //NCONPP_TENSORNETWORK_H
