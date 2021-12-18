#include <complex>
#include <pybind11/pybind11.h>
#define FORCE_IMPORT_ARRAY
#include <xtensor-python/pyarray.hpp>
#include <xtensor-python/pytensor.hpp>

#include "NetworkContractor.h"
#include "NetworkContractor.cpp"

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

    // now the contractor: NetworkContractor::contract();
    // it builds but when loading the module in python:
    // Python 3.10.0 (tags/v3.10.0:b494f59, Oct  4 2021, 19:00:18) [MSC v.1929 64 bit (AMD64)] on win32
    // Type "help", "copyright", "credits" or "license" for more information.
    //     >> > import nconpp
    //     Traceback(most recent call last) :
    //     File "<stdin>", line 1, in <module>
    //     File "C:\Users\MircoMarahrens\source\repos\nconpp\nconpp-venv\lib\site-packages\nconpp\__init__.py", line 1, in <module>
    //     from._nconpp import ncadd, contract
    //     ImportError : DLL load failed while importing _nconpp : The specified module could not be found.
    //m.def("contract", &NetworkContractor::contract<std::complex<double>>);
    xt::import_numpy();

    //m.def("contract", &compute);
    m.def("contract", &NetworkContractor::contract<xt::pyarray<std::complex<double>, xt::layout_type::dynamic>>);
}

//#include <pybind11/pybind11.h>
//#define FORCE_IMPORT_ARRAY
//#include <xtensor-python/pyvectorize.hpp>
//#include <numeric>
//#include <cmath>
//
//namespace py = pybind11;
//
//double scalar_func(double i, double j)
//{
//    return std::sin(i) - std::cos(j);
//}
//
//PYBIND11_MODULE(_nconpp, m)
//{
//    m.doc() = R"pbdoc(
//    contracts multiple tensors to one tensor, if possible.
//    
//    @params:
//     tensorList:
//         list of tensors to contract
//     legsList:
//         Nomenclature of the legs of the tensors in tensorList:
//             - the legs are named by integers
//             - contractable legs have the same positive integer as name,
//               hence occuring in pairs
//             - legs with negative integers won't be contracted, so called
//               dangling legs
//     contractionSequenceTensors:
//         order in which the tensors shall be contracted
//     contractionSequenceLegs:
//         order in which the legs shall be contracted
//     finalOrder:
//         Permutation of the legs of the final tensor.
//    
//    @return:
//        the final contracted tensor
//    )pbdoc";
//
//    xt::import_numpy();
//
//    m.def("vectorized_func", xt::pyvectorize(scalar_func), "");
//}

