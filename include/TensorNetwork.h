#pragma once

#include "Graph.h"
#include "Tensor.h"

template<class T>
class TensorNetwork : public Graph {
public:
    explicit TensorNetwork(std::vector<T> &tensorList,
                           std::vector<std::vector<int>> &subscriptVectorList);

    ~TensorNetwork() = default;

    void doTrace(int vertexId, const std::vector<size_t> &axisA, const std::vector<size_t> &axisB);

    void doTensorProduct(size_t indexA, size_t indexB, const std::vector<std::size_t> &axisA,
                         const std::vector<std::size_t> &axisB);

    T contract(std::vector<int> &contractionSequence);

    void expandTensorNetwork(int vertexIndex, int legIndex);

private:
    struct VertexLegs {
    public:
        explicit VertexLegs(const Vertex &mVertex) :
                mVertex(mVertex) {}

        virtual ~VertexLegs() = default;

        const Vertex &getVertex() {
            return mVertex;
        }

        std::vector<int> legs = {};
        Tensor::shape_type dims = {};

    private:
        const Vertex &mVertex;
    };

    struct VertexTensor {
    public:
        explicit VertexTensor(const Vertex &mVertex) : mVertex(mVertex) {}

        virtual ~VertexTensor() = default;

        const Vertex &getVertex() {
            return mVertex;
        }

        T tensor;

    private:
        const Vertex &mVertex;
    };

    std::vector<VertexTensor> mVerticesTensors;

    std::vector<VertexLegs> mVerticesLegs;

    void validateInputData(const std::vector<T> &tensorList,
                           const std::vector<std::vector<int>> &subscriptVectorList);

    void validateOutputData(const std::vector<int> &contractionSequence);

    void generateVerticesTensorAndVerticesLegs(std::vector<T> &tensorList,
                                               std::vector<std::vector<int>> &subscriptVectorList);

    void generateEdges();

    std::vector<int> getPositions(const std::vector<int> &search, int match);
};
