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
        std::string graph_type = "Lattice";
    };

    template <class V = GRAPH_PROPERTIES::default_t>
    struct custom_vertex_properties : V
    {
        std::vector<std::size_t> cartesian_coordinate;
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
    LatticeGraph(const std::string _graph_type,
                 const npp::shape_type &_grid_shape,
                 const npp::tensor_type<int> &_directions,
                 const std::vector<std::string> &_bcs)
        : m_grid_shape(_grid_shape), m_directions(_directions)
    {
        graph_properties.graph_type = _graph_type;

        m_boundary_grid_shape.resize(m_grid_shape.size());
        for (int i = 0; i < m_grid_shape.size(); ++i)
        {
            m_boundary_grid_shape[i] = m_grid_shape[i] + 2;
        }

        lattice_initialization();
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

    const npp::tensor_type<std::size_t> &getDirections() const
    {
        return m_directions;
    }

private:
    npp::shape_type m_grid_shape;
    npp::shape_type m_boundary_grid_shape;
    npp::tensor_type<std::size_t> m_directions;

    void lattice_initialization()
    {
        // get number of sites
        int _sites = npp::prod(m_boundary_grid_shape);

        // iterate over sites
        for (int i = 0; i < _sites; i++)
        {
            // store cartesian coordinate to site as vertex property
            auto &v = this->vertices[i];
            v.cartesian_coordinate =
                npp::unravel_index(i, m_boundary_grid_shape);

            // check if site is a boundary site and store it as vertex property
            for (std::size_t i : v.cartesian_coordinate)
            {
                if (i == 0 || i == m_boundary_grid_shape[0] - 1)
                {
                    v.boundary = true;
                    break;
                }
            }
        }

        int edge_index = 0;
        for (auto &v : this-> vertices)
        {
            if (!v.second.boundary)
            {
                // iterate over directions
                for (int j = 0; j < m_directions.shape()[0]; j++)
                {
                    // get cartesian coordinate of neighbour site
                    auto _cartesian_coordinate_neighbour = v.second.cartesian_coordinate;
                    for (int i = 0; i < _cartesian_coordinate_neighbour.size(); ++i)
                    {
                        _cartesian_coordinate_neighbour[i] += m_directions[i];
                    }

                    // get neighbour site
                    int _neighbour_site = npp::ravel_index(_cartesian_coordinate_neighbour, m_boundary_grid_shape);

                    // check if neighbour site exists
                    if (_neighbour_site != -1)
                    {
                        // add edge
                        this->addEdge(edge_index, v.first, _neighbour_site);
                        edge_index++;
                    }
                }
            }
        }
    }
};

#endif // NCONPP_LATTICEGRAPH_H
