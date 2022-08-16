#include <gtest/gtest.h>

#define TEST_FRIENDS \
    friend class TensorTest_flatten_Test; friend class TensorTest_unflatten_Test;

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

TEST(TensorTest, flatten) {
    std::vector<int> s = {2, 3, 2};
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

TEST(TensorTest, unflatten) {
    std::vector<int> s = {2, 3, 2};
    Tensor<double> A(s);

    ASSERT_EQ(A.unflatten(0), std::vector<int>({ 0, 0, 0 }));
    ASSERT_EQ(A.unflatten(1), std::vector<int>({ 0, 0, 1 }));
    ASSERT_EQ(A.unflatten(2), std::vector<int>({ 0, 1, 0 }));
    ASSERT_EQ(A.unflatten(3), std::vector<int>({ 0, 1, 1 }));
    ASSERT_EQ(A.unflatten(4), std::vector<int>({ 0, 2, 0 }));
    ASSERT_EQ(A.unflatten(5), std::vector<int>({ 0, 2, 1 }));
    ASSERT_EQ(A.unflatten(6), std::vector<int>({ 1, 0, 0 }));
    ASSERT_EQ(A.unflatten(7), std::vector<int>({ 1, 0, 1 }));
    ASSERT_EQ(A.unflatten(8), std::vector<int>({ 1, 1, 0 }));
    ASSERT_EQ(A.unflatten(9), std::vector<int>({ 1, 1, 1 }));
    ASSERT_EQ(A.unflatten(10), std::vector<int>({ 1, 2, 0 }));
    ASSERT_EQ(A.unflatten(11), std::vector<int>({ 1, 2, 1 }));
}

TEST(TensorTest, constructorShape) {
    std::vector<int> s = {4, 3, 8};
    Tensor<double> A({4, 3, 8});
    ASSERT_EQ(A.shape(), s);
}

TEST(TensorTest, constructorRandomData) {
    std::vector<int> s = {4, 3, 1, 8};

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
    std::vector<int> s = {4, 3, 8};
    Tensor<std::complex<double>> A({4, 3, 8});
    A.randomize();
    ASSERT_EQ(A.shape(), s);
    ASSERT_EQ(A.getData().size(), 4 * 3 * 8);
}

TEST(TensorTest, dimension) {
    std::vector<int> s = {4, 3, 8};
    Tensor<std::complex<double>> A({4, 3, 8});
    ASSERT_EQ(A.dimension(), 3);
}

TEST(TensorTest, reshapeMatch) {
    std::vector<int> s = {4, 3, 8};
    Tensor<std::complex<double>> A({4, 3, 8});
    A.reshape({3, 4, 2, 2, 2});
}

TEST(TensorTest, reshapeNoMatch) {
    std::vector<int> s = {4, 3, 8};
    Tensor<std::complex<double>> A({4, 3, 8});
    EXPECT_THROW(A.reshape({8, 8}), std::logic_error);
}

TEST(TensorTest, prod1) {
    std::vector<int> s = {3};
    std::vector<int> d = {1, 2, 3};
    Tensor<int> A(d, s);

    auto res = A.prod(0);

    ASSERT_EQ(res, 6);
}

TEST(TensorTest, prod2) {
//    std::vector<int> s = {3, 1, 2, 1};
//    std::vector<int> d = {1, 2, 3, 4, 5, 6};
//    Tensor<int> A(d, s);
//
//    auto res = A.prod(1);
//
//    ASSERT_EQ(res, 4 * 5);
}

TEST(TensorTest, prod3) {
//    std::vector<int> s = {3, 2, 1, 3, 4};
//    std::vector<int> d = {1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 2, 3, 4};
//    Tensor<int> A(d, s);
//
//    auto res = A.prod({1, 3});
//
//    ASSERT_EQ(res[0], 4 * 5);
//    ASSERT_EQ(res[1], 7 * 8 * 9);
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