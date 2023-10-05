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
    const static std::string CONSTRAINT_UNIQUELEGS = "Only unique dangling leg indices are allowed by convention.";
    const static std::string OUT_OF_SIZE = "The position to split is not within the range of legs.";
}

namespace WARNING
{

}

namespace INFO
{
    const static std::string DISCONNECTED_NETWORKS = "The network is not continuously connected.";
}

namespace GRAPH_PROPERTIES
{
    // custom graph properties
    struct custom_graph_properties
    {
        // place custom properties for the graph here
    };

    // custom vertex properties
    template <typename U = std::complex<double>>
    struct custom_vertex_properties
    {
        // place custom properties for vertices here
        std::vector<int> cartesian_coordinates;
        std::vector<int> legs; // TODO maybe change edge_indices from set to vector?
        npp::tensor_type<U> tensor;
    };

    // custom edge properties
    template <typename U = std::complex<double>>
    struct custom_edge_properties
    {
        // place custom properties for edges here
        npp::tensor_type<U> singular_values;
    };
}

template <typename T = std::complex<double>>
class TensorNetwork : public Graph<GRAPH_PROPERTIES::custom_graph_properties, GRAPH_PROPERTIES::custom_vertex_properties<T>, GRAPH_PROPERTIES::custom_edge_properties<T>>
{
private:
    // store negative and positive legs in separate sets
    std::set<int> m_dangling_legs = {}; // negative leg indices
    std::set<int> m_legs = {};          // positive leg indices

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
                    this->addEdge(_leg_index, prev, _vertex_index);

                    this->edges[_leg_index].singular_values = {};

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
                                                Graph<GRAPH_PROPERTIES::custom_graph_properties, GRAPH_PROPERTIES::custom_vertex_properties<T>, GRAPH_PROPERTIES::custom_edge_properties<T>>()
    {
        this->vertices = other.vertices;
        this->edges = other.edges;
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
                           const std::vector<std::vector<int>> &subscript_vector_list) : Graph<GRAPH_PROPERTIES::custom_graph_properties, GRAPH_PROPERTIES::custom_vertex_properties<T>, GRAPH_PROPERTIES::custom_edge_properties<T>>(tensor_list.size())
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

            this->vertices[_vertex_index].legs = std::move(_subscript_vector);
            this->vertices[_vertex_index].tensor = std::move(tensor_list[_vertex_index]);
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
        std::swap(this->vertices, other.vertices);
        std::swap(this->edges, other.edges);

        return *this;
    }

    /**
     * @brief Move constructor.
     *
     */
    TensorNetwork(TensorNetwork &&other) : m_dangling_legs(other.m_dangling_legs),
                                           m_legs(other.m_legs),
                                           Graph<GRAPH_PROPERTIES::custom_graph_properties, GRAPH_PROPERTIES::custom_vertex_properties<T>, GRAPH_PROPERTIES::custom_edge_properties<T>>()
    {
        other.m_dangling_legs = {};
        other.m_legs = {};

        this->vertices = std::move(other.vertices);
        this->edges = std::move(other.edges);

        other.vertices = {};
        other.edges = {};
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
                           std::vector<std::vector<int>> &&subscript_vector_list) : Graph<GRAPH_PROPERTIES::custom_graph_properties, GRAPH_PROPERTIES::custom_vertex_properties<T>, GRAPH_PROPERTIES::custom_edge_properties<T>>(tensor_list.size())
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

            this->vertices[_vertex_index].legs = std::move(subscript_vector_list[_vertex_index]);
            this->vertices[_vertex_index].tensor = std::move(tensor_list[_vertex_index]);
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
        this->vertices = std::move(other.vertices);
        this->edges = std::move(other.edges);

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
            auto _edge = this->edges[_leg_index];

            auto _src = _edge.src;
            auto &_src_vertex = this->vertices[_src];
            auto &_src_legs = _src_vertex.legs;
            auto &_src_tensor = _src_vertex.tensor;

            auto _dest = _edge.dest;
            auto &_dest_vertex = this->vertices[_dest];
            auto &_dest_legs = _dest_vertex.legs;
            auto &_dest_tensor = _dest_vertex.tensor;

            if (_src == _dest)
            { // trace

                std::vector<std::size_t> _axes = {};
                for (auto _axis = 0; _axis < _src_legs.size(); _axis++)
                {
                    if (_src_legs[_axis] == _leg_index)
                    {
                        _axes.emplace_back(_axis);
                    }
                }

                _src_legs.erase(_src_legs.begin() + _axes[1]);
                _src_legs.erase(_src_legs.begin() + _axes[0]);

                _src_tensor = npp::linalg::trace(_src_tensor, 0, _axes[0], _axes[1]);

                this->removeEdge(_leg_index);
            }
            else
            { // tensordot

                // TODO add multi axis tensor contraction?
                std::vector<std::size_t> _axes_a = {};
                std::vector<std::size_t> _axes_b = {};
                for (auto _axis = 0; _axis < _src_legs.size(); _axis++)
                {
                    if (_src_legs[_axis] == _leg_index)
                    {
                        _axes_a.emplace_back(_axis);
                    }
                }
                for (auto _axis = 0; _axis < _dest_legs.size(); _axis++)
                {
                    if (_dest_legs[_axis] == _leg_index)
                    {
                        _axes_b.emplace_back(_axis);
                    }
                }

                _src_legs.erase(_src_legs.begin() + _axes_a[0]);
                _dest_legs.erase(_dest_legs.begin() + _axes_b[0]);

                std::vector<int> _legs = {};
                _legs.insert(_legs.end(), _src_legs.begin(), _src_legs.end());
                _legs.insert(_legs.end(), _dest_legs.begin(), _dest_legs.end());

                // set new legs to src vertex
                _src_legs = _legs;

                if (npp::dimension(_edge.singular_values) > 0)
                {
                    _src_tensor = npp::linalg::tensordot(_src_tensor, npp::diag(_edge.singular_values), _axes_a, {0});
                }

                // tensordot and store result to src vertex
                _src_tensor = npp::linalg::tensordot(_src_tensor, _dest_tensor, _axes_a, _axes_b);

                this->removeEdge(_leg_index);

                // merge dest vertex into src vertex
                this->mergeVertices(_src, _dest);
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
     * @brief Retrieve the current number of vertices.
     *
     * @return std::size_t
     */
    std::size_t NumTensors()
    {
        return this->NumVertices();
    }

    /**
     * @brief Retrieve a current view of the shapes of the tensors.
     *
     * @return std::vector<npp::shape_type>
     */
    std::vector<npp::shape_type> TensorShapes()
    {
        auto _nv = this->NumVertices();
        std::vector<npp::shape_type> _result(_nv);
        for (int _i = 0; _i < _nv; _i++)
        {
            _result[_i] = this->vertices[_i].tensor.shape();
        }
        return _result;
    }

    /**
     * @brief Split a vertex on a specific leg. The position to split is between leg_index and leg_index + 1.
     *
     * @param leg_index
     * @param vertex_index
     */
    void split(std::size_t vertex_index, std::size_t leg_index)
    {
        auto _tensor = this->vertices[vertex_index].tensor;

        // split legs
        std::vector<int> _left_legs(this->vertices[vertex_index].legs.begin(), this->vertices[vertex_index].legs.begin() + leg_index);
        std::vector<int> _right_legs(this->vertices[vertex_index].legs.begin() + leg_index, this->vertices[vertex_index].legs.end());

        // split tensor
        std::size_t _len = this->vertices[vertex_index].legs.size();
        if (leg_index < _len)
        {
            auto _shape = npp::shape(_tensor);

            std::size_t _left = 1, _right = 1;
            npp::shape_type _left_shape, _right_shape;
            for (std::size_t _i = 0; _i < _len; _i++)
            {
                if (_i < leg_index)
                {
                    _left *= _shape[_i];

                    _left_shape.push_back(_shape[_i]);
                }
                else if (_i == leg_index)
                {
                    _right *= _shape[_i];

                    // new shape after svd
                    _left_shape.push_back(_left);
                    _right_shape.push_back(_left);

                    _right_shape.push_back(_shape[_i]);
                }
                else
                {
                    _right *= _shape[_i];

                    _right_shape.push_back(_shape[_i]);
                }
            }

            npp::reshape(_tensor, npp::shape_type({_left, _right}));

            npp::tensor_type<T> _U, _s, _V;
            std::tie(_U, _s, _V) = npp::linalg::svd(_tensor, false);

            // reshape U,V back to tensors with new shape
            npp::reshape(_U, _left_shape);
            npp::reshape(_V, _right_shape);

            // new leg ids
            int _new_leg = 1;
            if (!m_legs.empty())
            {
                _new_leg = *m_legs.rbegin() + 1;
            }

            // add to the subscript vectors
            _left_legs.emplace_back(_new_leg);
            _right_legs.emplace(_right_legs.begin(), _new_leg);

            // add to list of current legs
            m_legs.insert(_new_leg);

            // update current vertex for U
            this->vertices[vertex_index].legs = std::move(_left_legs);
            this->vertices[vertex_index].tensor = std::move(_U);

            // and create new vertex for V
            std::size_t _V_vertex = this->addVertex();
            this->vertices[_V_vertex].legs = std::move(_right_legs);
            this->vertices[_V_vertex].tensor = std::move(_V);

            // add new edge to graph
            this->addEdge(_new_leg, vertex_index, _V_vertex);

            // and store singular values s as edge property
            this->edges[_new_leg].singular_values = std::move(_s);

            // only edges of rhs needs to be updated, because edges of lhs are reused in U
            for (int _i : _right_legs)
            {
                if (_i > 0)
                {
                    this->edges[_i].src = _V_vertex;
                }
            }
        }
        else
        {
            throw std::invalid_argument(ERROR::OUT_OF_SIZE);
        }
    }

    /**
     * @brief Perform an outer product between two vertices.
     *
     * @param src
     * @param dest
     */
    void outer(std::size_t src, std::size_t dest)
    {
        auto _legs_a = this->vertices[src].legs;
        auto _legs_b = this->vertices[dest].legs;

        auto _tensor_a = this->vertices[src].tensor;
        auto _tensor_b = this->vertices[dest].tensor;

        auto _tensor = npp::linalg::outer(_tensor_a, _tensor_b);

        std::vector<int> _legs = {};
        _legs.insert(_legs.end(), _legs_a.begin(), _legs_a.end());
        _legs.insert(_legs.end(), _legs_b.begin(), _legs_b.end());

        this->vertices[src].legs = std::move(_legs);
        this->vertices[src].tensor = std::move(_tensor);

        this->removeVertex(dest);
    }

    /**
     * @brief Connect all remaining vertices consecutive starting with the most left one.
     *
     */
    void connect()
    {
        auto vs = this->getVertices();
        auto v0 = *vs.begin();
        std::size_t c = 0;
        for (const auto &el : vs)
        {
            if (c > 0)
            {
                outer(v0, el);
            }
            else
            {
                c += 1;
            }
        }
    }
};

#endif // NCONPP_TENSORNETWORK_H
