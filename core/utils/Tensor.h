#pragma once

#include <xtensor/xarray.hpp>
#include <xtensor/xexpression.hpp>
#include <xtensor/xlayout.hpp>
#include <xtensor-blas/xlinalg.hpp>

// Wrapper for tensor operations used by nconpp based on xtensor.
namespace Tensor
{
	// array
	template <typename dtype>
	using array_type = xt::xarray<dtype, xt::layout_type::dynamic>;

	// shape_type
	using shape_type = xt::xarray<size_t>::shape_type;

	// reshape
	template <typename T>
	static inline auto reshape(const xt::xexpression<T>& M, shape_type shape)
	{
		auto&& dM = M.derived_cast();
		return dM.reshape(shape);
	}

	// shape
	template <typename T>
	static inline auto shape(const xt::xexpression<T>& M)
	{
		auto&& dM = M.derived_cast();
		return dM.shape();
	}

	// shape
	template <typename T>
	static inline auto dimension(const xt::xexpression<T>& M)
	{
		auto&& dM = M.derived_cast();
		return dM.dimension();
	}

	// expand_dims
	template <typename T>
	static inline auto expand_dims(xt::xexpression<T>& M, std::size_t axis)
	{
		auto&& dM = M.derived_cast();
		return xt::expand_dims(dM, axis);
	}

	// tensordot
	template <typename T, typename O>
	static inline auto tensordot(const xt::xexpression<T>& xa, const xt::xexpression<O>& xb, const std::vector<std::size_t>& ax_a,
		const std::vector<std::size_t>& ax_b) -> decltype(xt::linalg::tensordot(xa, xb, ax_a, ax_b))
	{
		return xt::linalg::tensordot(xa, xb, ax_a, ax_b);
	}

	// trace
	template <typename T>
	static inline auto trace(const xt::xexpression<T>& M, int offset = 0, std::size_t axis1 = 0, std::size_t axis2 = 1)
	{
		using value_type = std::common_type_t<typename T::value_type>;
		using result_type = std::conditional_t<
			(T::static_layout != xt::layout_type::dynamic && T::static_layout != xt::layout_type::any),
			xt::xarray<value_type, T::static_layout>,
			xt::xarray<value_type, XTENSOR_DEFAULT_LAYOUT>>;

		result_type result;
		auto&& vM = xt::view_eval<T::static_layout>(M.derived_cast());
		auto d = xt::diagonal(vM, offset, std::size_t(axis1), std::size_t(axis2));
		std::size_t dim = d.dimension();
		if (dim == 1)
		{
			return xt::xarray<std::complex<double>>(xt::sum(d)());
		}
		else
		{
			return xt::xarray<std::complex<double>>(xt::sum(d, { dim - 1 }));
		}
	}

	// transpose
	template <typename T>
	static inline auto transpose(const xt::xexpression<T>& M, const std::vector<int>& perm)
	{
		auto&& dM = M.derived_cast();
		return xt::transpose(dM, perm);
	}
};
