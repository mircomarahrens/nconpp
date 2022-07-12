#include "TensorNetwork.h"

#include "Tensor.h"
#include "Utils.h"

using namespace std;

template<class T>
TensorNetwork<T>::TensorNetwork(vector<T> &tensorList, vector<vector<int>> subscriptVectorList) :
        Graph(subscriptVectorList.size()) {
    validateData(tensorList, subscriptVectorList);

    generateVerticesTensorAndVerticesLegs(tensorList, subscriptVectorList);

    generateEdges();
}

template<class T>
void TensorNetwork<T>::validateData(const vector<T> &tensorList, const vector<vector<int>> &subscriptVectorList) {
    if (tensorList.size() != subscriptVectorList.size()) {
        throw invalid_argument(
                "The number of tensors, which is " +
                to_string(tensorList.size()) +
                ", does not match the number of legs, which is " +
                to_string(subscriptVectorList.size()) + ".");
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
void TensorNetwork<T>::doTrace(int vertexId, const vector<size_t> &axisA, const vector<size_t> &axisB) {
    Tensor::trace(mVerticesTensors[vertexId], 0, axisA[0], axisB[0]);
    mVerticesLegs[vertexId].erase(mVerticesLegs[vertexId].begin() + axisA[0]);
    mVerticesLegs[vertexId].erase(mVerticesLegs[vertexId].begin() + axisB[0]);
}

template<class T>
void TensorNetwork<T>::doTensorProduct(size_t indexA, size_t indexB, const std::vector<std::size_t> &axisA,
                                       const std::vector<std::size_t> &axisB) {
    auto& ta = mVerticesTensors[indexA];
    auto& tb = mVerticesTensors[indexB];

    const auto &newTensor = Tensor::tensordot(std::move(ta), std::move(tb), axisA, axisB);

    auto legsA = mVerticesLegs[indexA];
    auto legsB = mVerticesLegs[indexB];

    mVerticesLegs.erase(mVerticesLegs.begin() + indexA);
    mVerticesLegs.erase(mVerticesLegs.begin() + indexB);

    Utils::removeIndicesFromVector(legsA, axisA);
    Utils::removeIndicesFromVector(legsB, axisB);

    legsA.insert(legsA.end(), legsB.begin(), legsB.end());

    mVerticesTensors.emplace_back(newTensor);
    mVerticesLegs.emplace_back(legsA);
}

template<class T>
T TensorNetwork<T>::contract(std::vector<int>& contractionSequence) {


    return std::move(mVerticesTensors[0].tensor);
}

//template<class T>
//void Nconpp::connectDisconnectedComponents(
//        vector<T> &tensorList,
//        vector<vector<int>> &subscriptVectorList,
//        vector<int> &contractionSequence) {
//
//    TensorNetwork network{subscriptVectorList};
//    auto connectedComponents = network.getConnectedComponentsIndices();
//
//    // TODO connect components
//    if (connectedComponents.size() > 1u) {
//        Vertex lastVertex{0}, currentVertex;
//        int newLeg;
//        enum class parity {
//            EVEN, ODD
//        };
//        parity p = parity::EVEN;
//        auto it = connectedComponents.begin();
//        while (it != connectedComponents.end()) {
//            int tmp = getShortestOfLegsList(*it, subscriptVectorList);
//            currentVertex = Vertex{tmp};
//            switch (p) {
//                case (parity::EVEN):
//                    newLeg = getNewLeg(contractionSequence);
//                    contractionSequence.push_back(newLeg);
//                    p = parity::ODD;
//                    break;
//                case (parity::ODD):
//                    network.constructEdge(lastVertex, currentVertex);
//                    p = parity::EVEN;
//                    break;
//            }
//
//            expandNetwork(currentVertex, newLeg, tensorList, subscriptVectorList);
//
//            lastVertex = currentVertex;
//            it++;
//        }
//
//        if (p == parity::ODD) {
//            // don't consider the last element, as this needs to be connected
//            vector<vector<int>> legsSubList(
//                    subscriptVectorList.begin(), subscriptVectorList.end() - 1);
//            set<int> subComponents = network.getVertexIndices();
//            subComponents.erase(currentVertex);
//
//            currentVertex = getShortestOfLegsList(
//                    subComponents,
//                    legsSubList);
//
//            network.constructEdge(lastVertex, currentVertex);
//
//            expandNetwork(currentVertex, newLeg, tensorList, subscriptVectorList);
//        }
//    }
//}
//
//
//template<class T>
//void Nconpp::expandNetwork(
//        int node,
//        int leg,
//        vector<T> &containerList,
//        vector<vector<int>> &legsList) {
//    T &container_type = containerList[node];
//    size_t dim = Tensor::dimension(container_type);
//    container_type =
//            Tensor::expand_dims(container_type, dim);
//    legsList[node].push_back(leg);
//}

//// Network operations
//int Nconpp::getShortestOfLegsList(const set<int> &indexSet,
//                                  const vector<vector<int>> &legsList) {
//    int shortest = 0;
//    for (int index: indexSet)
//        if (legsList[index].size() < legsList[shortest].size())
//            shortest = index;
//    return shortest;
//}
//
//int Nconpp::getLongestOfLegsList(const set<int> &indexSet,
//                                 const vector<vector<int>> &legsList) {
//    int longest = 0;
//    for (int index: indexSet)
//        if (legsList[index].size() > legsList[longest].size())
//            longest = index;
//    return longest;
//}
//
//int Nconpp::getNewLeg(const vector<int> &contractionSequenceLegs) {
//    int newLeg = *max_element(
//            contractionSequenceLegs.begin(),
//            contractionSequenceLegs.end());
//    newLeg += 1;
//    return newLeg;
//}
//
//void Nconpp::addEdge(
//        vector<vector<int>> &legsList,
//        int src,
//        int dest) {
//    vector<int> &vertexSrc = legsList[src];
//    vector<int> &vertexDest = legsList[dest];
//
//    if (Utils::getIntersection(vertexSrc, vertexDest).empty()) {
//        auto legs = Utils::allUniqueSorted(legsList);
//        int newLegIndex = 0;
//        if (!legs.empty()) {
//            auto ind = max_element(legs.begin(), legs.end());
//            newLegIndex = *ind + 1;
//        }
//        vertexSrc.push_back(newLegIndex);
//        vertexDest.push_back(newLegIndex);
//    }
//}
