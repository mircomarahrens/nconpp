#pragma once

#ifndef TEST_FRIENDS
#define TEST_FRIENDS
#endif

#include <vector>
#include <utility>
#include <unordered_map>

/**
 * This class is inspired by xtensor (https://github.com/xtensor-stack/xtensor) and the articles
 * https://johan-mabille.medium.com/how-we-wrote-xtensor-9365952372d9.
 *
 * @tparam T
 */
template<class T>
class Tensor {
public:
    explicit Tensor(std::vector<T> data, std::vector<std::size_t> shape);

    explicit Tensor(std::vector<std::size_t> shape);

    Tensor(std::initializer_list<std::size_t> shape);

    Tensor() = default;

    ~Tensor() = default;

    [[nodiscard]] std::size_t dimension() const;

    [[nodiscard]] std::size_t size() const;

    [[nodiscard]] std::size_t num_elements() const;

    [[nodiscard]] const std::vector<std::size_t> &shape() const;

    void reshape(const std::vector<std::size_t> &shape);

    void resize(const std::vector<std::size_t> &shape);

    void randomize(double lower = 0, double upper = 1.0);

    T prod(std::size_t axis);

    std::vector<T> prod(const std::vector<std::size_t> &axes);

    void expand_dims(std::size_t axis);

    void transpose(const std::vector<std::size_t> &perm);

    const auto &getData();

    auto &strides();

    template<class... I>
    T &operator()(I... i);

    template<class... I>
    const T &operator()(I... i) const;

    template<class I>
    Tensor<T> & operator*=(I rhs);

    template<class I>
    const Tensor<T> &operator*=(I rhs) const;

private:
    TEST_FRIENDS;
    std::vector<T> mData;
    std::vector<std::size_t> mShape;
    std::vector<std::size_t> mStrides;

    template<typename... Args>
    std::size_t flatten(Args... args);

    std::size_t flatten(std::vector<std::size_t> indices);

    std::size_t flatten_details(std::size_t size, std::vector<std::size_t> indices);

    void reorder(std::vector<std::size_t> &v, const std::vector<std::size_t> &order);

    void compute_strides(const std::vector<std::size_t> &shape);

    // checks
    void check_index_size(std::size_t index_size);

    void check_index(std::size_t index, std::size_t axis);

    void check_perm(const std::vector<std::size_t> &perm);

    void check_number_elements(std::size_t num_elements, std::size_t data_size);

    void check_new_shape(std::vector<std::size_t> s1, std::vector<std::size_t> s2);
};

template<class I, class T>
constexpr Tensor<T> operator*(I lhs, const Tensor<T>& rhs)
{
    auto cp = rhs;
    return cp *= lhs;
}

template<class I, class T>
constexpr Tensor<T> operator*(const Tensor<T>& lhs, I rhs)
{
    auto cp = lhs;
    return cp *= rhs;
}
