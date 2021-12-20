#include <complex>
#include <pybind11/pybind11.h>
#define FORCE_IMPORT_ARRAY
#include <xtensor-python/pyarray.hpp>

#include "binding.h"

namespace py = pybind11;

template <class T, xt::layout_type L>
struct is_array<xt::pyarray<T, L>> : std::true_type {};

PYBIND11_MODULE(_nconpp, m) {
    m.doc() = R"pbdoc(
    contracts multiple tensors to one tensor, if possible.
    
    @params:
     tensorList:
         list of tensors to contract
     legsList:
         Nomenclature of the legs of the tensors in tensorList:
             - the legs are named by integers
             - contractable legs have the same positive integer as name,
               hence occuring in pairs
             - legs with negative integers won't be contracted, so called
               dangling legs
     contractionSequenceLegs:
         order in which the legs shall be contracted
     finalOrder:
         Permutation of the legs of the final tensor.
    
    @return:
        the final contracted tensor
    )pbdoc";

    xt::import_numpy();

    //m.def("contract", &PyBinding::contractWrapper<xt::pyarray<std::complex<double>, xt::layout_type::dynamic>>);
    //m.def("contract", &PyBinding::contractWrapper<xt::xexpression<xt::pyarray<std::complex<double>, xt::layout_type::dynamic>>>);
    //m.def("contract", &NetworkContractor::contract<xt::pyarray<int, xt::layout_type::dynamic>>);
    m.def("contract", &NetworkContractor::contract<xt::pyarray<std::complex<double>, xt::layout_type::dynamic>>);
}
