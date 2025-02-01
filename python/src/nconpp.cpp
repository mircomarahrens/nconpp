// Copyright 2023 Mirco Marahrens

#ifndef NCONPP_PYTHON_SRC_NCONPP_CPP_
#define NCONPP_PYTHON_SRC_NCONPP_CPP_

#include <complex>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Graph.hpp"
#include "LatticeGraph.hpp"
#include "TensorNetwork.hpp"

#define FORCE_IMPORT_ARRAY
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "xtensor-python/pyarray.hpp"
#include "xtensor-python/pytensor.hpp"
#include "xtensor-python/pyvectorize.hpp"

namespace py = pybind11;

/**
 * @brief Trampoline class for Graph
 *
 * @tparam G
 * @tparam V
 * @tparam E
 */
template <class G, class V, class E> class PyGraph_trampoline : public Graph<G, V, E>
{
  public:
    using Graph<G, V, E>::Graph;
};

/**
 * @brief Wrapper function for class Graph
 *
 * @param m
 * @param typestr
 */
void PyGraph_wrapper(py::module &m)
{
    using default_struct = GRAPH_PROPERTIES::default_struct;
    using graph = PyGraph_trampoline<default_struct, default_struct, default_struct>;

    py::class_<default_struct>(m, "default_struct").def(py::init<>());

    py::class_<graph::graph_properties_t, default_struct>(m, "GraphProperties")
        .def(py::init<>())
        .def_readonly("parallel_edges", &graph::graph_properties_t::parallel_edges)
        .def_readonly("directed_edges", &graph::graph_properties_t::directed_edges);
    py::class_<graph::vertex_properties_t, default_struct>(m, "VertexProperties")
        .def(py::init<>())
        .def_readwrite("edge_indices", &graph::vertex_properties_t::edge_indices);
    py::class_<graph::edge_properties_t, default_struct>(m, "EdgeProperties")
        .def(py::init<>())
        .def_readonly("src", &graph::edge_properties_t::src)
        .def_readonly("dest", &graph::edge_properties_t::dest);

    py::class_<graph>(m, "Graph", py::buffer_protocol(), py::dynamic_attr())
        .def(py::init<>())
        .def(py::init<std::size_t, bool, bool>(), py::arg("nodes"), py::arg("parallel_edges") = true,
             py::arg("directed_edges") = false)
        .def("get_vertices", &graph::getVertices)
        .def("add_vertex", &graph::addVertex, py::arg("vertex") = py::none())
        .def("remove_vertex", &graph::removeVertex, py::arg("vertex"))
        .def("clear_vertex", &graph::clearVertex, py::arg("vertex"))
        .def("add_edge", &graph::addEdge, py::arg("id"), py::arg("src"), py::arg("dest"))
        .def("get_edges", &graph::getEdges)
        .def("remove_edge", &graph::removeEdge)
        .def_property_readonly("num_edges", &graph::NumEdges)
        .def_property_readonly("num_vertices", &graph::NumVertices)
        .def_readonly("adjacency_list", &graph::adjacency_list)
        .def_readonly("vertices", &graph::vertices)
        .def_readonly("edges", &graph::edges);
}

/**
 * @brief Wrapper function for class LatticeGraph
 *
 * @param m
 * @param typestr
 */
void PyLatticeGraph_wrapper(py::module &m)
{
    using lattice = LatticeGraph;
    using lg = GRAPH_PROPERTIES::lattice_graph_properties;
    using lv = GRAPH_PROPERTIES::lattice_vertex_properties;
    using le = GRAPH_PROPERTIES::lattice_edge_properties;
    using graph = PyGraph_trampoline<lg, lv, le>;

    py::class_<graph::graph_properties_t>(m, "LatticeGraphProperties", py::module_local())
        .def(py::init<>())
        .def_readonly("parallel_edges", &graph::graph_properties_t::parallel_edges)
        .def_readonly("directed_edges", &graph::graph_properties_t::directed_edges)
        .def_readwrite("name", &graph::graph_properties_t::name);
    py::class_<graph::vertex_properties_t>(m, "LatticeVertexProperties", py::module_local())
        .def(py::init<>())
        .def_readwrite("edge_indices", &graph::vertex_properties_t::edge_indices)
        .def_readwrite("coordinate", &graph::vertex_properties_t::coordinate)
        .def_readwrite("boundary", &graph::vertex_properties_t::boundary);
    py::class_<graph::edge_properties_t>(m, "LatticeEdgeProperties")
        .def(py::init<>())
        .def_readonly("src", &graph::edge_properties_t::src)
        .def_readonly("dest", &graph::edge_properties_t::dest)
        .def_readonly("boundary", &graph::edge_properties_t::boundary);

    py::class_<lattice>(m, "LatticeGraph", py::buffer_protocol(), py::dynamic_attr(), py::module_local())
        .def(py::init<std::string &, std::vector<std::size_t> &, lattice::directions_type &,
                      std::vector<std::string> &>(),
             py::arg("name") = "honeycomb", py::arg("shape") = std::vector<std::size_t>({4, 4}),
             py::arg("directions") =
                 LatticeGraph::directions_type({{{0, -1}, {0, +1}, {+1, 0}}, {{-1, 0}, {0, -1}, {0, +1}}}),
             py::arg("boundary_conditions") = std::vector<std::string>({"pbc", "pbc"}))
        .def("getGridShape", &lattice::getGridShape)
        .def("getBoundaryGridShape", &lattice::getBoundaryGridShape)
        .def("getDirections", &lattice::getDirections)
        .def("get_vertices", &lattice::getVertices)
        .def("add_vertex", &lattice::addVertex, py::arg("vertex") = py::none())
        .def("remove_vertex", &lattice::removeVertex, py::arg("vertex"))
        .def("clear_vertex", &lattice::clearVertex, py::arg("vertex"))
        .def("add_edge", &lattice::addEdge, py::arg("id"), py::arg("src"), py::arg("dest"))
        .def("get_edges", &lattice::getEdges)
        .def("remove_edge", &lattice::removeEdge)
        .def_property_readonly("num_edges", &lattice::NumEdges)
        .def_property_readonly("num_vertices", &lattice::NumVertices)
        .def_readonly("adjacency_list", &lattice::adjacency_list)
        .def_readonly("vertices", &lattice::vertices)
        .def_readonly("edges", &lattice::edges);
}

/**
 * @brief Trampoline class for TensorNetwork
 *
 * @tparam T
 */
template <typename T> class PyTensorNetwork_trampoline : public TensorNetwork<T>
{
  public:
    PyTensorNetwork_trampoline(const std::vector<npp::tensor_type<T>> &tensorList,
                               const std::vector<std::vector<int>> &subscriptVectorList)
        : TensorNetwork<T>(tensorList, subscriptVectorList)
    {
    }

    PyTensorNetwork_trampoline(std::vector<npp::tensor_type<T>> &&tensorList,
                               std::vector<std::vector<int>> &&subscriptVectorList)
        : TensorNetwork<T>(std::move(tensorList), std::move(subscriptVectorList))
    {
    }

    void contract_wrapper(std::optional<std::vector<int>> opt_contractionSequence = std::nullopt,
                          std::optional<std::vector<int>> opt_finalOrder = std::nullopt)
    {
        std::vector<int> contractionSequence = {};
        if (opt_contractionSequence.has_value())
        {
            contractionSequence = opt_contractionSequence.value();
        }

        std::vector<int> finalOrder = {};
        if (opt_finalOrder.has_value())
        {
            finalOrder = opt_finalOrder.value();
        }

        TensorNetwork<T>::contract(contractionSequence, finalOrder);
    }
};

class PyTensorNetworkGraphProperties_trampoline : public GRAPH_PROPERTIES::tensornetwork_graph_properties
{
};
template <typename T>
class PyTensorNetworkVertexProperties_trampoline : public GRAPH_PROPERTIES::tensornetwork_vertex_properties<T>
{
};
template <typename T>
class PyTensorNetworkEdgeProperties_trampoline : public GRAPH_PROPERTIES::tensornetwork_edge_properties<T>
{
};

/**
 * @brief Wrapper function for trampoline class PyTensorNetwork_trampoline
 *
 * @param m
 */
void PyTensorNetwork_wrapper(py::module &m)
{
    using tensornetwork = PyTensorNetwork_trampoline<std::complex<double>>;
    using tng = PyTensorNetworkGraphProperties_trampoline;
    using tnv = PyTensorNetworkVertexProperties_trampoline<std::complex<double>>;
    using tne = PyTensorNetworkEdgeProperties_trampoline<std::complex<double>>;
    using graph = PyGraph_trampoline<tng, tnv, tne>;

    py::class_<graph::graph_properties_t>(m, "TensorNetworkGraphProperties")
        .def(py::init<>())
        .def_readonly("parallel_edges", &graph::graph_properties_t::parallel_edges)
        .def_readonly("directed_edges", &graph::graph_properties_t::directed_edges);
    py::class_<graph::vertex_properties_t>(m, "TensorNetworVertexProperties")
        .def(py::init<>())
        .def_readwrite("edge_indices", &graph::vertex_properties_t::edge_indices)
        .def_readwrite("cartesian_coordinates", &graph::vertex_properties_t::cartesian_coordinates)
        .def_readwrite("legs", &graph::vertex_properties_t::legs)
        .def_readwrite("tensor", &graph::vertex_properties_t::tensor);
    py::class_<graph::edge_properties_t>(m, "TensorNetworkEdgeProperties")
        .def(py::init<>())
        .def_readonly("src", &graph::edge_properties_t::src)
        .def_readonly("dest", &graph::edge_properties_t::dest)
        .def_readwrite("singular_values", &graph::edge_properties_t::singular_values);

    py::class_<tensornetwork>(m, "TensorNetwork", py::buffer_protocol(), py::dynamic_attr(), py::module_local())
        .def(py::init<std::vector<npp::tensor_type<std::complex<double>>> &, std::vector<std::vector<int>> &>(),
             py::arg("tensorList"), py::arg("legsList"))
        .def(py::init<std::vector<npp::tensor_type<std::complex<double>>> &&, std::vector<std::vector<int>> &&>(),
             py::arg("tensorList"), py::arg("legsList"))
        .def("contract", &tensornetwork::contract_wrapper, py::arg("contractionSequence") = py::none(),
             py::arg("finalOrder") = py::none())
        .def("connect", &tensornetwork::connect)
        .def_property_readonly("num_tensors", &tensornetwork::NumTensors)
        .def("get_vertices", &tensornetwork::getVertices)
        .def("add_vertex", &tensornetwork::addVertex, py::arg("vertex") = py::none())
        .def("remove_vertex", &tensornetwork::removeVertex, py::arg("vertex"))
        .def("clear_vertex", &tensornetwork::clearVertex, py::arg("vertex"))
        .def("add_edge", &tensornetwork::addEdge, py::arg("id"), py::arg("src"), py::arg("dest"))
        .def("get_edges", &tensornetwork::getEdges)
        .def("remove_edge", &tensornetwork::removeEdge)
        .def_property_readonly("num_edges", &tensornetwork::NumEdges)
        .def_property_readonly("num_vertices", &tensornetwork::NumVertices)
        .def_readonly("adjacency_list", &tensornetwork::adjacency_list)
        .def_readonly("vertices", &tensornetwork::vertices)
        .def_readonly("edges", &tensornetwork::edges);
}

PYBIND11_MODULE(_nconpp, m)
{
    xt::import_numpy();

    m.doc() = R"pbdoc(
        Library for Tensor Network algorithms.

        @params:
         tensorList:
             list of containers, i.e. tensors to contract
         legsList:
             Nomenclature of the legs of the tensors in tensorList:
                 - the legs are named by integers
                 - contractable legs have the same positive integer as name,
                   hence occuring in pairs
                 - legs with negative integers won't be contracted, so called
                   dangling legs
         contractionSequence (optional):
             order in which the tensors shall be contracted
         finalOrder (optional):
             Permutation of the legs of the final tensor.

        @return:
         the final contracted container
    )pbdoc";

    PyGraph_wrapper(m);
    PyLatticeGraph_wrapper(m);
    PyTensorNetwork_wrapper(m);
}

/**
 *     """A class for a graph representation of a lattice.

    Note:
        We construct lattices as graphs to achieve a solid framework where each
        constituent has a fixed integer value as identifier. We enumerate sites,
        bonds, boundary conditions (lattice) and setting up vertices and edges
        (graph) corresponding to them. For handling of the lattice boundaries we
        introduce boundary points.

        A 4x4 honeycomb lattice has in total 6x6=36 sites, 4x4=16 bulk points
        and 20 boundary points. In cartesian coordinates the graph has the
        following representation:

            (0, 5)  (1, 5)  (2, 5)  (3, 5)  (4, 5)  (5, 5)

            (0, 4)  (1, 4)  (2, 4)  (3, 3)  (4, 4)  (5, 4)

            (0, 3)  (1, 3)  (2, 3)  (3, 3)  (4, 3)  (5, 3)

            (0, 2)  (1, 2)  (2, 2)  (3, 2)  (4, 2)  (5, 2)

            (0, 1)  (1, 1)  (2, 1)  (3, 1)  (4, 1)  (5, 1)
        y
        ^   (0, 0)  (1, 0)  (2, 0)  (3, 0)  (4, 0)  (5, 0)
        |
        +-->x

        The default lattice has periodic boundary conditions in both spatial
        directions, hence are the edges of this graph as follows:

                    (1, 1)--(2, 1)  (3, 1)--(4, 1)
                      |       |       |       |
            (4, 4)--(1, 4)  (2, 4)--(3, 3)  (4, 4)--(1, 4)
                      |       |       |       |
                    (1, 3)--(2, 3)  (3, 3)--(4, 3)
                      |       |       |       |
            (4, 2)--(1, 2)  (2, 2)--(3, 2)  (4, 2)--(1, 2)
                      |       |       |       |
                    (1, 1)--(2, 1)  (3, 1)--(4, 1)
                      |       |       |       |
                    (1, 4)  (2, 4)--(3, 3)  (4, 4)

        whereas for open boundary conditions in both spatial directions
        the coordinates of the lattice sites are as

                    (1, 5)  (2, 5)  (3, 5)  (4, 5)
                      |       |       |       |
            (4, 4)--(1, 4)  (2, 4)--(3, 3)  (4, 4)--(1, 4)
                      |       |       |       |
                    (1, 3)--(2, 3)  (3, 3)--(4, 3)
                      |       |       |       |
            (4, 2)--(1, 2)  (2, 2)--(3, 2)  (4, 2)--(1, 2)
                      |       |       |       |
                    (1, 1)--(2, 1)  (3, 1)--(4, 1)
                      |       |       |       |
                    (1, 0)  (2, 0)  (3, 0)  (4, 0)

        and for closed boundary conditions we have

                    (1, 4)  (2, 4)--(3, 3)  (4, 4)
                      |       |       |       |
                    (1, 3)--(2, 3)  (3, 3)--(4, 3)
                      |       |       |       |
                    (1, 2)  (2, 2)--(3, 2)  (4, 2)
                      |       |       |       |
                    (1, 1)--(2, 1)  (3, 1)--(4, 1)

        The last lattice graph represents the core of the lattice.

    Args:
        Graph (type): optional, default: None. Parent class of LatticeGraph.
        name (str): see Attributes for a full description
        grid (tuple): see Attributes for a full description
        accu (list): accumulators, see Attributes for a full description
        bcs (tuple): boundary_conditions, see Attributes for a full description

    Attributes:
        name (str): a free name for the lattice, e.g. "honeycomb", "square",
            etc.

        grid (tuple): the number of sites spatial distributed, e.g. for d=2
            we have (Nx, Ny), where Nx, Ny are integers representing the
            maximal number of sites for the x- and y-direction.

        sites (tuple): the total number of sites.

        accumulators (list): a list of tuples. The length of this list is the
            number of sites in a "cell", i.e. a unit cell. Each entry of this
            list is again a tuple consisting of multiple integers representing
            additive coordinates. If a additive coordinate is added to a
            origin site in the lattice one is getting a target site. A pair
            (origin, target) is forming a bond/edge in the lattice/graph.

        boundary_conditions (tuple): the boundary condition per spatial
            direction, i.e. for d=2 we can have different conditions for the
            x- and y-direction of the lattice. We currently support the
            following boundary conditions:

                - "pbc": periodic boundary conditions
                - "obc": open boundary conditions
                - "cbc": closed boundary conditions

                tbd:

                - "ibc": infinite boundary conditions

        edges (dict of int : any): key: id, value: pairs of (origin, target) resulted from
            the "unit cell".

        vertices (dict of int: any): key: id, value: vertex properties

    Methods:
        - _init_lattice: see docstrings in methods for details.
        - unravel: see docstrings in methods for details.
        - ravel: see docstrings in methods for details.
    """
 *
 */

#endif // NCONPP_PYTHON_SRC_NCONPP_CPP_
