#pragma once

#include "Graph.h"

template<class T>
class TensorNetwork : public Graph {
public:
    struct TensorNetworkVertex {
    public:
        const Vertex &getVertex() {
            return mVertex;
        }

        T tensor;

    private:
        const Vertex &mVertex;
    };

    struct TensorNetworkLeg {
    public:
        const Vertex &getVertex() {
            return mVertex;
        }

        int index{0};
        int dim{0};

        friend bool operator<(const TensorNetworkLeg &lhs, const TensorNetworkLeg &rhs) {
            return lhs.index < rhs.index;
        };

        friend bool operator>(const TensorNetworkLeg &lhs, const TensorNetworkLeg &rhs) {
            return lhs.index > rhs.index;
        };

        friend bool operator==(const TensorNetworkLeg &lhs, const TensorNetworkLeg &rhs) {
            return lhs.index == rhs.index && lhs.dim == rhs.dim;
        };

        friend bool operator!=(const TensorNetworkLeg &lhs, const TensorNetworkLeg &rhs) {
            return lhs.index != rhs.index || lhs.dim != rhs.dim;
        };

    private:
        const Vertex &mVertex;
    };

    explicit TensorNetwork(std::vector<T> &tensorList, std::vector<std::vector<int>> subscriptVectorList);

    ~TensorNetwork() = default;

    void doTrace(int vertexId, const std::vector<size_t> &axisA, const std::vector<size_t> &axisB);

    void doTensorProduct(size_t indexA, size_t indexB, const std::vector<std::size_t> &axisA,
                         const std::vector<std::size_t> &axisB);

private:
    std::vector<TensorNetworkVertex> mTensorNetworkVertices;

    std::vector<TensorNetworkLeg> mTensorNetworkLegs;

    void validateData(const std::vector<T> &tensorList,
                      const std::vector<std::vector<int>> &subscriptVectorList);

    void generateVerticesAndLegs(std::vector<T> &tensorList,
                                 std::vector<std::vector<int>> &subscriptVectorList);

    void generateEdges();
};
