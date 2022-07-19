#include "TensorNetwork.h"

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

template<class T>
TensorNetwork<T>::TensorNetwork(std::vector<T> &tensorList, std::vector<std::vector<int>> &subscriptVectorList) :
        Graph(subscriptVectorList.size()) {
    validateInputData(tensorList, subscriptVectorList);

    generateVerticesTensorAndVerticesLegs(tensorList, subscriptVectorList);

    generateEdges();
}

template<class T>
void TensorNetwork<T>::validateInputData(const std::vector<T> &tensorList,
                                         const std::vector<std::vector<int>> &subscriptVectorList) {
    if (tensorList.size() != subscriptVectorList.size()) {
        throw std::invalid_argument(
                "The number of tensors, which is " +
                std::to_string(tensorList.size()) +
                ", does not match the number of legs, which is " +
                std::to_string(subscriptVectorList.size()) + ".");
    }
}

template<class T>
void TensorNetwork<T>::generateVerticesTensorAndVerticesLegs(std::vector<T> &tensorList,
                                                             std::vector<std::vector<int>> &subscriptVectorList) {
    for (int i = 0; i < tensorList.size(); i++) {
        VertexTensor tnv{mVertices[i]};
        tnv.tensor = tensorList[i];
        mVerticesTensors.emplace_back(tnv);

        VertexLegs tnl{mVertices[i]};
        tnl.legs = subscriptVectorList[i];
        tnl.dims = Tensor::shape(tensorList[i]);
        mVerticesLegs.emplace_back(tnl);
    }
}

template<class T>
void TensorNetwork<T>::generateEdges() {
    for (auto i_vc: mVerticesLegs) {
        auto i_legs = i_vc.legs;
        for (auto j_vc: mVerticesLegs) {
            auto j_legs = j_vc.legs;
            if (!Utils::getIntersection(i_legs, j_legs).empty())
                Graph::constructEdge(i_vc.getVertex(), j_vc.getVertex());
        }
    }
}

template<class T>
void TensorNetwork<T>::doTrace(int vertexId, const std::vector<size_t> &axisA, const std::vector<size_t> &axisB) {
    Tensor::trace(mVerticesTensors[vertexId], 0, axisA[0], axisB[0]);
    mVerticesLegs[vertexId].erase(mVerticesLegs[vertexId].begin() + axisA[0]);
    mVerticesLegs[vertexId].erase(mVerticesLegs[vertexId].begin() + axisB[0]);
}

template<class T>
void TensorNetwork<T>::doTensorProduct(size_t indexA, size_t indexB, const std::vector<std::size_t> &axisA,
                                       const std::vector<std::size_t> &axisB) {
    auto &ta = mVerticesTensors[indexA];
    auto &tb = mVerticesTensors[indexB];

    const auto &newTensor = Tensor::tensordot(std::move(ta), std::move(tb), axisA, axisB);

    auto legsA = mVerticesLegs[indexA];
    auto legsB = mVerticesLegs[indexB];

    mVerticesLegs.erase(mVerticesLegs.begin() + indexA);
    mVerticesLegs.erase(mVerticesLegs.begin() + indexB);

    Utils::removeByIndicesFromVector(legsA, axisA.begin(), axisA.end());
    Utils::removeByIndicesFromVector(legsB, axisB.begin(), axisB.end());

    legsA.insert(legsA.end(), legsB.begin(), legsB.end());

    mVerticesTensors.emplace_back(newTensor);
    mVerticesLegs.emplace_back(legsA);
}

template<class T>
T TensorNetwork<T>::contract(std::vector<int> &contractionSequence) {

    auto it_cs = contractionSequence.begin();
    for (auto vli: mVerticesLegs) {
        std::vector<int> results = getPositions(vli.legs, *it_cs);
        if (results.size() == 2) {
            // TODO doTrace
        } else if (results.size() == 1) {
            // doTensorProduct
            for (auto vlj: mVerticesLegs) {
                // TODO
            }
        } else {
            // error
        }
    }

    validateOutputData();

    return std::move(mVerticesTensors[0].tensor);
}

template<class T>
void TensorNetwork<T>::validateOutputData(const std::vector<int> &contractionSequence) {
    if (!contractionSequence.empty()) {
        throw std::invalid_argument(
                "The contraction sequence vector, which size is " +
                std::to_string(contractionSequence.size()) +
                ", is not empty.");
    }
}

template<class T>
std::vector<int> TensorNetwork<T>::getPositions(const std::vector<int> &search, int match) {
    std::vector<int> results;
    auto pos = std::find_if(search.begin(), search.end(), [match](int i) { return i == match; });
    while (pos != search.end()) {
        results.emplace_back(std::distance(search.begin(), pos));
        pos = std::find_if(std::next(pos), search.end(), [match](int i) { return i == match; });
    }
    return results;
}

template<class T>
void TensorNetwork<T>::expandTensorNetwork(int vertexIndex, int legIndex) {
    T &container_type = mVerticesTensors[vertexIndex];
    size_t dim = Tensor::dimension(container_type);
    container_type = Tensor::expand_dims(container_type, dim);
    mVerticesLegs[vertexIndex].legs.emplace_back(legIndex);
}
