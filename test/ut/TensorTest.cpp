#include <gtest/gtest.h>

#define TEST_FRIENDS \
    friend class TensorTest_flatten1_Test; friend class TensorTest_flatten2_Test; friend class TensorTest_flatten3_Test; friend class TensorTest_transpose2_Test;

#include "Tensor.h"
#include "Tensor.cpp"

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

TEST(TensorTest, tensordot) {
//    auto A = xt::random::rand<double>({4, 3, 8});
//    auto B = xt::random::rand<double>({2, 3, 1, 3, 3});
//
//    auto C = TensorOperations::tensordot(A, B, {1}, {1});
//    TensorOperations::shape_type shapeC = {4, 8, 2, 1, 3, 3};
//    ASSERT_EQ(TensorOperations::Shape(C), shapeC);
//
//    TensorOperations::array_type<std::complex<double>> D = TensorOperations::trace(C, 0, 4, 5);
//
//    TensorOperations::shape_type shapeD = {4, 8, 2, 1};
//    ASSERT_EQ(TensorOperations::Shape(D), shapeD);
}

///////////////////////////////// PLAYGROUND //////////////////////////////////////

template<std::size_t... Indices>
struct indices {
};

template<class T>
struct Shape {
    std::vector<T> v;

    Shape(std::initializer_list<T> l) : v(l) {
        std::cout << "constructed with a " << l.size() << "-element list\n";
    }

    void append(std::initializer_list<T> l) {
        v.insert(v.end(), l.begin(), l.end());
    }
};

template<typename Tuple, size_t... Indices>
std::array<int, sizeof...(Indices)>
call_f_detail(Tuple &tuple, std::index_sequence<Indices...>) {
    return {f(std::get<Indices>(tuple))...};
}

template<typename Tuple>
std::array<int, std::tuple_size<Tuple>::value>
call_f(Tuple &tuple) {
    return call_f_detail(tuple,
            // make the sequence type sequence<0, 1, 2, ..., N-1>
                         std::make_index_sequence<std::tuple_size<Tuple>::value>{}
    );
}
//
//TEST(TensorTest, parameter_pack) {
//    indices<1, 2, 3, 4> inds;
//
//    Shape shape({1, 2, 3, 4});
//
////    std::tuple<double, double, double> in{1,2,3};
//    auto res = call_f(inds);
//}