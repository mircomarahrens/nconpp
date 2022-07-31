#pragma once

#include "Graph.h"
#include "Tensor.h"

class TensorNetwork : public Graph {
public:
    explicit TensorNetwork(std::vector<Tensor<std::complex<double>>> &tensorList,
                           std::vector<std::vector<int>> &subscriptVectorList);

    ~TensorNetwork() = default;

    Tensor<std::complex<double>> contract(std::vector<int> &contractionSequence);

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
        TensorOperations::shape_type dims = {};

    private:
        const Vertex &mVertex;
    };

    struct VertexTensor {
    public:
        explicit VertexTensor(const Vertex &mVertex, Tensor<std::complex<double>> &tensor) : mVertex(mVertex), mTensor(tensor) {}

        virtual ~VertexTensor() = default;

        const Vertex &getVertex() {
            return mVertex;
        }

        Tensor<std::complex<double>> &getTensor() {
            return mTensor;
        }

    private:
        const Vertex &mVertex;
        Tensor<std::complex<double>> &mTensor;
    };

    std::vector<VertexTensor> mVerticesTensors;

    std::vector<VertexLegs> mVerticesLegs;

    static void validateInputData(const std::vector<Tensor<std::complex<double>>> &tensorList,
                                  const std::vector<std::vector<int>> &subscriptVectorList);

    void validateOutputData(const std::vector<int> &contractionSequence);

    void generateVerticesTensorAndVerticesLegs(std::vector<Tensor<std::complex<double>>> &tensorList,
                                               std::vector<std::vector<int>> &subscriptVectorList);

    void generateEdges();

    std::vector<std::size_t> getPositions(const std::vector<int> &search, int match);

    void doTrace(int vertexId, const std::vector<size_t> &axisA, const std::vector<size_t> &axisB);

    void doTensorProduct(size_t indexA, size_t indexB, const std::vector<std::size_t> &axisA,
                         const std::vector<std::size_t> &axisB);
};
