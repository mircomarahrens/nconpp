#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "xtensor/xmath.hpp"
#include "xtensor/xarray.hpp"

#define FORCE_IMPORT_ARRAY
#include "xtensor-python/pyarray.hpp"
#include "xtensor-python/pyvectorize.hpp"

#include <complex>
#include <iostream>
#include <numeric>
#include <cmath>

#include "NetworkContractor.h"
#include "NetworkContractor.cpp"

namespace py = pybind11;

// Examples

inline double example1(xt::pyarray<double> &m)
{
    return m(0);
}

inline xt::pyarray<double> example2(xt::pyarray<double> &m)
{
    return m + 2;
}

// Readme Examples

inline double readme_example1(xt::pyarray<double> &m)
{
    auto sines = xt::sin(m);
    return std::accumulate(sines.cbegin(), sines.cend(), 0.0);
}

// Vectorize Examples

inline double scalar_func(double i, double j)
{
    return std::sin(i) + std::cos(j);
}

// Python Module and Docstrings

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
        &NetworkContractor::contract_debug<xt::pyarray<std::complex<double>>>,
        py::arg("containerList"),
        py::arg("legsList"),
        py::arg("contractionSequenceLegs") = py::none(),
        py::arg("finalOrder") = py::none());

    m.def("example1", example1, "Return the first element of an array, of dimension at least one");
    m.def("example2", example2, "Return the the specified array plus 2");

    m.def("readme_example1", readme_example1, "Accumulate the sines of all the values of the specified array");

    m.def("vectorize_example1", xt::pyvectorize(scalar_func), "Add the sine and and cosine of the two specified values");
}
