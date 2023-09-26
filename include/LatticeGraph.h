//
// Created by mirco on 9/22/2023.
//

#ifndef NCONPP_LATTICEGRAPH_H
#define NCONPP_LATTICEGRAPH_H

#include "Graph.h"
#include "Tensor.h"

namespace GRAPH_PROPERTIES
{
    template <class G = GRAPH_PROPERTIES::default_t>
    struct custom_graph_properties : G
    {
        std::string name = "Lattice";
    };

    template <class V = GRAPH_PROPERTIES::default_t>
    struct custom_vertex_properties : V
    {
        std::vector<std::size_t> coordinate;
        bool boundary;
    };

    template <class E = GRAPH_PROPERTIES::default_t>
    struct custom_edge_properties : E
    {
    };
}

class LatticeGraph : public Graph<GRAPH_PROPERTIES::custom_graph_properties<>, GRAPH_PROPERTIES::custom_vertex_properties<>, GRAPH_PROPERTIES::custom_edge_properties<>>
{
public:
    using directions_type = std::vector<std::vector<std::vector<int>>>;

    LatticeGraph(const std::string &_name,
                 const npp::shape_type &_grid_shape,
                 const directions_type &_directions,
                 const std::vector<std::string> &_bcs)
        : m_grid_shape(_grid_shape), m_directions(_directions), m_bcs(_bcs)
    {
        graph_properties.name = _name;

        m_boundary_grid_shape.resize(m_grid_shape.size());
        for (int i = 0; i < m_grid_shape.size(); ++i)
        {
            m_boundary_grid_shape[i] = m_grid_shape[i] + 2;
        }

        m_lattice_initialization();
    };

    ~LatticeGraph() = default;

    const npp::shape_type &getGridShape() const
    {
        return m_grid_shape;
    }

    const npp::shape_type &getBoundaryGridShape() const
    {
        return m_boundary_grid_shape;
    }

    const directions_type &getDirections() const
    {
        return m_directions;
    }

private:
    npp::shape_type m_grid_shape;
    npp::shape_type m_boundary_grid_shape;
    directions_type m_directions;
    std::vector<std::string> m_bcs;

    void m_lattice_initialization()
    {
        // get number of sites
        std::size_t _sites = npp::prod(m_boundary_grid_shape);

        // iterate over sites
        for (std::size_t _i = 0; _i < _sites; _i++)
        {
            // store cartesian coordinate to site as vertex property
            auto &v = this->vertices[_i];
            v.coordinate =
                npp::unravel_index(_i, m_boundary_grid_shape);

            // check if site is a boundary site and store it as vertex property
            for (std::size_t _ic = 0; _ic < v.coordinate.size(); _ic++)
            {
                int _c = v.coordinate[_ic];
                if (_c == 0 || _c == m_boundary_grid_shape[_ic] - 1)
                {
                    v.boundary = true;
                    break;
                }
            }
        }

        int _edge_index = 0;
        for (auto &_vertex : this->vertices)
        {
            if (!_vertex.second.boundary)
            {
                auto _origin_index = _vertex.first;
                auto _origin = _vertex.second.coordinate;
                auto _dirs = m_directions[std::reduce(_origin.begin(), _origin.end()) % m_directions.size()];

                for (auto _dir : _dirs)
                {
                    // get cartesian coordinate of neighbour site
                    auto _target_coordinate = _vertex.second.coordinate;
                    for (std::size_t _id = 0; _id < _dir.size(); ++_id)
                    {
                        _target_coordinate[_id] += _dir[_id];
                    }

                    // ravel the target coordinate to index representation
                    std::size_t _target_index =
                        npp::ravel_index(_target_coordinate, m_boundary_grid_shape);

                    if (this->vertices[_target_index].boundary)
                    {
                        for (std::size_t _ib = 0; _ib < m_bcs.size(); ++_ib)
                        {
                            if (m_bcs[_ib] == "obc")
                            {
                                continue;
                            }
                            else if (m_bcs[_ib] == "pbc")
                            {
                                if (_target_coordinate[_ib] == 0)
                                {
                                    _target_coordinate[_ib] = m_grid_shape[_ib];
                                }
                                else if (_target_coordinate[_ib] == m_boundary_grid_shape[_ib] - 1)
                                {
                                    _target_coordinate[_ib] = 1;
                                }
                            }
                        }
                    }

                    // re-ravel
                    _target_index =
                        npp::ravel_index(_target_coordinate, m_boundary_grid_shape);

                    // check if edge is already present
                    if (adjacency_list[_target_index].find(_origin_index) == adjacency_list[_target_index].end())
                    {
                        // add edge
                        this->addEdge(_edge_index, _origin_index, _target_index);
                        _edge_index++;
                    }
                }
            }
        }
    }
};

#endif // NCONPP_LATTICEGRAPH_H
