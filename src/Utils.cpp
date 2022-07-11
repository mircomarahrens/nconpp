#include "Utils.h"

void Utils::removeNegatives(std::vector<int> &vec) {
    vec.erase(
            remove_if(
                    vec.begin(), vec.end(), isNegative),
            vec.end());
}

void Utils::removeNegatives(std::set<int> &set) {
    for (auto itr = set.begin(); itr != set.end();) {
        if (isNegative(*itr))
            set.erase(itr++);
    }
}

void Utils::removePositives(std::vector<int> &vec) {
    vec.erase(
            remove_if(
                    vec.begin(), vec.end(), isPositive),
            vec.end());
}

void Utils::removePositives(std::set<int> &set) {
    for (auto itr = set.begin(); itr != set.end();) {
        if (isPositive(*itr))
            set.erase(itr++);
    }
}

template<typename dtype>
std::set<dtype> Utils::allUniqueSorted(
        const std::vector<std::vector<dtype>> &container) {
    std::set<dtype> conSet;
    for (std::vector<dtype> data: container) {
        std::sort(data.begin(), data.end());
        std::set_union(data.begin(), data.end(),
                       conSet.begin(), conSet.end(),
                       std::inserter(conSet, conSet.end()));
    }
    return conSet;
}

std::set<int> Utils::createRangeSet(size_t size) {
    std::vector<int> vec(size);
    std::iota(vec.begin(), vec.end(), 0);
    return {vec.begin(), vec.end()};
}

template<typename dtype>
std::optional<int> Utils::getIndexToElement(std::vector<dtype> vec,
                                            dtype element,
                                            int pos) {
    auto it = std::find(vec.begin() + pos, vec.end(), element);
    if (it != vec.end())
        return int(std::distance(vec.begin(), it));
    return {};
}

template<typename dtype>
std::vector<dtype> Utils::getIntersection(std::vector<dtype> vec1,
                                          std::vector<dtype> vec2) {
    std::sort(vec1.begin(), vec1.end());
    std::sort(vec2.begin(), vec2.end());

    std::vector<dtype> intersec = {};
    std::set_intersection(vec1.begin(), vec1.end(),
                          vec2.begin(), vec2.end(),
                          std::back_inserter(intersec));

    return intersec;
}

template<typename T, typename S>
void Utils::removeIndicesFromVector(std::vector<T> &v, const S &rm) {
    using std::begin;
    using std::end;
    return removeIndicesFromVector(v, begin(rm), end(rm));
}

template<typename T>
void Utils::extend(std::vector<T> &v_first, std::vector<T> &v_second) {
    v_first.reserve(v_first.size() + distance(v_second.begin(), v_second.end()));
    v_first.insert(v_first.end(), v_second.begin(), v_second.end());
}
