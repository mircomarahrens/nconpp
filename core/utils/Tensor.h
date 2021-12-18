#pragma once

#include <xtensor/xarray.hpp>
#include <xtensor/xexpression.hpp>
#include <xtensor/xlayout.hpp>
#include <xtensor-blas/xlinalg.hpp>

// Wrapper for tensor operations used by nconpp.
// The current approach is based on xtensor due to its
// availability for modern package manager like conan
// or vcpkg and its native numpy style.
// 
// This may be replaced later.
namespace Tensor
{
	// array
	template <typename dtype>
	using array_type = xt::xarray<dtype, xt::layout_type::dynamic>;

	// shape_type
	using shape_type = xt::xarray<size_t>::shape_type;

	// reshape
	template <typename T>
	static inline auto reshape(array_type<T>& M, shape_type shape)
	{
		return M.reshape(shape);
	}

	// shape
	template <typename T>
	static inline auto shape(array_type<T>& M)
	{
		return M.shape();
	}

	// shape
	template <typename T>
	static inline auto dimension(array_type<T>& M)
	{
		return M.dimension();
	}

	// expand_dims
	template <typename T>
	static inline auto expand_dims(array_type<T>& M, std::size_t axis)
	{
		return xt::expand_dims(M, axis);
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
	static inline auto trace(const array_type<T>& M, int offset = 0, std::size_t axis1 = 0, std::size_t axis2 = 1)
	{
		auto d = xt::diagonal(M, offset, axis1, axis2);
		std::size_t dim = d.dimension();
		if (dim == 1)
		{
			return array_type<T>(xt::sum(d)());
		}
		else
		{
			return array_type<T>(xt::sum(d, { dim - 1 }));
		}
	}

	// transpose
	template <typename T>
	static inline auto transpose(array_type<T>& M, const std::vector<int>& perm)
	{
		xt::transpose(M, perm);
	}
};
