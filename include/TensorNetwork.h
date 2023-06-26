//
// Created by mirco on 2/25/2023.
//

#ifndef NCONPP_TENSORNETWORK_H
#define NCONPP_TENSORNETWORK_H

#include "GraphNew.h"
#include "Tensor.h"

#include <algorithm>
#include <complex>
#include <tuple>
#include <string>

// TODO optimize error handling
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
        std::vector<int> legs;
        npp::tensor_type<T> tensor;
        bool is_singular_vector = false;
    };

    struct custom_edge_properties
    {
        // place custom properties for edges here
    };

    // TODO maybe inherit? TensorNetwork : public Graph
    // graph object for this class
    Graph<custom_vertex_properties, custom_edge_properties> m_graph;

    // store negative and positive legs in separate sets
    std::set<int> m_dangling_legs = {}; // negative leg indices
    std::set<int> m_legs = {};          // positive leg indices

    /**
     * @brief Perform a trace on a specific vertex between axes_a and axes_b.
     *
     * @param vertex_index
     * @param axes_a
     * @param axes_b
     */
    void trace(std::size_t vertex_index, std::size_t axes_a, std::size_t axes_b)
    {
        auto _vertex_properties = m_graph.vertices[vertex_index];
        auto _legs = _vertex_properties.legs;
        auto _tensor = _vertex_properties.tensor;

        if (_vertex_properties.is_singular_vector)
        {
            _tensor = npp::diag(_tensor);
        }

        _tensor = npp::linalg::trace(_tensor, 0, axes_a, axes_b);

        _legs.erase(_legs.begin() + axes_b);
        _legs.erase(_legs.begin() + axes_a);

        m_graph.vertices[vertex_index].legs = std::move(_legs);
        m_graph.vertices[vertex_index].tensor = std::move(_tensor);
    };

    /**
     * @brief Perform a tensordot on specific vertices between axes_a and axes_b.
     *
     * @param src
     * @param dest
     * @param axes_a
     * @param axes_b
     */
    void tensordot(std::size_t src, std::size_t dest, std::vector<std::size_t> axes_a, std::vector<std::size_t> axes_b)
    {
        auto _source_properties = m_graph.vertices[src];
        auto _target_properties = m_graph.vertices[dest];

        auto _legs_a = _source_properties.legs;
        auto _legs_b = _target_properties.legs;

        _legs_a.erase(_legs_a.begin() + axes_a[0]);
        _legs_b.erase(_legs_b.begin() + axes_b[0]);

        std::vector<int> _legs = {};
        _legs.insert(_legs.end(), _legs_a.begin(), _legs_a.end());
        _legs.insert(_legs.end(), _legs_b.begin(), _legs_b.end());

        auto _tensor_a = _source_properties.tensor;
        auto _tensor_b = _target_properties.tensor;

        if (_source_properties.is_singular_vector)
        {
            _tensor_a = npp::diag(_tensor_a);
        }

        if (_target_properties.is_singular_vector)
        {
            _tensor_b = npp::diag(_tensor_b);
        }

        auto shapeA = _tensor_a.shape();
        auto shapeB = _tensor_b.shape();

        auto _tensor = npp::linalg::tensordot(_tensor_a, _tensor_b, axes_a, axes_b);

        m_graph.vertices[src].legs = std::move(_legs);
        m_graph.vertices[src].tensor = std::move(_tensor);
    };

    /**
     * @brief Perform an outer product between two vertices.
     *
     * @param src
     * @param dest
     */
    void outer(std::size_t src, std::size_t dest)
    {
        auto _legs_a = m_graph.vertices[src].legs;
        auto _legs_b = m_graph.vertices[dest].legs;

        auto _tensor_a = m_graph.vertices[src].tensor;
        auto _tensor_b = m_graph.vertices[dest].tensor;

        auto _tensor = npp::linalg::outer(_tensor_a, _tensor_b);

        std::vector<int> _legs = {};
        _legs.insert(_legs.end(), _legs_a.begin(), _legs_a.end());
        _legs.insert(_legs.end(), _legs_b.begin(), _legs_b.end());

        m_graph.vertices[src].legs = std::move(_legs);
        m_graph.vertices[src].tensor = std::move(_tensor);

        m_graph.removeVertex(dest);
    }

    /**
     * @brief Create all edges
     *
     * @param _vertex_leg_map
     * @param _vertex_index
     * @param _subscript_vector
     */
    void create_edges(std::unordered_map<std::size_t, std::size_t> &_vertex_leg_map, int _vertex_index, const std::vector<int> &_subscript_vector)
    {
        for (int _leg_index : _subscript_vector)
        {
            // 0 is an invalid leg index by convention
            if (_leg_index == 0)
            {
                throw std::invalid_argument(ERROR::CONSTRAINT_INVALIDLEG);
            }

            // store negative leg ids as dangling legs
            if (_leg_index < 0)
            {
                if (m_dangling_legs.contains(_leg_index))
                {
                    throw std::invalid_argument(ERROR::CONSTRAINT_UNIQUELEGS);
                }
                m_dangling_legs.insert(_leg_index);
            }

            // store positive legs as edge legs
            if (_leg_index > 0)
            {
                // if leg_id has already been seen -> form an edge
                if (m_legs.contains(_leg_index))
                {
                    // obtain previous src from map
                    std::size_t prev = _vertex_leg_map[_leg_index];

                    // add edge between previous src and dest (the current src)
                    m_graph.addEdge(prev, _vertex_index, _leg_index);

                    // erase entry from map
                    _vertex_leg_map.erase(_leg_index);
                }
                else
                {
                    // store mapping leg to src
                    _vertex_leg_map[_leg_index] = _vertex_index;

                    // store edge leg
                    m_legs.insert(_leg_index);
                }
            }
        }
    }

public:
    /**
     * @brief Forbid empty initialization of Tensor Network object
     *
     */
    TensorNetwork() = delete;

    /**
     * @brief Copy constructor.
     *
     * @param other
     */
    TensorNetwork(const TensorNetwork &other) : m_dangling_legs(other.m_dangling_legs),
                                                m_legs(other.m_legs),
                                                m_graph(other.m_graph)
    {
    }

    /**
     * Explicit copy constructor with given parameters.
     *
     * @param tensor_list
     *  - a list of tensors
     * @param subscript_vector_list
     *  - aka LegsList
     *  - Nomenclature of the legs of the tensor in tensor_list:
     *      - the legs are named by integers
     *      - 0 is not a valid leg identifier
     *      - contractible legs have the same positive integer as identifier, hence occurring in pairs
     *      - legs with negative integers won't be contracted, so-called dangling legs
     */
    explicit TensorNetwork(const std::vector<npp::tensor_type<T>> &tensor_list,
                           const std::vector<std::vector<int>> &subscript_vector_list) : m_graph(tensor_list.size())
    {
        if (tensor_list.size() != subscript_vector_list.size())
        {
            // TODO optimize error handling
            throw std::invalid_argument(
                "The number of tensors, which is " +
                std::to_string(tensor_list.size()) +
                ", does not match the number of legs, which is " +
                std::to_string(subscript_vector_list.size()) + ".");
        }

        // counting occurrence of legs to check constraints
        std::unordered_map<std::size_t, std::size_t> _vertex_leg_map;
        for (std::size_t _vertex_index = 0; _vertex_index < tensor_list.size(); _vertex_index++)
        {
            const std::vector<int> &_subscript_vector = subscript_vector_list[_vertex_index];

            create_edges(_vertex_leg_map, _vertex_index, _subscript_vector);

            m_graph.vertices[_vertex_index].legs = std::move(_subscript_vector);
            m_graph.vertices[_vertex_index].tensor = std::move(tensor_list[_vertex_index]);
        }
    }

    /**
     * @brief Copy assignment
     *
     * @param other
     * @return TensorNetwork&
     */
    TensorNetwork &operator=(const TensorNetwork &other)
    {
        std::swap(m_dangling_legs, other.m_dangling_legs);
        std::swap(m_legs, other.m_legs);
        std::swap(m_graph, other.m_graph);

        return *this;
    }

    /**
     * @brief Move constructor.
     *
     */
    TensorNetwork(TensorNetwork &&other) : m_dangling_legs(other.m_dangling_legs),
                                           m_legs(other.m_legs),
                                           m_graph(other.m_graph)
    {
        other.m_dangling_legs = {};
        other.m_legs = {};
        other.m_graph = {};
    }

    /**
     * Explicit move constructor with given parameters.
     *
     * @param tensor_list
     *  - a list of tensors
     * @param subscript_vector_list
     *  - aka LegsList
     *  - Nomenclature of the legs of the tensor in tensor_list:
     *      - the legs are named by integers
     *      - 0 is not a valid leg identifier
     *      - contractible legs have the same positive integer as identifier, hence occurring in pairs
     *      - legs with negative integers won't be contracted, so-called dangling legs
     */
    explicit TensorNetwork(std::vector<npp::tensor_type<T>> &&tensor_list,
                           std::vector<std::vector<int>> &&subscript_vector_list) : m_graph(tensor_list.size())
    {
        if (tensor_list.size() != subscript_vector_list.size())
        {
            throw std::invalid_argument(
                "The number of tensors, which is " +
                std::to_string(tensor_list.size()) +
                ", does not match the number of legs, which is " +
                std::to_string(subscript_vector_list.size()) + ".");
        }

        // counting occurrence of legs to check constraints
        std::unordered_map<std::size_t, std::size_t> _vertex_leg_map;
        for (std::size_t _vertex_index = 0; _vertex_index < tensor_list.size(); _vertex_index++)
        {
            const std::vector<int> &_subscript_vector = subscript_vector_list[_vertex_index];

            create_edges(_vertex_leg_map, _vertex_index, _subscript_vector);
                
            m_graph.vertices[_vertex_index].legs = std::move(subscript_vector_list[_vertex_index]);
            m_graph.vertices[_vertex_index].tensor = std::move(tensor_list[_vertex_index]);
        }
    };

    /**
     * @brief Move assignment
     *
     * @param other
     * @return TensorNetwork&
     */
    TensorNetwork &operator=(TensorNetwork &&other)
    {
        m_dangling_legs = std::move(other.m_dangling_legs);
        m_legs = std::move(other.m_legs);
        m_graph = std::move(other.m_graph);

        return *this;
    }

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
        return m_dangling_legs;
    }

    /**
     * @brief Retrieve current legs (positive indices).
     *
     */
    const std::set<int> &Legs()
    {
        return m_legs;
    }

    /**
     *
     * @param contraction_sequence
     *  - order by legs in which the tensors shall be contracted
     * @param final_order
     *  - permutation of the legs of the final tensors
     */
    void contract(std::vector<int> contraction_sequence = {}, std::vector<int> final_order = {})
    {
        // fill contraction sequence with positive legs if initially empty
        if (contraction_sequence.empty())
        {
            contraction_sequence.insert(contraction_sequence.begin(), m_legs.begin(), m_legs.end());
        }

        // fill final order with negative legs if initially empty
        if (final_order.empty())
        {
            final_order.insert(final_order.end(), m_dangling_legs.begin(), m_dangling_legs.end());
        }

        while (!contraction_sequence.empty())
        {

            int _leg_index = *contraction_sequence.begin();
            auto _edge = m_graph.edges[_leg_index];

            auto _src = _edge.src;
            auto _dest = _edge.dest;

            if (_src == _dest)
            { // trace

                std::vector<std::size_t> _axes = {};
                for (auto _axis = 0; _axis < m_graph.vertices[_dest].legs.size(); _axis++)
                {
                    if (m_graph.vertices[_dest].legs[_axis] == _leg_index)
                    {
                        _axes.emplace_back(_axis);
                    }
                }

                trace(_src, _axes[0], _axes[1]);

                m_graph.removeEdge(_leg_index);
            }
            else
            { // tensordot

                // TODO add multi axis tensor contraction?
                std::vector<std::size_t> _axes_a = {};
                std::vector<std::size_t> _axes_b = {};
                for (auto _axis = 0; _axis < m_graph.vertices[_src].legs.size(); _axis++)
                {
                    if (m_graph.vertices[_src].legs[_axis] == _leg_index)
                    {
                        _axes_a.emplace_back(_axis);
                    }
                }
                for (auto _axis = 0; _axis < m_graph.vertices[_dest].legs.size(); _axis++)
                {
                    if (m_graph.vertices[_dest].legs[_axis] == _leg_index)
                    {
                        _axes_b.emplace_back(_axis);
                    }
                }

                tensordot(_src, _dest, _axes_a, _axes_b);

                m_graph.removeEdge(_leg_index);
                // TODO m_graph.mergeVertices(_src, _dest);
            }
            contraction_sequence.erase(contraction_sequence.begin());
            m_legs.erase(_leg_index);
        }

        if (!contraction_sequence.empty())
        {
            throw std::invalid_argument(
                "The contraction sequence vector, which size is " +
                std::to_string(contraction_sequence.size()) +
                ", is not empty.");
        }
    }

    /**
     * @brief Connect all tensors into a single one by outer products.
     *
     */
    void connect()
    {
        auto _nv = m_graph.NumVertices();
        while (_nv > 1)
        {
            outer(0, 1);
            _nv--;
        }
    }

    /**
     *
     * @return
     */
    std::size_t NumTensors()
    {
        return m_graph.NumVertices();
    }

    /**
     * @brief Retrieve a current view of tensors.
     *
     * @return const std::vector<npp::tensor_type<T>>&
     */
    std::vector<npp::tensor_type<T>> TensorList()
    {
        auto _nv = m_graph.NumVertices();
        std::vector<npp::tensor_type<T>> _result(_nv);
        for (int _i = 0; _i < _nv; _i++)
        {
            _result[_i] = m_graph.vertices[_i].tensor;
        }
        return _result;
    }

    /**
     * @brief Retrieve a current view of the shapes of the tensors.
     *
     * @return std::vector<npp::shape_type>
     */
    std::vector<npp::shape_type> TensorShapes()
    {
        auto _nv = m_graph.NumVertices();
        std::vector<npp::shape_type> _result(_nv);
        for (int _i = 0; _i < _nv; _i++)
        {
            _result[_i] = m_graph.vertices[_i].tensor.shape();
        }
        return _result;
    }

    // /**
    //  * @brief Split a vertex on a specific leg. The position to split is between leg_index and leg_index + 1.
    //  *
    //  * @param leg_index
    //  * @param vertex_index
    //  */
    // void split(std::size_t vertex_index, std::size_t leg_index)
    // {
    //     // throw std::logic_error("Not finally implemented yet. Current TODO: update edges.");

    //     auto _vertex = m_graph.vertices[vertex_index];
    //     std::vector<int> _legs = _vertex.legs;
    //     auto _tensor = _vertex.tensor;

    //     // split legs
    //     std::vector<int> _left_legs(_legs.begin(), _legs.begin() + leg_index);
    //     std::vector<int> _right_legs(_legs.begin() + leg_index, _legs.end());

    //     // split tensor
    //     std::size_t _len = _legs.size();
    //     if (leg_index < _len)
    //     {
    //         auto _shape = npp::shape(_tensor);

    //         std::size_t _left = 1, _right = 1;
    //         npp::shape_type _left_shape, _right_shape;
    //         for (std::size_t _i = 0; _i < _len; _i++)
    //         {
    //             if (_i < leg_index)
    //             {
    //                 _left *= _shape[_i];

    //                 _left_shape.push_back(_shape[_i]);
    //             }
    //             else if (_i == leg_index)
    //             {
    //                 _right *= _shape[_i];

    //                 // new shape after svd
    //                 _left_shape.push_back(_left);
    //                 _right_shape.push_back(_left);

    //                 _right_shape.push_back(_shape[_i]);
    //             }
    //             else
    //             {
    //                 _right *= _shape[_i];

    //                 _right_shape.push_back(_shape[_i]);
    //             }
    //         }

    //         npp::reshape(_tensor, npp::shape_type({_left, _right}));

    //         npp::tensor_type<T> _U, _s, _V;
    //         std::tie(_U, _s, _V) = npp::linalg::svd(_tensor, false);

    //         // reshape U,V back to tensors with new shape
    //         npp::reshape(_U, _left_shape);
    //         npp::reshape(_V, _right_shape);

    //         // new leg ids
    //         int _new_leg_left = 1;
    //         if (!m_legs.empty())
    //         {
    //             _new_leg_left = *m_legs.rbegin() + 1;
    //         }
    //         int _new_leg_right = _new_leg_left + 1;

    //         // add to the subscript vectors
    //         _left_legs.emplace_back(_new_leg_left);
    //         std::vector<int> s_legs{_new_leg_left, _new_leg_right};
    //         _right_legs.emplace(_right_legs.begin(), _new_leg_right);

    //         // add to list of current legs
    //         m_legs.insert(_new_leg_left);
    //         m_legs.insert(_new_leg_right);

    //         // update current vertex for U
    //         m_graph.vertices[vertex_index] = vertex_properties_t{.legs = std::move(_left_legs), .tensor = std::move(_U)});

    //         // and create new vertices for s and V
    //         // TODO
    //         auto _s_vertex = m_graph.addVertex(vertex_properties_t{.legs = std::move(s_legs), .tensor = std::move(_s), .singular_value = true});
    //         auto _V_vertex = m_graph.addVertex(vertex_properties_t{.legs = std::move(_right_legs), .tensor = std::move(_V)});

    //         // add new edges to graph
    //         m_graph.addEdge(vertex_index, _s_vertex, _new_leg_left);
    //         m_graph.addEdge(_s_vertex, _V_vertex, _new_leg_right);

    //         // only edges of rhs needs to be updated, because edges of lhs are reused in U
    //         for (int _i : _right_legs)
    //         {
    //             if (_i > 0)
    //             {
    //                 // pair(src, tar)
    //                 auto _edge = m_graph.getEdge(_i);

    //                 if (_edge.first != _V_vertex)
    //                 {
    //                     m_graph.updateEdge(_i, _V_vertex, _edge.second);
    //                 }
    //             }
    //         }
    //     }
    //     else
    //     {
    //         throw std::invalid_argument(ERROR::OUT_OF_SIZE);
    //     }
    // }
};

#endif // NCONPP_TENSORNETWORK_H
