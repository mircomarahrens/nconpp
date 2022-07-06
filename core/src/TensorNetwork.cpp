#include "TensorNetwork.h"

#include "Tensor.h"
#include "Utils.h"

using namespace std;


template<class T>
TensorNetwork::TensorNetwork(vector<T> &tensorList, std::vector<std::vector<int>> subscriptVectorList) :
        Graph(subscriptVectorList.size()) {
    // 1. data validation
    // 2. extract VertexContainer
    getEdgesFromVerticesLegsList(subscriptVectorList);
}

const vector<VertexContainer<vector, Leg>>&
TensorNetwork::getVertexLegsSet() {
    return mVertexContainerList;
}

void TensorNetwork::getEdgesFromVerticesLegsSet() {
    for (auto i_vc: mVertexContainerList) {
        auto i_container = i_vc.getContainer();
        for (auto j_vc: mVertexContainerList) {
            auto j_container = j_vc.getContainer();
            if (!Utils::getIntersection(i_container, j_container).empty())
                Graph::constructEdge(i_vc.getVertex(), j_vc.getVertex());
        }
    }
}

void TensorNetwork::getEdgesFromVerticesLegsList(const vector<std::vector<int>> &verticesLegsList) {

}

void TensorNetwork::doTrace(int vertexId, const vector<size_t> &axisA, const vector<size_t> &axisB) {
    auto newTensor = Tensor::trace(mVertexContainerList[vertexId], 0, axisA[0], axisB[0]);

    containerList.erase(containerList.begin() + indexA);
    containerList.emplace_back(newTensor);

    auto legs = legsList[indexA];

    // remove indices from legs
    const vector<size_t> indices{axisA[0], axisB[0]};
    // TODO recheck
    //  -> remove elements by reverse iteration
    //  -> and by iterator not by index
    Utils::removeIndicesFromVector(legs, indices);

    // remove old legs
    legsList.erase(legsList.begin() + indexA);
    legsList.emplace_back(legs);
}
