#include "NetworkContractor.h"
#include "NetworkContractor.cpp"

class binding : public NetworkContractor
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

	template <template<class> class C, class T>
	using check_constraints = std::enable_if_t<C<T>::value, bool>;

	template <class T>
	struct is_container : xtl::disjunction<is_tensor<T>, is_array<T>> {};

	template <class T, template <class> class C = is_container,
		check_constraints<C, T> = true>
	static void contractWrapper(
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
