#include <gtest/gtest.h>

#include <complex>
#include <random>

#include "Tensor.h"

using namespace std::complex_literals;

class TensorTest : public testing::Test {
  TensorTest() = default;

  ~TensorTest() override = default;
};

TEST(TensorTest, trace) {
  npp::shape_type shapeA = {4, 3, 2, 8, 2};
  npp::tensor_type<std::complex<double>> A =
      npp::random::rand<double>(shapeA) +
      1i * npp::random::rand<double>(shapeA);
  A = npp::linalg::trace(A, 0, 3, 4);

  shapeA = {4, 3, 2};
  ASSERT_EQ(npp::shape(A), shapeA);
}

TEST(TensorTest, tensordot) {
  npp::shape_type shapeA = {4, 3, 2, 8};
  npp::tensor_type<std::complex<double>> A =
      npp::random::rand<double>(shapeA) +
      1i * npp::random::rand<double>(shapeA);

  npp::shape_type shapeB = {8, 4, 2, 1, 3};
  npp::tensor_type<std::complex<double>> B =
      npp::random::rand<double>(shapeB) +
      1i * npp::random::rand<double>(shapeB);

  npp::tensor_type<std::complex<double>> C =
      npp::linalg::tensordot(A, B, {0, 2, 3}, {1, 2, 0});

  npp::shape_type shapeC = {3, 1, 3};
  ASSERT_EQ(npp::shape(C), shapeC);

  npp::tensor_type<std::complex<double>> D = npp::linalg::trace(C, 0, 0, 2);

  npp::shape_type shapeD = {1};
  ASSERT_EQ(npp::shape(D), shapeD);
}

TEST(TensorTest, svd) {
  // from
  // https://github.com/xtensor-stack/xtensor-blas/blob/master/test/test_linalg.cpp
  npp::tensor_type<double> arg_0 = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}};

  auto res = npp::linalg::svd(arg_0);

  npp::tensor_type<double> expected_0 = {
      {-0.13511895, 0.90281571, 0.40824829},
      {-0.49633514, 0.29493179, -0.81649658},
      {-0.85755134, -0.31295213, 0.40824829}};
  npp::tensor_type<double> expected_1 = {1.42267074e+01, 1.26522599e+00,
                                         5.89938022e-16};
  npp::tensor_type<double> expected_2 = {
      {-0.4663281, -0.57099079, -0.67565348},
      {-0.78477477, -0.08545673, 0.61386131},
      {-0.40824829, 0.81649658, -0.40824829}};

  ASSERT_TRUE(npp::allclose(std::get<0>(res), expected_0));
  ASSERT_TRUE(npp::allclose(std::get<1>(res), expected_1));
  ASSERT_TRUE(npp::allclose(std::get<2>(res), expected_2));

  npp::tensor_type<double> U = std::get<0>(res);
  ASSERT_EQ(U.shape(), npp::shape_type({3, 3}));

  npp::tensor_type<double> s = std::get<1>(res);
  ASSERT_EQ(s.shape(), npp::shape_type({3}));

  npp::tensor_type<double> V = std::get<2>(res);
  ASSERT_EQ(V.shape(), npp::shape_type({3, 3}));

  npp::tensor_type<double> smat = npp::zeros<double>({3, 3});
  for (size_t i = 0; i < s.shape()[0]; i++) smat(i, i) = s(i);

  auto result = npp::linalg::dot(npp::linalg::dot(U, smat), V);
  ASSERT_TRUE(npp::allclose(arg_0, result));
}

TEST(TensorTest, reshape) {
  npp::tensor_type<double> tensor = xt::random::rand<double>({3, 4, 5});

  std::size_t pos = 2;

  npp::shape_type shape = npp::shape(tensor);
  npp::shape_type left_shape = std::vector(shape.begin(), shape.begin() + pos);
  npp::shape_type right_shape = std::vector(shape.begin() + pos, shape.end());

  size_t left = 1;
  for (auto l : left_shape) {
    left *= l;
  }

  size_t right = 1;
  for (auto r : right_shape) {
    right *= r;
  }

  npp::shape_type new_shape = npp::shape_type({left, right});

  // copied via methods
  auto new_tensor = npp::reshape(tensor, {left, right});
  ASSERT_EQ(npp::shape(new_tensor), new_shape);

  // in place via member
  tensor.reshape({left, right});
  ASSERT_EQ(tensor.shape(), new_shape);
}

TEST(TensorTest, split) {
  auto shape = npp::shape_type({3, 4, 5, 6});
  npp::tensor_type<double> tensor = npp::random::rand<double>(shape);

  std::size_t len = shape.size();
  std::size_t left = 1, right = 1;
  std::size_t e = 2;
  npp::shape_type left_shape, right_shape;
  for (std::size_t s = 0; s < len; s++) {
    if (s < e) {
      left *= shape[s];
      left_shape.push_back(shape[s]);
    } else if (s == e) {
      right *= shape[s];

      left_shape.push_back(left);
      right_shape.push_back(left);

      right_shape.push_back(shape[s]);
    } else {
      right *= shape[s];

      right_shape.push_back(shape[s]);
    }
  }

  ASSERT_EQ(left_shape, npp::shape_type({3, 4, 12}));
  ASSERT_EQ(right_shape, npp::shape_type({12, 5, 6}));

  tensor.reshape({left, right});
  ASSERT_EQ(tensor.shape(), npp::shape_type({left, right}));

  auto res = npp::linalg::svd(tensor, false);

  npp::tensor_type<double> U = std::get<0>(res);
  ASSERT_EQ(U.shape(), npp::shape_type({left, left}));

  npp::tensor_type<double> s = std::get<1>(res);
  ASSERT_EQ(s.shape(), npp::shape_type({left}));

  npp::tensor_type<double> V = std::get<2>(res);
  ASSERT_EQ(V.shape(), npp::shape_type({left, right}));

  npp::tensor_type<double> smat = npp::diag(s);

  auto result = npp::linalg::dot(npp::linalg::dot(U, smat), V);
  ASSERT_TRUE(npp::allclose(tensor, result));

  U.reshape(left_shape);
  ASSERT_EQ(U.shape(), npp::shape_type(left_shape));

  V.reshape(right_shape);
  ASSERT_EQ(V.shape(), npp::shape_type(right_shape));
}
