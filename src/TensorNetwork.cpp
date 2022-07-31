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

TensorNetwork::TensorNetwork(std::vector<Tensor<std::complex<double>>> &tensorList, std::vector<std::vector<int>> &subscriptVectorList) :
        Graph(subscriptVectorList.size()) {
    validateInputData(tensorList, subscriptVectorList);

    generateVerticesTensorAndVerticesLegs(tensorList, subscriptVectorList);

    generateEdges();
}

void TensorNetwork::validateInputData(const std::vector<Tensor<std::complex<double>>> &tensorList,
                                         const std::vector<std::vector<int>> &subscriptVectorList) {
    if (tensorList.size() != subscriptVectorList.size()) {
        throw std::invalid_argument(
                "The number of tensors, which is " +
                std::to_string(tensorList.size()) +
                ", does not match the number of legs, which is " +
                std::to_string(subscriptVectorList.size()) + ".");
    }
}

void TensorNetwork::generateVerticesTensorAndVerticesLegs(std::vector<Tensor<std::complex<double>>> &tensorList,
                                                             std::vector<std::vector<int>> &subscriptVectorList) {
    for (int i = 0; i < tensorList.size(); i++) {
        VertexTensor tnv{mVertices[i], tensorList[i]};
        mVerticesTensors.emplace_back(tnv);

        VertexLegs tnl{mVertices[i]};
        tnl.legs = subscriptVectorList[i];
        tnl.dims = TensorOperations::shape(tensorList[i]);
        mVerticesLegs.emplace_back(tnl);
    }
}

void TensorNetwork::generateEdges() {
    for (auto i_vc: mVerticesLegs) {
        auto i_legs = i_vc.legs;
        for (auto j_vc: mVerticesLegs) {
            auto j_legs = j_vc.legs;
            if (!Utils::getIntersection(i_legs, j_legs).empty())
                Graph::constructEdge(i_vc.getVertex(), j_vc.getVertex());
        }
    }
}

// TODO ???
void TensorNetwork::doTrace(int vertexId, const std::vector<size_t> &axisA, const std::vector<size_t> &axisB) {
    TensorOperations::trace(mVerticesTensors[vertexId].tensor, 0, axisA[0], axisB[0], true);
    //mVerticesLegs[vertexId].erase(mVerticesLegs[vertexId].begin() + axisA[0]);
    //mVerticesLegs[vertexId].erase(mVerticesLegs[vertexId].begin() + axisB[0]);
}

void TensorNetwork::doTensorProduct(size_t indexA, size_t indexB, const std::vector<std::size_t> &axisA,
                                       const std::vector<std::size_t> &axisB) {
    auto &ta = mVerticesTensors[indexA];
    auto &tb = mVerticesTensors[indexB];

    const auto &newTensor = TensorOperations::tensordot(std::move(ta), std::move(tb), axisA, axisB);

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

Tensor<std::complex<double>> TensorNetwork::contract(std::vector<int> &contractionSequence) {

    auto iter = contractionSequence.begin();
    for (auto iv: mVerticesLegs) {
        auto il = iv.legs;
        if (std::find(il.begin(), il.end(), *iter) != il.end()) {
            const auto &axisA = getPositions(il, *iter);
            if (axisA.size() == 2) { // doTrace
                doTrace(iv.getVertex().index, axisA[0], axisA[1]);
            } else if (axisA.size() == 1) { // doTensorProduct
                for (auto jv: mVerticesLegs) {
                    auto jl = jv.legs;
                    if (std::find(jl.begin(), jl.end(), *iter) != jl.end()) {
                        const auto &axisB = getPositions(jl, *iter);
                        if (axisB.size() == 1) {
                            doTensorProduct(iv.getVertex().index, jv.getVertex().index, axisA, axisB);
                        } else {
                            // TODO error?
                        }
                    }
                }
            } else {
                // TODO error
            }
            contractionSequence.erase(iter);
        }
    }

    validateOutputData(contractionSequence);

    return std::move(mVerticesTensors[0].tensor);
}

void TensorNetwork::validateOutputData(const std::vector<int> &contractionSequence) {
    if (!contractionSequence.empty()) {
        throw std::invalid_argument(
                "The contraction sequence vector, which size is " +
                std::to_string(contractionSequence.size()) +
                ", is not empty.");
    }
}

std::vector<std::size_t> TensorNetwork::getPositions(const std::vector<int> &search, int match) {
    std::vector<std::size_t> results;
    auto pos = std::find_if(search.begin(), search.end(), [match](int i) { return i == match; });
    while (pos != search.end()) {
        results.emplace_back((size_t) std::distance(search.begin(), pos));
        pos = std::find_if(std::next(pos), search.end(), [match](int i) { return i == match; });
    }
    return results;
}

void TensorNetwork::expandTensorNetwork(int vertexIndex, int legIndex) {
    Tensor &container_type = mVerticesTensors[vertexIndex];
    size_t dim = TensorOperations::dimension(container_type);
    container_type = TensorOperations::expand_dims(container_type, dim);
    mVerticesLegs[vertexIndex].legs.emplace_back(legIndex);
}
