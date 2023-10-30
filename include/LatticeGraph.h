// Copyright 2023 Mirco Marahrens

#ifndef INCLUDE_LATTICEGRAPH_H_
#define INCLUDE_LATTICEGRAPH_H_

#include <string>
#include <vector>

#include "Graph.h"
#include "Tensor.h"

namespace GRAPH_PROPERTIES {
struct lattice_graph_properties {
  std::string name;
};

struct lattice_vertex_properties {
  std::vector<std::size_t> coordinate;
  bool boundary;
};

struct lattice_edge_properties {
  bool boundary;
};
}  // namespace GRAPH_PROPERTIES

class LatticeGraph : public Graph<GRAPH_PROPERTIES::lattice_graph_properties,
                                  GRAPH_PROPERTIES::lattice_vertex_properties,
                                  GRAPH_PROPERTIES::lattice_edge_properties> {
 public:
  using directions_type = std::vector<std::vector<std::vector<int>>>;

  LatticeGraph(const std::string &_name = "Honeycomb",
               const std::vector<std::size_t> &_grid_shape = {4, 4},
               const directions_type &_directions =
                   {{{0, -1}, {0, +1}, {+1, 0}}, {{-1, 0}, {0, -1}, {0, +1}}},
               const std::vector<std::string> &_bcs = {"pbc", "pbc"})
      : m_grid_shape(_grid_shape), m_directions(_directions), m_bcs(_bcs) {
    this->graph_properties.parallel_edges = false;
    this->graph_properties.name = _name;

    m_boundary_grid_shape.resize(m_grid_shape.size());
    for (int i = 0; i < m_grid_shape.size(); ++i) {
      m_boundary_grid_shape[i] = m_grid_shape[i] + 2;
    }

    m_lattice_initialization();
  }

  ~LatticeGraph() = default;

  const std::vector<std::size_t> &getGridShape() const { return m_grid_shape; }

  const std::vector<std::size_t> &getBoundaryGridShape() const {
    return m_boundary_grid_shape;
  }

  const directions_type &getDirections() const { return m_directions; }

 private:
  std::vector<std::size_t> m_grid_shape;
  std::vector<std::size_t> m_boundary_grid_shape;
  directions_type m_directions;
  std::vector<std::string> m_bcs;

  void m_lattice_initialization() {
    // get number of sites
    std::size_t _sites = npp::prod(m_boundary_grid_shape);

    // vertices
    for (std::size_t _i = 0; _i < _sites; _i++) {
      // store cartesian coordinate to site as vertex property
      auto &v = this->vertices[_i];
      v.coordinate = npp::unravel_index(_i, m_boundary_grid_shape);

      // check if site is a boundary site, store the result as vertex property
      for (std::size_t _ic = 0; _ic < v.coordinate.size(); _ic++) {
        int _c = v.coordinate[_ic];
        if (_c == 0 || _c == m_boundary_grid_shape[_ic] - 1) {
          v.boundary = true;
          break;
        }
      }
    }

    // edges
    int _edge_index = 0;
    for (auto &_vertex : this->vertices) {
      if (!_vertex.second.boundary) {
        auto _origin_id = _vertex.first;
        auto _origin_co = _vertex.second.coordinate;

        // get possible directions to site in unit cell
        auto _dirs =
            m_directions[std::reduce(_origin_co.begin(), _origin_co.end()) %
                         m_directions.size()];

        // iterate over possible directions
        for (auto _dir : _dirs) {
          // get cartesian coordinate of neighbour site
          auto _target_co = _vertex.second.coordinate;
          for (std::size_t _id = 0; _id < _dir.size(); ++_id) {
            _target_co[_id] += _dir[_id];
          }

          // ravel the target coordinate to index representation
          std::size_t _target_id =
              npp::ravel_index(_target_co, m_boundary_grid_shape);

          // check if target is on dummy boundary
          bool _boundary = false;
          if (this->vertices[_target_id].boundary) {
            for (std::size_t _ib = 0; _ib < m_bcs.size(); ++_ib) {
              if (m_bcs[_ib] == "obc") {
                continue;
              } else if (m_bcs[_ib] == "pbc") {
                if (_target_co[_ib] == 0) {
                  _target_co[_ib] = m_grid_shape[_ib];
                  _boundary = true;
                } else if (_target_co[_ib] == m_boundary_grid_shape[_ib] - 1) {
                  _target_co[_ib] = 1;
                  _boundary = true;
                }
              }
            }
          }

          // re-ravel new index
          _target_id = npp::ravel_index(_target_co, m_boundary_grid_shape);

          // check if parallel edge is already present
          if (this->adjacency_list[_target_id].find(_origin_id) ==
              this->adjacency_list[_target_id].end()) {
            // add edge
            this->addEdge(_edge_index, _origin_id, _target_id);
            this->edges[_edge_index].boundary = _boundary;
            _edge_index++;
            _boundary = false;
          }
        }
      }
    }
  }
};

#endif  // INCLUDE_LATTICEGRAPH_H_
