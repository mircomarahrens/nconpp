#pragma once

#ifndef TEST_FRIENDS
#define TEST_FRIENDS
#endif

#include <vector>
#include <string>
#include <unordered_map>

template<typename T>
class Tensor {
public:
    explicit Tensor(std::vector<T> data, std::vector<int> shape);

    explicit Tensor(std::vector<int> shape);

    ~Tensor() = default;

    // Shape
    const std::vector<int> &shape();

    // reshape
    void reshape(const std::vector<int> &shape);

    // random values
    void randomize(double lower = 0, double upper = 1.0);

    // dimension
    auto dimension();

    // prod
    T prod(int axis);

    std::vector<T> prod(std::vector<int> axis);

    // expand_dims
    void expand_dims(std::size_t axis);

    void transpose(const std::vector<int> &perm);

    const auto &getData();

private:
    TEST_FRIENDS;
    std::vector<T> mData;
    std::vector<int> mShape;

    template<typename... Args>
    int flatten(Args... args);

    auto unflatten(int val);

    std::unordered_map<int, int> getProducts();
};
