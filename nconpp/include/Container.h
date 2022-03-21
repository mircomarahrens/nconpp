#pragma once

#include <algorithm>
#include <iterator>
#include <optional>
#include <vector>
#include <set>
#include <numeric>

class Container
{
public:
    static void removeNegatives(std::vector<int>& vec)
    {
        vec.erase(
            remove_if(
                vec.begin(), vec.end(), isNegative),
            vec.end());
    };

    static void removeNegatives(std::set<int>& set)
    {
        for (auto itr = set.begin(); itr != set.end();)
        {
            if (isNegative(*itr))
                set.erase(itr++);
        }
    };

    static void removePositives(std::vector<int>& vec)
    {
        vec.erase(
            remove_if(
                vec.begin(), vec.end(), isPositive),
            vec.end());
    };

    static void removePositives(std::set<int>& set)
    {
        for (auto itr = set.begin(); itr != set.end();)
        {
            if (isPositive(*itr))
                set.erase(itr++);
        }
    };

    static std::set<int> allUniqueIntegersSorted(
        const std::vector<std::vector<int>>& legsIndex)
    {
        std::set<int> conSet;
        for (std::vector<int> legs : legsIndex)
        {
            std::sort(legs.begin(), legs.end());
            std::set_union(legs.begin(), legs.end(),
                conSet.begin(), conSet.end(),
                std::inserter(conSet, conSet.end()));
        }
        return conSet;
    };

    static std::set<int> createRangeSet(size_t size)
    {
        std::vector<int> vec(size);
        std::iota(vec.begin(), vec.end(), 0);
        return std::set<int>(vec.begin(), vec.end());
    };

    template <typename dtype>
    static std::optional<int> getIndexToElement(std::vector<dtype> vec,
        dtype element,
        int pos = 0)
    {
        auto it = std::find(vec.begin() + pos, vec.end(), element);
        if (it != vec.end())
            return int(std::distance(vec.begin(), it));
        return {};
    };

    static std::vector<int> getIntersection(std::vector<int> vec1,
        std::vector<int> vec2)
    {
        std::sort(vec1.begin(), vec1.end());
        std::sort(vec2.begin(), vec2.end());

        std::vector<int> intersec = {};
        std::set_intersection(vec1.begin(), vec1.end(),
            vec2.begin(), vec2.end(),
            std::back_inserter(intersec));

        return intersec;
    }

    template <typename T, typename S>
    // requires std::is_convertible_v<S::value_type, std::size_t>
    static void removeIndicesFromVector(std::vector<T>& v, const S& rm)
    {
        using std::begin;
        using std::end;
        return removeIndicesFromVector(v, begin(rm), end(rm));
    }

private:
    static bool isNegative(int i)
    {
        return i < 0;
    };
    static bool isPositive(int i)
    {
        return i > 0;
    };

    template <typename T, typename Iter>
    static void removeIndicesFromVector(std::vector<T>& v, Iter begin, Iter end)
        // requires std::is_convertible_v<std::iterator_traits<Iter>::value_type, std::size_t>
    {
        std::size_t current_index = 0;

        if (std::is_sorted(begin, end))
        {

            // sorted version - advance through begin..end
            auto rm_iter = begin;
            const auto pred = [&](const T&) {
                // anymore to remove?
                if (rm_iter != end && *rm_iter == current_index++)
                {
                    return ++rm_iter, true;
                }
                return false;
            };
            v.erase(std::remove_if(v.begin(), v.end(), pred), v.end());
        }
        else
        {
            // unsorted version - search for each index in begin..end
            const auto pred = [&](const T&) {
                return std::find(begin, end, current_index++) != end;
            };
            v.erase(std::remove_if(v.begin(), v.end(), pred), v.end());
        }
    };
};
