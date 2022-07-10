#include "TensorNetwork.h"

#include "Tensor.h"
#include "Utils.h"

using namespace std;

template<class T>
TensorNetwork<T>::TensorNetwork(vector<T> &tensorList, vector<vector<int>> subscriptVectorList) :
        Graph(subscriptVectorList.size()) {
    // 1. data validation
    if (tensorList.size() != subscriptVectorList.size()) {
        throw invalid_argument(
                "The number of tensors, which is " +
                to_string(tensorList.size()) +
                ", does not match the number of legs, which is " +
                to_string(subscriptVectorList.size()) + ".");
    }

    // 2. extract VertexContainer
    generateVertexContainerList(tensorList, subscriptVectorList);

    generateEdges();
}

template<class T>
const vector<VertexContainer<T, vector, Leg>> &
TensorNetwork<T>::getVertexLegsSet() {
    return mVertexContainerList;
}

template<class T>
void TensorNetwork<T>::generateVertexContainerList(std::vector<T> &tensorList,
                                                   std::vector<std::vector<int>> &subscriptVectorList) {
    mLegs = {};
    for (int i = 0; i < tensorList.size(); i++) {
        int li = 0;
        vector<Leg> legs = {};
        for (int l: subscriptVectorList[i]) {
            auto s = Tensor::shape(tensorList[i]);
            Leg leg(l, s[li]);
            if (mLegs.find(l) != mLegs.end())
                mLegs[l] = leg;
            li++;
            legs.emplace_back(leg);
        }

        VertexContainer vc{mVertices[i], tensorList[i], legs};
        mVertexContainerList.emplace_back(vc);
    }
}

template<class T>
void TensorNetwork<T>::generateEdges() {
    for (auto i_vc: mVertexContainerList) {
        auto i_container = i_vc.getContainer();
        for (auto j_vc: mVertexContainerList) {
            auto j_container = j_vc.getContainer();
            if (!Utils::getIntersection(i_container, j_container).empty())
                Graph::constructEdge(i_vc.getVertex(), j_vc.getVertex());
        }
    }
}

template<class T>
void TensorNetwork<T>::doTrace(int vertexId, const vector<size_t> &axisA, const vector<size_t> &axisB) {
    auto vc = mVertexContainerList[vertexId];
    auto newTensor = Tensor::trace(vc.getData(), 0, axisA[0], axisB[0]);

    // TODO remove indices from legs
    auto legs = vc.getContainer();
    const vector<size_t> indices{axisA[0], axisB[0]};
    // TODO recheck
    //  -> remove elements by reverse iteration
    //  -> and by iterator not by index
    Utils::removeIndicesFromVector(legs, indices);

    // remove old legs
    legsList.erase(legsList.begin() + indexA);
    legsList.emplace_back(legs);
}
