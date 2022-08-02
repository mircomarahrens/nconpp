#pragma once

#include <vector>

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
    void randomize(std::size_t lower = 0, std::size_t upper = 1);

    // dimension
    auto dimension();

    // prod
    void prod(std::size_t axis);

    void prod(const std::vector<std::size_t> &axis);

    // expand_dims
    void expand_dims(std::size_t axis);

    void transpose(const std::vector<int> &perm);

    const auto &getData();

private:
    std::vector<T> mData;
    std::vector<int> mShape;
};
