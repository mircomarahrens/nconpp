#pragma once

#include "Graph.h"

struct Leg {
    int index{0};
    int dim{0};

    friend bool operator<(const Leg &lhs, const Leg &rhs) {
        return lhs.index < rhs.index;
    };

    friend bool operator>(const Leg &lhs, const Leg &rhs) {
        return lhs.index > rhs.index;
    };

    friend bool operator==(const Leg &lhs, const Leg &rhs) {
        return lhs.index == rhs.index;
    };

    friend bool operator!=(const Leg &lhs, const Leg &rhs) {
        return lhs.index != rhs.index;
    };
};

template<template<typename> class T, typename U>
class VertexContainer {
public:

    VertexContainer(const Vertex &mVertex, T<U> mContainer) : mVertex(mVertex), mContainer(mContainer) {}

    VertexContainer() = default;

    ~VertexContainer() = default;

    const Vertex &getVertex() {
        return mVertex;
    }

    T<U> getContainer() const {
        return mContainer;
    }

private:
    const Vertex &mVertex;
    T<U> mContainer;
};

class TensorNetwork : public Graph {
public:
    template<class T>
    explicit TensorNetwork(std::vector<T> &tensorList, std::vector<std::vector<int>> subscriptVectorList);

    ~TensorNetwork() = default;

    const std::vector<VertexContainer<std::vector, Leg>> &getVertexLegsSet();

    void doTrace(int vertexId, const std::vector<size_t> &axisA, const std::vector<size_t> &axisB);

private:
    std::vector<VertexContainer<std::vector, Leg>> mVertexContainerList;

    void getEdgesFromVerticesLegsList(const std::vector<std::vector<int>> &verticesLegsList);
};
