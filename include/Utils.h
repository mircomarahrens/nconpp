#pragma once

#include <algorithm>
#include <iterator>
#include <numeric>
#include <optional>
#include <set>
#include <vector>

class Utils {
public:
    static void removeNegatives(std::vector<int> &vec);

    static void removeNegatives(std::set<int> &set);

    static void removePositives(std::vector<int> &vec);

    static void removePositives(std::set<int> &set);

    template<typename T>
    static std::set<T> allUniqueSorted(const std::vector<std::vector<T>> &container);

    static std::set<int> createRangeSet(size_t size);

    template<typename T>
    static std::optional<int> getIndexToElement(std::vector<T> vec,
                                                T element,
                                                int pos = 0);

    template<typename T>
    static std::vector<T> getIntersection(std::vector<T> vec1,
                                          std::vector<T> vec2);

    template<typename T, typename S>
    static void removeIndicesFromVector(std::vector<T> &v, const S &rm);

    template<typename T>
    static void extend(std::vector<T> &v_first, std::vector<T> &v_second);

private:
    static bool isNegative(int i) {
        return i < 0;
    };

    static bool isPositive(int i) {
        return i > 0;
    };

    template<typename T, typename Iter>
    static void removeIndicesFromVector(std::vector<T> &v, Iter begin, Iter end)
    // requires std::is_convertible_v<std::iterator_traits<Iter>::value_type, std::size_t>
    {
        std::size_t current_index = 0;

        if (std::is_sorted(begin, end)) {

            // sorted version - advance through begin..end
            auto rm_iter = begin;
            const auto pred = [&](const T &) {
                // anymore to remove?
                if (rm_iter != end && *rm_iter == current_index++) {
                    return ++rm_iter, true;
                }
                return false;
            };
            v.erase(std::remove_if(v.begin(), v.end(), pred), v.end());
        } else {
            // unsorted version - search for each index in begin..end
            const auto pred = [&](const T &) {
                return std::find(begin, end, current_index++) != end;
            };
            v.erase(std::remove_if(v.begin(), v.end(), pred), v.end());
        }
    };
};
