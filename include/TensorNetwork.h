#pragma once

#include "Graph.h"
#include "Tensor.h"

class TensorNetwork : public Graph {
public:
    explicit TensorNetwork(std::vector<Tensor<std::complex<double>>> &tensorList,
                           std::vector<std::vector<size_t>> &subscriptVectorList);

    ~TensorNetwork() = default;

    Tensor<std::complex<double>> contract(std::vector<int> &contractionSequence);

private:
    struct TensorNetworkVertex {
    public:
        explicit TensorNetworkVertex(const Vertex &mVertex, Tensor<std::complex<double>> &tensor,
                                     std::vector<size_t> &legs) : mVertex(mVertex), mTensor(tensor),
                                                                  mLegs(legs) {}

        virtual ~TensorNetworkVertex() = default;

        const Vertex &getVertex() {
            return mVertex;
        }

        Tensor<std::complex<double>> &getTensor() {
            return mTensor;
        }

        std::vector<size_t> &getLegs() {
            return mLegs;
        }

        void addLeg(size_t id) {
            mLegs.emplace_back(id);
        }

    private:
        const Vertex &mVertex;
        Tensor<std::complex<double>> &mTensor;
        std::vector<size_t> &mLegs;
    };

    std::vector<TensorNetworkVertex> mVerticesTensors;

    static void validateInputData(const std::vector<Tensor<std::complex<double>>> &tensorList,
                                  const std::vector<std::vector<size_t>> &subscriptVectorList);

    static void validateOutputData(const std::vector<int> &contractionSequence);

    void generateVerticesTensorAndVerticesLegs(std::vector<Tensor<std::complex<double>>> &tensorList,
                                               std::vector<std::vector<size_t>> &subscriptVectorList);

    void generateEdges();

    static std::vector<std::size_t> getPositions(const std::vector<size_t> &search, int match);

    void expandTensorNetwork(int vertexIndex, int legIndex);

    void trace(std::size_t index, std::size_t axis1 = 0, std::size_t axis2 = 1);

    void tensordot(std::size_t indexA, std::size_t indexB, const std::vector<std::size_t> &axisA,
                   const std::vector<std::size_t> &axisB);
};
