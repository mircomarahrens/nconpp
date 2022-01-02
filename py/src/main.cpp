#include <complex>
#include <pybind11/pybind11.h>
#define FORCE_IMPORT_ARRAY
#include <xtensor-python/pyarray.hpp>

#include "NetworkContractor.h"
#include "NetworkContractor.cpp"

namespace py = pybind11;

template <class T, xt::layout_type L>
struct is_array<xt::pyarray<T, L>> : std::true_type
{};

class WithConstraints : public NetworkContractor
{
public:
	// https://xtensor.readthedocs.io/en/latest/bindings.html
	template <class C>
	struct is_tensor : std::false_type {};

	template <class T, std::size_t N, xt::layout_type L, class Tag>
	struct is_tensor<xt::xtensor<T, N, L, Tag>> : std::true_type {};

	template <class C>
	struct is_array : std::false_type {};

	template <class T, xt::layout_type L>
	struct is_array<xt::xarray<T, L>> : std::true_type {};

	template <class T>
	struct is_container : xtl::disjunction<is_tensor<T>, is_array<T>> {};

	template <template<class> class C, class T>
	using check_constraints = std::enable_if_t<C<T>::value, bool>;

	template <class T, template <class> class C = is_container,
		check_constraints<C, T> = true>
		static void contract(
			std::vector<T>& containerList,
			std::vector<std::vector<int>> legsList,
			std::vector<int> contractionSequenceLegs = {},
			std::vector<int> finalOrder = {})
	{
		NetworkContractor::contract(
			containerList,
			legsList,
			contractionSequenceLegs,
			finalOrder);
	};
};

int add(int i, int j) {
	return i + j;
}

int sub(int i, int j) {
	return i - j;
}

PYBIND11_MODULE(_nconpp, m)
{
	xt::import_numpy();

	m.doc() = R"pbdoc(
	contracts multiple tensors to one tensor, if possible.
	
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

	//m.def("contract", &WithConstraints::contract<xt::pyarray<std::complex<double>, xt::layout_type::dynamic>>);
	m.def("contract", &NetworkContractor::contract);

	m.def("add", &add, R"pbdoc(
        Add two numbers

        Some other explanation about the add function.
    )pbdoc");

	m.def("sub", &sub, R"pbdoc(
        Sub two numbers

        Some other explanation about the sub function.
    )pbdoc");
}
