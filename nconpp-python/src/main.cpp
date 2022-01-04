#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "xtensor/xmath.hpp"
#include "xtensor/xarray.hpp"

#define FORCE_IMPORT_ARRAY
#include "xtensor-python/pyarray.hpp"
#include "xtensor-python/pyvectorize.hpp"

#include <complex>
#include <cmath>
#include <iostream>
#include <numeric>
#include <optional>

#include "NetworkContractor.h"
#include "NetworkContractor.cpp"

namespace py = pybind11;

template <class T>
T contract_wrapper(std::vector<T>& containerList,
	std::vector<std::vector<int>> legsList,
	std::optional<std::vector<int>> contractionSequenceLegs,
	std::optional<std::vector<int>> finalOrder)
{
	// if empty fill with defaults
	std::vector<int> _contractionSequenceLegs = {};
	if (!contractionSequenceLegs.has_value()) {
		set<int> conSet = Container::allUniqueIntegersSorted(legsList);
		_contractionSequenceLegs.assign(conSet.begin(), conSet.end());
		Container::removeNegatives(_contractionSequenceLegs);
	}
	else
	{
		_contractionSequenceLegs = contractionSequenceLegs.value();
	}
	std::vector<int> _finalOrder = {};
	if (!finalOrder.has_value())
	{
		set<int> conSet = Container::allUniqueIntegersSorted(legsList);
		_finalOrder.assign(conSet.begin(), conSet.end());
		Container::removePositives(_finalOrder);
		std::reverse(_finalOrder.begin(), _finalOrder.end());
	}
	else
	{
		_finalOrder = finalOrder.value();
	}

	return NetworkContractor::contract(
		containerList,
		legsList,
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
	     contractionSequenceTensors (optional):
	         order in which the tensors shall be contracted
	     finalOrder (optional):
	         Permutation of the legs of the final tensor.
	
	    @return:
	     the final contracted container
    )pbdoc";

    //m.def("contract", &NetworkContractor::contract<xt::pyarray<std::complex<double>, xt::layout_type::dynamic>>);

    //m.def("add", &NetworkContractor::add);

    //m.def("add", &NetworkContractor::add<std::complex<double>>);
    //m.def("add2", &NetworkContractor::add<xt::pyarray<std::complex<double>>>);

    m.def("contract",
        &contract_wrapper<xt::pyarray<std::complex<double>>>,
        py::arg("containerList"),
        py::arg("legsList"),
        py::arg("contractionSequenceLegs") = py::none(),
        py::arg("finalOrder") = py::none());
}
