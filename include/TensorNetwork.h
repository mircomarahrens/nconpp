#pragma once

#include "Graph.h"
#include "Tensor.h"

#include <algorithm>
#include <complex>

namespace Utils {
    template<typename D, typename Iter>
    void removeByIndicesFromVector(std::vector<D> &v, Iter begin, Iter end)
    // requires std::is_convertible_v<std::iterator_traits<Iter>::value_type, std::size_t>
    {
        std::size_t current_index = 0;

        if (std::is_sorted(begin, end)) {
            // sorted version - advance through begin..end
            auto rm_iter = begin;
            const auto pred = [&](const D &) {
                // anymore to remove?
                if (rm_iter != end && *rm_iter == current_index++) {
                    return ++rm_iter, true;
                }
                return false;
            };
            v.erase(std::remove_if(v.begin(), v.end(), pred), v.end());
        } else {
            // unsorted version - search for each index in begin..end
            const auto pred = [&](const D &) {
                return std::find(begin, end, current_index++) != end;
            };
            v.erase(std::remove_if(v.begin(), v.end(), pred), v.end());
        }
    };

    template<typename D>
    std::vector<D> getIntersection(std::vector<D> vec1,
                                   std::vector<D> vec2) {
        std::sort(vec1.begin(), vec1.end());
        std::sort(vec2.begin(), vec2.end());

        std::vector<D> intersec = {};
        std::set_intersection(vec1.begin(), vec1.end(),
                              vec2.begin(), vec2.end(),
                              std::back_inserter(intersec));

        return intersec;
    }
}

class TensorNetwork : public Graph {
public:
    /**
     * TODO add comment
     *
     * @param tensorList
     * @param subscriptVectorList
     */
    explicit TensorNetwork(std::vector<Tensor<std::complex<double>>> &tensorList,
                           std::vector<std::vector<int>> &subscriptVectorList) :
            Graph(subscriptVectorList.size()) {
        validateInputData(tensorList, subscriptVectorList);

        generateVerticesTensorAndVerticesLegs(tensorList, subscriptVectorList);

        generateEdges();
    };

    ~TensorNetwork() = default;

    /**
     * TODO add comment
     *
     * @param contractionSequence
     * @return
     */
    Tensor<std::complex<double>> contract(std::vector<int> &contractionSequence) {

        auto iter = contractionSequence.begin();
        for (auto iv: mVerticesTensors) {
            auto il = iv.getLegs();
            if (std::find(il.begin(), il.end(), *iter) != il.end()) {
                const auto &axisA = getPositions(il, *iter);
                if (axisA.size() == 2) {
                    trace(iv.getVertex().index, axisA[0], axisA[1]);
                } else if (axisA.size() == 1) {
                    for (auto jv: mVerticesTensors) {
                        auto jl = jv.getLegs();
                        if (std::find(jl.begin(), jl.end(), *iter) != jl.end()) {
                            const auto &axisB = getPositions(jl, *iter);
                            if (axisB.size() == 1) {
                                tensordot(iv.getVertex().index, jv.getVertex().index, axisA, axisB);
                            } else {
                                // TODO error?
                            }
                        }
                    }
                }
                contractionSequence.erase(iter);
            }
        }

        validateOutputData(contractionSequence);

        return mVerticesTensors[0].getTensor();
    };

private:
    struct TensorNetworkVertex {
    public:
        explicit TensorNetworkVertex(const Vertex &mVertex, Tensor<std::complex<double>> &tensor,
                                     std::vector<int> &legs) : mVertex(mVertex), mTensor(tensor),
                                                                  mLegs(legs) {}

        virtual ~TensorNetworkVertex() = default;

        const Vertex &getVertex() {
            return mVertex;
        }

        Tensor<std::complex<double>> &getTensor() {
            return mTensor;
        }

        std::vector<int> &getLegs() {
            return mLegs;
        }

        void addLeg(size_t id) {
            mLegs.emplace_back(id);
        }

    private:
        const Vertex &mVertex;
        Tensor<std::complex<double>> &mTensor;
        std::vector<int> &mLegs;
    };

    std::vector<TensorNetworkVertex> mVerticesTensors;


    static void validateInputData(const std::vector<Tensor<std::complex<double>>> &tensorList,
                                  const std::vector<std::vector<int>> &subscriptVectorList) {
        if (tensorList.size() != subscriptVectorList.size()) {
            throw std::invalid_argument(
                    "The number of tensors, which is " +
                    std::to_string(tensorList.size()) +
                    ", does not match the number of legs, which is " +
                    std::to_string(subscriptVectorList.size()) + ".");
        }
    };

    /**
     * TODO add comment
     *
     * @param contractionSequence
     */
    static void validateOutputData(const std::vector<int> &contractionSequence) {
        if (!contractionSequence.empty()) {
            throw std::invalid_argument(
                    "The contraction sequence vector, which size is " +
                    std::to_string(contractionSequence.size()) +
                    ", is not empty.");
        }
    };

    /**
     * TODO add comment
     *
     * @param tensorList
     * @param subscriptVectorList
     */
    void generateVerticesTensorAndVerticesLegs(std::vector<Tensor<std::complex<double>>> &tensorList,
                                               std::vector<std::vector<int>> &subscriptVectorList) {
        for (int i = 0; i < tensorList.size(); i++) {
            TensorNetworkVertex tnv{mVertices[i], tensorList[i], subscriptVectorList[i]};
            mVerticesTensors.emplace_back(tnv);
        }
    };

    /**
     * TODO add comment
     *
     */
    void generateEdges() {
        for (auto i_vc: mVerticesTensors) {
            auto i_legs = i_vc.getLegs();
            for (auto j_vc: mVerticesTensors) {
                auto j_legs = j_vc.getLegs();
                if (!Utils::getIntersection(i_legs, j_legs).empty())
                    Graph::constructEdge(i_vc.getVertex(), j_vc.getVertex());
            }
        }
    };

    /**
     * TODO add comment
     *
     * @param search
     * @param match
     * @return
     */
    static std::vector<std::size_t> getPositions(const std::vector<int> &search, int match) {
        std::vector<std::size_t> results;
        auto pos = std::find_if(search.begin(), search.end(), [match](int i) { return i == match; });
        while (pos != search.end()) {
            results.emplace_back((size_t) std::distance(search.begin(), pos));
            pos = std::find_if(std::next(pos), search.end(), [match](int i) { return i == match; });
        }
        return results;
    };

    /**
     * TODO add comment
     * inplace manipulation of tensors
     *
     */
    void trace(std::size_t index, std::size_t axis1 = 0, std::size_t axis2 = 1) {
        // TODO perform a trace in-place
    };

    /**
     * TODO add comment
     * inplace manipulation of tensors
     *
     * @param indexA
     * @param indexB
     * @param axisA
     * @param axisB
     */
    void tensordot(std::size_t indexA, std::size_t indexB, const std::vector<std::size_t> &axisA,
                   const std::vector<std::size_t> &axisB) {
        // TODO perform a tensordot in-place
    };
};
