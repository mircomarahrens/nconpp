// Copyright 2023 Mirco Marahrens

#include <complex>
#include <optional>
#include <string>
#include <type_traits>
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
template <class G, class V, class E> class PyGraph_trampoline : public Graph<G, V, E> {
  public:
    using Graph<G, V, E>::Graph;
};

/**
 * @brief Wrapper function for class Graph
 *
 * @param m
 * @param typestr
 */
void PyGraph_wrapper(py::module &m) {
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

    py::class_<graph>(m, "Graph", py::dynamic_attr())
        .def(py::init<>())
        .def(py::init<std::size_t, bool, bool>(), py::arg("nodes"),
             py::arg("parallel_edges") = true, py::arg("directed_edges") = false)
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
void PyLatticeGraph_wrapper(py::module &m) {
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

    py::class_<lattice>(m, "LatticeGraph", py::dynamic_attr(), py::module_local())
        .def(py::init<std::string &, std::vector<std::size_t> &, lattice::directions_type &,
                      std::vector<std::string> &>(),
             py::arg("name") = "honeycomb", py::arg("shape") = std::vector<std::size_t>({4, 4}),
             py::arg("directions") = LatticeGraph::directions_type(
                 {{{0, -1}, {0, +1}, {+1, 0}}, {{-1, 0}, {0, -1}, {0, +1}}}),
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
        .def_readonly("edges", &lattice::edges)
        .def("removeEdgelessBoundaryVertices", &lattice::removeEdgelessBoundaryVertices);
}

/**
 * @brief Trampoline class for TensorNetwork
 *
 * @tparam T
 */
template <typename T> class PyTensorNetwork_trampoline : public TensorNetwork<T> {
  public:
    template <typename TensorList, typename SubscriptVectorList,
              typename = std::enable_if_t<
                  std::is_same_v<std::decay_t<TensorList>, std::vector<npp::tensor_type<T>>> &&
                  std::is_same_v<std::decay_t<SubscriptVectorList>, std::vector<std::vector<int>>>>>
    PyTensorNetwork_trampoline(TensorList &&tensorList, SubscriptVectorList &&subscriptVectorList)
        : TensorNetwork<T>(std::forward<TensorList>(tensorList),
                           std::forward<SubscriptVectorList>(subscriptVectorList)) {}

    void contract_wrapper(std::optional<std::vector<int>> opt_contractionSequence = std::nullopt,
                          std::optional<std::vector<int>> opt_finalOrder = std::nullopt) {
        TensorNetwork<T>::contract(opt_contractionSequence.value_or(std::vector<int>{}),
                                   opt_finalOrder.value_or(std::vector<int>{}));
    }
};

/**
 * @brief Wrapper function for TensorNetwork
 *
 * @param m
 */
void PyTensorNetwork_wrapper(py::module &m) {
    using tensornetwork = PyTensorNetwork_trampoline<std::complex<double>>;
    using tng = GRAPH_PROPERTIES::tensornetwork_graph_properties;
    using tnv = GRAPH_PROPERTIES::tensornetwork_vertex_properties<std::complex<double>>;
    using tne = GRAPH_PROPERTIES::tensornetwork_edge_properties<std::complex<double>>;
    using graph = PyGraph_trampoline<tng, tnv, tne>;

    py::class_<graph::graph_properties_t>(m, "TensorNetworkGraphProperties")
        .def(py::init<>())
        .def_readonly("parallel_edges", &graph::graph_properties_t::parallel_edges)
        .def_readonly("directed_edges", &graph::graph_properties_t::directed_edges);
    py::class_<graph::vertex_properties_t>(m, "TensorNetworkVertexProperties")
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

    py::class_<tensornetwork>(m, "TensorNetwork", py::buffer_protocol(), py::dynamic_attr(),
                              py::module_local())
        .def(py::init<std::vector<npp::tensor_type<std::complex<double>>> &,
                      std::vector<std::vector<int>> &>(),
             py::arg("tensorList"), py::arg("legsList"))
        .def(py::init<std::vector<npp::tensor_type<std::complex<double>>> &&,
                      std::vector<std::vector<int>> &&>(),
             py::arg("tensorList"), py::arg("legsList"))
        .def("contract", &tensornetwork::contract_wrapper,
             py::arg("contractionSequence") = py::none(), py::arg("finalOrder") = py::none())
        .def("connect", &tensornetwork::connect)
        .def_property_readonly("num_tensors", &tensornetwork::NumTensors)
        .def("get_vertices", &tensornetwork::getVertices)
        .def("add_vertex", &tensornetwork::addVertex, py::arg("vertex") = py::none())
        .def("remove_vertex", &tensornetwork::removeVertex, py::arg("vertex"))
        .def("clear_vertex", &tensornetwork::clearVertex, py::arg("vertex"))
        .def("add_edge", &tensornetwork::addEdge, py::arg("id"), py::arg("src"), py::arg("dest"))
        .def("get_edges", &tensornetwork::getEdges)
        .def("remove_edge", &tensornetwork::removeEdge)
        .def("get_legs", &tensornetwork::getLegs)
        .def_property_readonly("num_edges", &tensornetwork::NumEdges)
        .def_property_readonly("num_vertices", &tensornetwork::NumVertices)
        .def_readonly("adjacency_list", &tensornetwork::adjacency_list)
        .def_readonly("vertices", &tensornetwork::vertices)
        .def_readonly("edges", &tensornetwork::edges);
}

PYBIND11_MODULE(_nconpp, m) {
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
