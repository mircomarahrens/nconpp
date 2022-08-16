#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#define FORCE_IMPORT_ARRAY
#include "xtensor-python/pyarray.hpp"
#include "xtensor-python/pyvectorize.hpp"

#include <complex>
#include <optional>

#include "../lib/Nconpp.h"
#include "../lib/Nconpp.cpp"

namespace py = pybind11;

template <class T>
T contract_wrapper(std::vector<T>& containerList,
	std::vector<std::vector<int>> legsList,
	bool skipValidation,
	std::optional<std::vector<int>> contractionSequenceLegs,
	std::optional<std::vector<int>> finalOrder)
{
	std::vector<int> _contractionSequenceLegs = {};
	if (contractionSequenceLegs.has_value()) {
		_contractionSequenceLegs = contractionSequenceLegs.value();
	}
	std::vector<int> _finalOrder = {};
	if (finalOrder.has_value())
	{
		_finalOrder = finalOrder.value();
	}

	return Nconpp::contract(
		containerList,
		legsList,
		skipValidation,
		_contractionSequenceLegs,
		_finalOrder);
};

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

    m.def("contract",
        &contract_wrapper<xt::pyarray<std::complex<double>>>,
        py::arg("containerList"),
        py::arg("legsList"),
        py::arg("skipValidation"),
        py::arg("contractionSequenceLegs") = py::none(),
        py::arg("finalOrder") = py::none());
}
