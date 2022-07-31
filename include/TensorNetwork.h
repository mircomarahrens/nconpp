#pragma once

#include "Graph.h"
#include "Tensor.h"

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
    explicit TensorNetwork(std::vector<Tensor<std::complex<double>>> &tensorList,
                           std::vector<std::vector<int>> &subscriptVectorList);

    ~TensorNetwork() = default;

    Tensor<std::complex<double>> contract(std::vector<int> &contractionSequence);

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
                                  const std::vector<std::vector<int>> &subscriptVectorList);

    static void validateOutputData(const std::vector<int> &contractionSequence);

    void generateVerticesTensorAndVerticesLegs(std::vector<Tensor<std::complex<double>>> &tensorList,
                                               std::vector<std::vector<int>> &subscriptVectorList);

    void generateEdges();

    static std::vector<std::size_t> getPositions(const std::vector<int> &search, int match);

    void trace(std::size_t index, std::size_t axis1 = 0, std::size_t axis2 = 1);

    void tensordot(std::size_t indexA, std::size_t indexB, const std::vector<std::size_t> &axisA,
                   const std::vector<std::size_t> &axisB);
};
