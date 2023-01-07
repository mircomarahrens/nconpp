#include <gtest/gtest.h>

#define TEST_FRIENDS \
    friend class TensorTest_flatten1_Test; friend class TensorTest_flatten2_Test; friend class TensorTest_flatten3_Test; friend class TensorTest_transpose2_Test;

#include "Tensor.h"
#include "Tensor.cpp"
#include "nlinalg.h"

#include <complex>
#include <random>

class TensorTest : public testing::Test {
    TensorTest() = default;

    ~TensorTest() override = default;
};

TEST(DISABLED_TensorTest, permutation) {
    std::vector<int> s = {4, 3, 8};
    for (int i: s) {
        for (int j = 0; j < i; j++)
            std::cout << j << std::endl;
    }
}

TEST(TensorTest, flatten1) {
    std::vector<std::size_t> s = {2, 3, 2};
    Tensor<double> A({2, 3, 2});

    ASSERT_EQ(A.flatten(0, 0, 0), 0);
    ASSERT_EQ(A.flatten(0, 0, 1), 1);
    ASSERT_EQ(A.flatten(0, 1, 0), 2);
    ASSERT_EQ(A.flatten(0, 1, 1), 3);
    ASSERT_EQ(A.flatten(0, 2, 0), 4);
    ASSERT_EQ(A.flatten(0, 2, 1), 5);
    ASSERT_EQ(A.flatten(1, 0, 0), 6);
    ASSERT_EQ(A.flatten(1, 0, 1), 7);
    ASSERT_EQ(A.flatten(1, 1, 0), 8);
    ASSERT_EQ(A.flatten(1, 1, 1), 9);
    ASSERT_EQ(A.flatten(1, 2, 0), 10);
    ASSERT_EQ(A.flatten(1, 2, 1), 11);
}

TEST(TensorTest, flatten2) {
    std::vector<std::size_t> s = {2, 3, 2};
    std::vector<int> d = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    Tensor<int> A(d, {2, 3, 2});

    ASSERT_EQ(A.flatten(0, 0, 0), 0);
    ASSERT_EQ(A.flatten(0, 0, 1), 1);
    ASSERT_EQ(A.flatten(0, 1, 0), 2);
    ASSERT_EQ(A.flatten(0, 1, 1), 3);
    ASSERT_EQ(A.flatten(0, 2, 0), 4);
    ASSERT_EQ(A.flatten(0, 2, 1), 5);
    ASSERT_EQ(A.flatten(1, 0, 0), 6);
    ASSERT_EQ(A.flatten(1, 0, 1), 7);
    ASSERT_EQ(A.flatten(1, 1, 0), 8);
    ASSERT_EQ(A.flatten(1, 1, 1), 9);
    ASSERT_EQ(A.flatten(1, 2, 0), 10);
    ASSERT_EQ(A.flatten(1, 2, 1), 11);

    ASSERT_EQ(A(0, 0, 0), d[0]);
    ASSERT_EQ(A(0, 0, 1), d[1]);
    ASSERT_EQ(A(0, 1, 0), d[2]);
    ASSERT_EQ(A(0, 1, 1), d[3]);
    ASSERT_EQ(A(0, 2, 0), d[4]);
    ASSERT_EQ(A(0, 2, 1), d[5]);
    ASSERT_EQ(A(1, 0, 0), d[6]);
    ASSERT_EQ(A(1, 0, 1), d[7]);
    ASSERT_EQ(A(1, 1, 0), d[8]);
    ASSERT_EQ(A(1, 1, 1), d[9]);
    ASSERT_EQ(A(1, 2, 0), d[10]);
    ASSERT_EQ(A(1, 2, 1), d[11]);
}

TEST(TensorTest, flatten3) {
    std::vector<std::size_t> s = {16, 4, 4};
    Tensor<double> A(s);

    std::unordered_map<std::size_t, std::vector<std::size_t>> indices = {};

    std::size_t index = 0;
    for (std::size_t i0 = 0; i0 < s[0]; ++i0)
        for (std::size_t i1 = 0; i1 < s[1]; ++i1)
            for (std::size_t i2 = 0; i2 < s[2]; ++i2) {
                indices[index] = std::vector<std::size_t>({i0, i1, i2});
                index += 1;
            }

    for (const auto &[key, val]: indices) {
        ASSERT_EQ(A.flatten(val), key);
    }
}

TEST(TensorTest, constructorShape) {
    std::vector<std::size_t> s = {4, 3, 8};
    Tensor<double> A({4, 3, 8});
    ASSERT_EQ(A.shape(), s);
}

TEST(TensorTest, constructorRandomData) {
    std::vector<std::size_t> s = {4, 3, 1, 8};

    std::vector<double> data(4 * 3 * 8);

    std::default_random_engine gen{std::random_device{}()};
    std::uniform_real_distribution<double> dist(0, 1);
    std::generate(std::begin(data), std::end(data), [&] { return dist(gen); });

    Tensor<double> A(data, {4, 3, 1, 8});
    ASSERT_EQ(A.shape(), s);
    ASSERT_EQ(A.getData(), data);
    ASSERT_EQ(A.getData().size(), 4 * 3 * 8);
}

TEST(TensorTest, randomize) {
    std::vector<std::size_t> s = {4, 3, 8};
    Tensor<std::complex<double>> A({4, 3, 8});
    A.randomize();
    ASSERT_EQ(A.shape(), s);
    ASSERT_EQ(A.getData().size(), 4 * 3 * 8);
}

TEST(TensorTest, dimension) {
    std::vector<std::size_t> s = {4, 3, 8};
    Tensor<std::complex<double>> A({4, 3, 8});
    ASSERT_EQ(A.dimension(), 3);
}

TEST(TensorTest, expand_dims) {
    std::vector<std::size_t> s = {4, 3, 8};
    Tensor<std::complex<double>> A({4, 3, 8});

    A.expand_dims(0);
    std::vector<std::size_t> new_shape1 = {1, 4, 3, 8};
    ASSERT_EQ(A.dimension(), 4);
    ASSERT_EQ(A.shape(), new_shape1);

    A.expand_dims(4);
    std::vector<std::size_t> new_shape2 = {1, 4, 3, 8, 1};
    ASSERT_EQ(A.dimension(), 5);
    ASSERT_EQ(A.shape(), new_shape2);

    A.expand_dims(2);
    std::vector<std::size_t> new_shape3 = {1, 4, 1, 3, 8, 1};
    ASSERT_EQ(A.dimension(), 6);
    ASSERT_EQ(A.shape(), new_shape3);
}

TEST(TensorTest, reshapeMatch) {
    std::vector<std::size_t> s = {4, 3, 8};
    Tensor<std::complex<double>> A({4, 3, 8});
    A.reshape({3, 4, 2, 2, 2});
}

TEST(TensorTest, reshapeNoMatch) {
    std::vector<int> s = {4, 3, 8};
    Tensor<std::complex<double>> A({4, 3, 8});
    EXPECT_THROW(A.reshape({8, 8}), std::logic_error);
}

TEST(TensorTest, accessOperator) {
    std::vector<std::size_t> s = {4, 3, 8};
    Tensor<std::complex<double>> A({4, 3, 8});
    A.randomize();
    ASSERT_EQ(A.shape(), s);
    ASSERT_EQ(A.getData().size(), 4 * 3 * 8);

    std::complex<double> val(2.0, 2.0);
    A(1, 2, 4) = val;
    ASSERT_EQ(A(1, 2, 4), val);
}

TEST(TensorTest, accessOperator_IndexOutOfRange) {
    std::vector<std::size_t> s = {4, 3, 8};
    Tensor<std::complex<double>> A({4, 3, 8});
    A.randomize();
    ASSERT_EQ(A.shape(), s);
    ASSERT_EQ(A.getData().size(), 4 * 3 * 8);

    EXPECT_THROW(A(1, 4, 4), std::logic_error);
}

TEST(TensorTest, asterikOperator_int) {
    std::vector<std::size_t> s = {3, 1, 2};
    std::vector<int> d = {1, 2, 3, 4, 5, 6};
    Tensor<int> A(d, s);

    auto A1 = A * 2;
    auto A2 = 2 * A;
}

TEST(TensorTest, prod1) {
    std::vector<std::size_t> s = {3};
    std::vector<int> d = {1, 2, 3};
    Tensor<int> A(d, s);

    auto res = A.prod(0);

    ASSERT_EQ(res, 6);
}

TEST(TensorTest, prod2) {
    std::vector<std::size_t> s = {3, 1, 2};
    std::vector<int> d = {1, 2, 3, 4, 5, 6};
    Tensor<int> A(d, s);

    auto res1 = A.prod(1);
    ASSERT_EQ(res1, 6);

    auto res2 = A.prod(2);
    ASSERT_EQ(res2, 5 * 6);
}

TEST(TensorTest, prod3) {
    std::vector<std::size_t> s = {3, 2, 1, 3, 4};
    std::vector<int> d(72);
    std::iota(d.begin(), d.end(), 1);
    Tensor<int> A(d, s);

    auto res = A.prod({1, 3});

    ASSERT_EQ(res[0], A(2, 0, 0, 2, 3) * A(2, 1, 0, 2, 3));
    ASSERT_EQ(res[1], A(2, 1, 0, 0, 3) * A(2, 1, 0, 1, 3) * A(2, 1, 0, 2, 3));
}

TEST(TensorTest, transpose1) {
    std::vector<std::size_t> s = {8, 7, 6};
    std::vector<int> d(336);
    std::iota(d.begin(), d.end(), 0);
    Tensor<int> A(d, s);
    auto before = A(1, 2, 3);
    A.transpose(std::vector<std::size_t>({2, 0, 1}));
    ASSERT_EQ(before, A(3, 1, 2));
}

TEST(TensorTest, transpose2) {
    std::vector<std::size_t> s = {16, 4, 4};
    std::vector<int> d(256);
    std::iota(d.begin(), d.end(), 0);
    Tensor<int> A(d, s);

    std::unordered_map<std::size_t, std::vector<std::size_t>> indices = {};

    std::size_t index = 0;
    for (std::size_t i0 = 0; i0 < s[0]; ++i0)
        for (std::size_t i1 = 0; i1 < s[1]; ++i1)
            for (std::size_t i2 = 0; i2 < s[2]; ++i2) {
                indices[index] = std::vector<std::size_t>({i0, i1, i2});
                index += 1;
            }

    for (const auto &[key, val]: indices) {
        ASSERT_EQ(key, A.flatten(val));
    }

    A.transpose(std::vector<std::size_t>({2, 0, 1}));

    // TODO transform indices
}

TEST(DISABLED_TensorTest, dot) {
//    Tensor<std::complex<double>> A({4, 3, 2, 8});
//    A.randomize();
//
//    Tensor<std::complex<double>> B({8, 4, 2, 1, 3});
//    B.randomize();
//
//    auto C = nlinalg::dot(A, B);
}

TEST(DISABLED_TensorTest, tensordot) {
//    Tensor<std::complex<double>> A({4, 3, 2, 8});
//    A.randomize();
//
//    Tensor<std::complex<double>> B({8, 4, 2, 1, 3});
//    B.randomize();
//
//    auto C = tensordot(A, B, {0,2,3}, {1,2,0});

//    auto B = xt::random::rand<double>();
//    auto C = TensorOperations::tensordot(A, B, {1}, {1});
//    TensorOperations::shape_type shapeC = ;
//    ASSERT_EQ(TensorOperations::Shape(C), shapeC);
//
//    TensorOperations::array_type<std::complex<double>> D = TensorOperations::trace(C, 0, 4, 5);
//
//    TensorOperations::shape_type shapeD = {4, 8, 2, 1};
//    ASSERT_EQ(TensorOperations::Shape(D), shapeD);
}

///////////////////////////////// PLAYGROUND //////////////////////////////////////



//template <typename T>
//Tensor<T> tensordot(const Tensor<T>& a, const Tensor<T>& b, const std::vector<size_t>& axes_a, const std::vector<size_t>& axes_b) {
//    // Validate the input tensors and axes
//    assert(a.dimension() == axes_a.size());
//    assert(b.dimension() == axes_b.size());
//    assert(a.shape()[axes_a[0]] == b.shape()[axes_b[1]]);
//
//    // Compute the output shape
//    std::vector<std::size_t> output_shape;
//    for (std::size_t i = 0; i < axes_a.size(); i++) {
//        if (i != axes_a[0]) {
//            output_shape.push_back(a.shape()[i]);
//        }
//    }
//    for (std::size_t i = 0; i < axes_b.size(); i++) {
//        if (i != axes_b[1]) {
//            output_shape.push_back(b.shape()[i]);
//        }
//    }
//
//    // Create an output tensor with the correct shape
//    Tensor<T> output(output_shape);
//
//    // Compute the dot product
//    for (std::size_t i = 0; i < output.num_elements(); i++) {
//        std::vector<std::size_t> index = output.unflatten(i);
//        T sum = T(0);
//        for (std::size_t j = 0; j < a.shape()[axes_a[0]]; j++) {
//            index[axes_a[0]] = j;
//            index[axes_b[1]] = j;
//            a.flatten(index);
//            sum += b(b.flatten(index));
//        }
//        output(i) = sum;
//    }
//
//    return output;
//}

//template<std::size_t... Indices>
//struct indices {
//};
//
//template<class T>
//struct Shape {
//    std::vector<T> v;
//
//    Shape(std::initializer_list<T> l) : v(l) {
//        std::cout << "constructed with a " << l.size() << "-element list\n";
//    }
//
//    void append(std::initializer_list<T> l) {
//        v.insert(v.end(), l.begin(), l.end());
//    }
//};
//
//template<typename Tuple, size_t... Indices>
//std::array<int, sizeof...(Indices)>
//call_f_detail(Tuple &tuple, std::index_sequence<Indices...>) {
//    return {f(std::get<Indices>(tuple))...};
//}
//
//template<typename Tuple>
//std::array<int, std::tuple_size<Tuple>::value>
//call_f(Tuple &tuple) {
//    return call_f_detail(tuple,
//            // make the sequence type sequence<0, 1, 2, ..., N-1>
//                         std::make_index_sequence<std::tuple_size<Tuple>::value>{}
//    );
//}
//
//TEST(TensorTest, parameter_pack) {
//    Shape shape({1, 2, 3, 4});
//
//    std::tuple<double, double, double> in{1,2,3};
//    auto res = call_f(in);
//}

TEST(TensorTest, common_type) {
    using common_type_t = typename std::common_type<double, std::complex<double>>::type;

    Tensor<common_type_t> T({4, 3, 1, 8});
}