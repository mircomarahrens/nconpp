#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#define FORCE_IMPORT_ARRAY
#include "xtensor-python/pyarray.hpp"
#include "xtensor-python/pytensor.hpp"
#include "xtensor-python/pyvectorize.hpp"

#include <complex>
#include <vector>
#include <string>
#include <unordered_map>
#include <optional>

#include "Graph.h"
#include "LatticeGraph.h"
#include "TensorNetwork.h"

namespace py = pybind11;


/**
 * @brief Trampoline class for Graph
 *
 * @tparam G
 * @tparam V
 * @tparam E
 */
template <class G, class V, class E>
class PyGraph_trampoline : public Graph<G, V, E>
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
        .def(py::init<std::size_t, bool, bool>(), py::arg("nodes"), py::arg("parallel_edges") = true, py::arg("directed_edges") = false)
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
 * @brief Trampoline class for Graph
 *
 */
// class PyLatticeGraph_trampoline : public LatticeGraph
// {
// public:
//     using LatticeGraph::LatticeGraph;
// };

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

    py::class_<lg>(m, "lattice_graph_properties")
        .def(py::init<>())
        .def_readwrite("name", &lg::name);
    py::class_<lv>(m, "lattice_vertex_properties")
        .def(py::init<>())
        .def_readwrite("coordinate", &lv::coordinate)
        .def_readwrite("boundary", &lv::boundary);
    py::class_<le>(m, "lattice_edge_properties")
        .def(py::init<>());

    py::class_<graph::graph_properties_t, lg>(m, "LatticeGraphProperties", py::module_local())
        .def(py::init<>())
        .def_readonly("parallel_edges", &graph::graph_properties_t::parallel_edges)
        .def_readonly("directed_edges", &graph::graph_properties_t::directed_edges);
    py::class_<graph::vertex_properties_t, lv>(m, "LatticeVertexProperties", py::module_local())
        .def(py::init<>())
        .def_readwrite("edge_indices", &graph::vertex_properties_t::edge_indices);
    py::class_<graph::edge_properties_t, le>(m, "LatticeEdgeProperties")
        .def(py::init<>())
        .def_readonly("src", &graph::edge_properties_t::src)
        .def_readonly("dest", &graph::edge_properties_t::dest);

    // py::class_<graph>(m, "graph_lattice", py::buffer_protocol(), py::dynamic_attr(), py::module_local())
    //     .def(py::init<>())
    //     .def(py::init<std::size_t, bool, bool>(), py::arg("nodes"), py::arg("parallel_edges") = true, py::arg("directed_edges") = false)
    //     .def("get_vertices", &graph::getVertices)
    //     .def("add_vertex", &graph::addVertex, py::arg("vertex") = py::none())
    //     .def("remove_vertex", &graph::removeVertex, py::arg("vertex"))
    //     .def("clear_vertex", &graph::clearVertex, py::arg("vertex"))
    //     .def("add_edge", &graph::addEdge, py::arg("id"), py::arg("src"), py::arg("dest"))
    //     .def("get_edges", &graph::getEdges)
    //     .def("remove_edge", &graph::removeEdge)
    //     .def_property_readonly("num_edges", &graph::NumEdges)
    //     .def_property_readonly("num_vertices", &graph::NumVertices)
    //     .def_readonly("adjacency_list", &graph::adjacency_list)
    //     .def_readonly("vertices", &graph::vertices)
    //     .def_readonly("edges", &graph::edges);

    py::class_<lattice>(m, "LatticeGraph", py::buffer_protocol(), py::dynamic_attr(), py::module_local())
        .def(py::init<std::string&, std::vector<std::size_t>&, lattice::directions_type&, std::vector<std::string>&>(), 
        py::arg("name") = "honeycomb",
        py::arg("shape") = std::vector<std::size_t>({4, 4}),
        py::arg("directions") = LatticeGraph::directions_type({{{0, -1}, {0, +1}, {+1, 0}}, {{-1, 0}, {0, -1}, {0, +1}}}),
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
        .def_readonly("edges", &lattice::edges);;
}

/**
 * @brief Trampoline class for TensorNetwork
 *
 * @tparam T
 */
template <typename T>
class PyTensorNetwork_trampoline : public TensorNetwork<T>
{
public:
    PyTensorNetwork_trampoline(const std::vector<npp::tensor_type<T>> &tensorList,
                               const std::vector<std::vector<int>> &subscriptVectorList) : TensorNetwork<T>(tensorList, subscriptVectorList){};

    PyTensorNetwork_trampoline(std::vector<npp::tensor_type<T>> &&tensorList,
                               std::vector<std::vector<int>> &&subscriptVectorList) : TensorNetwork<T>(std::move(tensorList), std::move(subscriptVectorList)){};

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

/**
 * @brief Wrapper function for trampoline class PyTensorNetwork_trampoline
 *
 * @tparam T
 * @param m
 * @param typestr
 */
template <typename T>
void PyTensorNetwork_wrapper(py::module &m, const std::string &typestr = std::string())
{
    std::string pyclass_name = std::string("TensorNetwork");
    if (!typestr.empty())
    {
        pyclass_name += typestr;
    }

    // py::class_<GRAPH_PROPERTIES::custom_graph_properties>(m, "custom_graph_properties")
    // 	.def(py::init<>());
    // py::class_<GRAPH_PROPERTIES::custom_vertex_properties<T>>(m, "custom_vertex_properties")
    // 	.def(py::init<>())
    // 	.def_readwrite("cartesian_coordinates", &GRAPH_PROPERTIES::custom_vertex_properties<T>::cartesian_coordinates)
    // 	.def_readwrite("legs", &GRAPH_PROPERTIES::custom_vertex_properties<T>::legs)
    // 	.def_readwrite("tensor", &GRAPH_PROPERTIES::custom_vertex_properties<T>::tensor);
    // py::class_<GRAPH_PROPERTIES::custom_edge_properties<T>>(m, "custom_edge_properties")
    // 	.def(py::init<>())
    // 	.def_readwrite("singular_values", &GRAPH_PROPERTIES::custom_edge_properties<T>::singular_values);

    // PyGraph_properties_wrapper<GRAPH_PROPERTIES::custom_graph_properties, GRAPH_PROPERTIES::custom_vertex_properties<T>, GRAPH_PROPERTIES::custom_edge_properties<T>>(m);

    // py::class_<PyTensorNetwork_trampoline<T>, Graph<GRAPH_PROPERTIES::custom_graph_properties, GRAPH_PROPERTIES::custom_vertex_properties<T>, GRAPH_PROPERTIES::custom_edge_properties<T>>>(m, pyclass_name.c_str(), py::buffer_protocol(), py::dynamic_attr())
    // 	.def(py::init<std::vector<npp::tensor_type<T>> &, std::vector<std::vector<int>> &>(),
    // 		 py::arg("tensorList"), py::arg("legsList"))
    // 	.def(py::init<std::vector<npp::tensor_type<T>> &&, std::vector<std::vector<int>> &&>(),
    // 		 py::arg("tensorList"), py::arg("legsList"))
    // 	.def("contract", &PyTensorNetwork_trampoline<T>::contract_wrapper,
    // 		 py::arg("contractionSequence") = py::none(), py::arg("finalOrder") = py::none())
    // 	.def("connect", &PyTensorNetwork_trampoline<T>::connect)
    // 	.def_property_readonly("num_tensors", &PyTensorNetwork_trampoline<T>::NumTensors);
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
}
