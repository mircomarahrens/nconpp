#pragma once

#include "Graph.h"

template<class T>
class TensorNetwork : public Graph {
public:
    explicit TensorNetwork(std::vector<T> &tensorList, std::vector<std::vector<int>> subscriptVectorList);

    ~TensorNetwork() = default;

    void doTrace(int vertexId, const std::vector<size_t> &axisA, const std::vector<size_t> &axisB);

    void doTensorProduct(size_t indexA, size_t indexB, const std::vector<std::size_t> &axisA,
                         const std::vector<std::size_t> &axisB);

    T contract(std::vector<int>& contractionSequence);

private:
    struct VertexLegs {
    public:
        const Vertex &getVertex() {
            return mVertex;
        }

        std::vector<int> legs;
        std::vector<int> dims;

    private:
        const Vertex &mVertex;
    };

    struct VertexTensor {
    public:
        const Vertex &getVertex() {
            return mVertex;
        }

        T tensor;
    private:
        const Vertex &mVertex;
    };

    std::vector<VertexTensor> mVerticesTensors;

    std::vector<VertexLegs> mVerticesLegs;

    void validateData(const std::vector<T> &tensorList,
                      const std::vector<std::vector<int>> &subscriptVectorList);

    void generateVerticesTensorAndVerticesLegs(std::vector<T> &tensorList,
                                               std::vector<std::vector<int>> &subscriptVectorList);

    void generateEdges();
};
