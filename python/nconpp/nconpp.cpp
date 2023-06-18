#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#define FORCE_IMPORT_ARRAY
#include "xtensor-python/pyarray.hpp"
#include "xtensor-python/pyvectorize.hpp"

#include <complex>
#include <vector>
#include <string>
#include <optional>

#include "TensorNetwork.h"

namespace py = pybind11;

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
					const std::vector<std::vector<int>> &subscriptVectorList) : TensorNetwork<T>(tensorList, subscriptVectorList) {};

	PyTensorNetwork_trampoline(std::vector<npp::tensor_type<T>> &&tensorList,
					std::vector<std::vector<int>> &&subscriptVectorList) : TensorNetwork<T>(std::move(tensorList), std::move(subscriptVectorList)) {};

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

	py::class_<PyTensorNetwork_trampoline<T>>(m, pyclass_name.c_str(), py::buffer_protocol(), py::dynamic_attr())
		.def(py::init<std::vector<npp::tensor_type<T>> &, std::vector<std::vector<int>> &>(),
			 py::arg("tensorList"), py::arg("legsList"))
		.def(py::init<std::vector<npp::tensor_type<T>> &&, std::vector<std::vector<int>> &&>(),
			 py::arg("tensorList"), py::arg("legsList"))
		.def("contract", &PyTensorNetwork_trampoline<T>::contract_wrapper,
			 py::arg("contractionSequence") = py::none(), py::arg("finalOrder") = py::none())
		.def("connect", &PyTensorNetwork_trampoline<T>::connect)
		.def_property_readonly("num_tensors", &PyTensorNetwork_trampoline<T>::NumTensors);
}

PYBIND11_MODULE(_nconpp, m)
{
	xt::import_numpy();

	m.doc() = R"pbdoc(
        Library for Tensor Network contraction.

	    Contracts multiple tensors to one tensor, if possible.

	    @params:
	     containerList:
	         list of containers, i.e. tensors to contract
	     legsList:
	         Nomenclature of the legs of the tensors in tensorList:
	             - the legs are named by integers
	             - contractable legs have the same positive integer as name,
	               hence occuring in pairs
	             - legs with negative integers won't be contracted, so called
	               dangling legs
	     contractionSequenceLegs (optional):
	         order in which the tensors shall be contracted
	     finalOrder (optional):
	         Permutation of the legs of the final tensor.

	    @return:
	     the final contracted container
    )pbdoc";

	PyTensorNetwork_wrapper<std::complex<double>>(m);
	// TODO define derived (?) class for Pybind11 with different data types (aka dtype)
	// class_wrapper<double>(m);
}
