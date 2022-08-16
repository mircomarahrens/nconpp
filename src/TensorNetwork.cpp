#include "TensorNetwork.h"

TensorNetwork::TensorNetwork(std::vector<Tensor<std::complex<double>>> &tensorList,
                             std::vector<std::vector<int>> &subscriptVectorList) :
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
        TensorNetworkVertex tnv{mVertices[i], tensorList[i], subscriptVectorList[i]};
        mVerticesTensors.emplace_back(tnv);
    }
}

void TensorNetwork::generateEdges() {
    for (auto i_vc: mVerticesTensors) {
        auto i_legs = i_vc.getLegs();
        for (auto j_vc: mVerticesTensors) {
            auto j_legs = j_vc.getLegs();
            if (!Utils::getIntersection(i_legs, j_legs).empty())
                Graph::constructEdge(i_vc.getVertex(), j_vc.getVertex());
        }
    }
}

Tensor<std::complex<double>> TensorNetwork::contract(std::vector<int> &contractionSequence) {

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

// inplace manipulation of tensors
void TensorNetwork::trace(std::size_t index, std::size_t axis1, std::size_t axis2) {
    // TODO perform a trace in-place
}

// inplace manipulation of tensors
void TensorNetwork::tensordot(std::size_t indexA, std::size_t indexB, const std::vector<std::size_t> &axisA,
                              const std::vector<std::size_t> &axisB) {
    // TODO perform a tensordot in-place
}
