//
// Created by mirco on 10/5/2022.
//

#ifndef NCONPP_NBLAS_H
#define NCONPP_NBLAS_H

#include "Tensor.h"

#include "cblas.h"

namespace nblas {
    template <class T, class U, class R>
    void ger(const Tensor<T>& x, const Tensor<U>& y, R& result)
    {
        // TODO
    }

    template <class T, class U, class R>
    void dot(const Tensor<T>& x, const Tensor<U>& y, R& result)
    {
        // TODO
    }

    template <class T, class U, class R>
    void dotu(const Tensor<T>& x, const Tensor<U>& y, R& result)
    {
        // TODO
    }


//    /**
//     * Non-broadcasting dot function.
//     * In the case of two 1D vectors, computes the vector dot
//     * product. In the case of complex vectors, computes the dot
//     * product without conjugating the first argument.
//     * If \em t or \em o is a 2D matrix, computes the matrix-times-vector
//     * product. If both \em t and \em o ar 2D matrices, computes
//     * the matrix-product.
//     *
//     * @param t input array
//     * @param o input array
//     *
//     * @return resulting array
//     */
//    template <class T>
//    auto dot(const Tensor<T>& t, const Tensor<T>& o)
//    {
//        Tensor<T> result;
//        // both scalar
//        if (t.dimension() == 0 || o.dimension() == 0)
//        {
//            return (t * o);
//        }
//        // both vectors
//        if (t.dimension() == 1 && o.dimension() == 1)
//        {
//            result.reshape(std::vector<std::size_t>{1});
//            if (t.shape()[0] != o.shape()[0])
//            {
//                _THROW()
//                //XTENSOR_THROW(std::runtime_error, "Dot: shape mismatch.");
//            }
//
////            if (xtl::is_complex<typename T::value_type>::value)
////            {
//                blas::dotu(t, o, result(0));
////            }
////            else
////            {
////                blas::dot(t, o, result(0));
////            }
//            return result;
//        }
//        // else one or both higher dimensional objects
//        else
//        {
//            if (t.dimension() == 2 && o.dimension() == 1)
//            {
//                XTENSOR_ASSERT(t.layout() == layout_type::row_major || t.layout() == layout_type::column_major);
//                XTENSOR_ASSERT(std::min(t.strides()[0], t.strides()[1]) <= 1);
//
//                if (t.shape()[1] != o.shape()[0])
//                {
////                    XTENSOR_THROW(std::runtime_error, "Dot: shape mismatch.");
//                }
//
//                result.reshape({static_cast<std::size_t>(t.shape()[0])});
//
//                blas_index_t shape_x, shape_y;
//                cxxblas::Transpose trans;
//                if (result.layout() != t.layout())
//                {
//                    shape_x = static_cast<blas_index_t>(t.shape()[1]);
//                    shape_y = static_cast<blas_index_t>(t.shape()[0]);
//                    trans = cxxblas::Transpose::Trans;
//                }
//                else
//                {
//                    shape_x = static_cast<blas_index_t>(t.shape()[0]);
//                    shape_y = static_cast<blas_index_t>(t.shape()[1]);
//                    trans = cxxblas::Transpose::NoTrans;
//                }
//
//                cxxblas::gemv<blas_index_t>(
//                        get_blas_storage_order(result),
//                        trans,
//                        shape_x,
//                        shape_y,
//                        value_type(1.0),
//                        t.data() + t.data_offset(),
//                        get_leading_stride(t),
//                        o.data() + o.data_offset(),
//                        get_leading_stride(o),
//                        value_type(0.0),
//                        result.data(),
//                        get_leading_stride(result)
//                );
//            }
//            else if (t.dimension() == 1 && o.dimension() == 2)
//            {
//                XTENSOR_ASSERT(o.layout() == layout_type::row_major || o.layout() == layout_type::column_major);
//                XTENSOR_ASSERT(std::min(o.strides()[0], o.strides()[1]) <= 1);
//
//                if (t.shape()[0] != o.shape()[0])
//                {
//                    XTENSOR_THROW(std::runtime_error, "Dot: shape mismatch.");
//                }
//
//                result.resize({static_cast<std::size_t>(o.shape()[1])});
//
//                blas_index_t shape_x, shape_y;
//                cxxblas::Transpose trans;
//                if (result.layout() != o.layout())
//                {
//                    shape_x = static_cast<blas_index_t>(o.shape()[1]);
//                    shape_y = static_cast<blas_index_t>(o.shape()[0]);
//                    trans = cxxblas::Transpose::NoTrans;
//                }
//                else
//                {
//                    shape_x = static_cast<blas_index_t>(o.shape()[0]);
//                    shape_y = static_cast<blas_index_t>(o.shape()[1]);
//                    trans = cxxblas::Transpose::Trans;
//                }
//
//                cxxblas::gemv<blas_index_t>(
//                        get_blas_storage_order(result),
//                        trans,
//                        shape_x,
//                        shape_y,
//                        value_type(1.0),
//                        o.data() + o.data_offset(),
//                        get_leading_stride(o),
//                        t.data() + t.data_offset(),
//                        get_leading_stride(t),
//                        value_type(0.0),
//                        result.data(),
//                        get_leading_stride(result)
//                );
//            }
//            else if (t.dimension() == 2 && o.dimension() == 2)
//            {
//                XTENSOR_ASSERT(o.layout() == layout_type::row_major || o.layout() == layout_type::column_major);
//                XTENSOR_ASSERT(std::min(o.strides()[0], o.strides()[1]) <= 1);
//                XTENSOR_ASSERT(t.layout() == layout_type::row_major || t.layout() == layout_type::column_major);
//                XTENSOR_ASSERT(std::min(t.strides()[0], t.strides()[1]) <= 1);
//
//                if (t.shape()[1] != o.shape()[0])
//                {
//                    XTENSOR_THROW(std::runtime_error, "Dot: shape mismatch.");
//                }
//
//                cxxblas::Transpose transpose_A = cxxblas::Transpose::NoTrans,
//                        transpose_B = cxxblas::Transpose::NoTrans;
//
//                if (result.layout() != t.layout())
//                {
//                    transpose_A = cxxblas::Transpose::Trans;
//                }
//                if (result.layout() != o.layout())
//                {
//                    transpose_B = cxxblas::Transpose::Trans;
//                }
//
//                // This adds a fast path for A * A' by calling SYRK and only computing
//                // the upper triangle
//                if (std::is_same<typename T::value_type, typename O::value_type>::value &&
//                    (static_cast<const void*>(t.data() + t.data_offset()) == static_cast<const void*>(o.data() + o.data_offset())) &&
//                    ((transpose_A == cxxblas::Transpose::Trans && transpose_B == cxxblas::Transpose::NoTrans) ||
//                     (transpose_A == cxxblas::Transpose::NoTrans && transpose_B == cxxblas::Transpose::Trans)))
//                {
//                    // TODO add check to compare strides & shape
//
//                    result.resize({static_cast<std::size_t>(t.shape()[0]), static_cast<std::size_t>(t.shape()[0])});
//
//                    cxxblas::syrk<blas_index_t>(
//                            get_blas_storage_order(result),
//                            cxxblas::StorageUpLo::Upper,
//                            transpose_A,
//                            static_cast<blas_index_t>(t.shape()[0]),
//                            static_cast<blas_index_t>(t.shape()[1]),
//                            value_type(1.0),
//                            t.data() + t.data_offset(),
//                            get_leading_stride(t),
//                            value_type(0.0),
//                            result.data(),
//                            get_leading_stride(result)
//                    );
//
//                    for (std::size_t i = 0; i < t.shape()[0]; ++i)
//                    {
//                        for (std::size_t j = i + 1; j < t.shape()[0]; ++j)
//                        {
//                            result(j, i) = result(i, j);
//                        }
//                    }
//                    return result;
//                }
//
//                result.resize({static_cast<std::size_t>(t.shape()[0]), static_cast<std::size_t>(o.shape()[1])});
//
//                cxxblas::gemm<blas_index_t>(
//                        get_blas_storage_order(result),
//                        transpose_A,
//                        transpose_B,
//                        static_cast<blas_index_t>(t.shape()[0]),
//                        static_cast<blas_index_t>(o.shape()[1]),
//                        static_cast<blas_index_t>(o.shape()[0]),
//                        value_type(1.0),
//                        t.data() + t.data_offset(),
//                        get_leading_stride(t),
//                        o.data() + o.data_offset(),
//                        get_leading_stride(o),
//                        value_type(0.0),
//                        result.data(),
//                        get_leading_stride(result)
//                );
//            }
//            else
//            {
//                // TODO more testing for different layouts!
//                std::size_t l = t.shape().back();
//                std::size_t match_dim = 0;
//
//                if (o.dimension() > 1)
//                {
//                    match_dim = o.dimension() - 2;
//                }
//                if (o.shape()[match_dim] != l)
//                {
//                    XTENSOR_THROW(std::runtime_error, "Dot: shape mismatch.");
//                }
//
//                blas_index_t a_dim = static_cast<blas_index_t>(t.dimension());
//                blas_index_t b_dim = static_cast<blas_index_t>(o.dimension());
//
//                blas_index_t nd = a_dim + b_dim - 2;
//
//                std::size_t j = 0;
//                std::vector<std::size_t> dimensions(static_cast<std::size_t>(nd));
//
//                for (blas_index_t i = 0; i < a_dim - 1; ++i)
//                {
//                    dimensions[j++] = t.shape()[static_cast<std::size_t>(i)];
//                }
//                for (blas_index_t i = 0; i < b_dim - 2; ++i)
//                {
//                    dimensions[j++] = o.shape()[static_cast<std::size_t>(i)];
//                }
//                if (b_dim > 1)
//                {
//                    dimensions[j++] = o.shape().back();
//                }
//
//                result.resize(dimensions);
//
//                blas_index_t a_stride = static_cast<blas_index_t>(t.strides().back());
//                blas_index_t b_stride = static_cast<blas_index_t>(o.strides()[match_dim]);
//
//                auto a_iter = detail::offset_iter_without_axis<std::decay_t<decltype(t)>>(t, t.dimension() - 1);
//                auto b_iter = detail::offset_iter_without_axis<std::decay_t<decltype(o)>>(o, match_dim);
//
//                value_type temp;
//                auto result_it = result.begin();
//
//                do
//                {
//                    do
//                    {
//                        cxxblas::dot<blas_index_t>(
//                                static_cast<blas_index_t>(l),
//                                t.data() + a_iter.offset(),
//                                a_stride,
//                                o.data() + b_iter.offset(),
//                                b_stride,
//                                temp
//                        );
//                        *(result_it++) = temp;
//
//                    } while (b_iter.next());
//
//                } while (a_iter.next());
//
//            }
//            return result;
//        }
//    }
}

#endif //NCONPP_NBLAS_H
