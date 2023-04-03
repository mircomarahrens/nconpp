#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#define FORCE_IMPORT_ARRAY
#include "xtensor-python/pyarray.hpp"
#include "xtensor-python/pyvectorize.hpp"

#include <complex>
#include <vector>

#include "TensorNetwork.h"

namespace py = pybind11;

template <typename T>
void class_wrapper(py::module &m)
{
	py::class_<TensorNetwork<T>>(m, "TensorNetwork")
		.def(py::init<std::vector<npp::tensor<T>> &, std::vector<std::vector<int>> &>(),
			 py::arg("tensorList"), py::arg("legsList"))
		// .def("contract", &TensorNetwork<T>::contract,
		// 	 py::arg("contractionSequence") = py::none(), py::arg("finalOrder") = py::none())
		.def("connect", &TensorNetwork<T>::connect)
		.def_property_readonly("num_tensors", &TensorNetwork<T>::num_tensors);
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

	class_wrapper<std::complex<double>>(m);
	class_wrapper<double>(m);
}
