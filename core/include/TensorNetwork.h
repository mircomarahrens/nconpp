#pragma once

#include <unordered_map>

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

template<class D, template<typename> class V, typename T>
class VertexContainer {
public:
    VertexContainer(const Vertex &vertex, D data, V<T> container) :
            mVertex(vertex), mData(data), mContainer(container) {}

    VertexContainer() = default;

    ~VertexContainer() = default;

    const Vertex &getVertex() {
        return mVertex;
    }

    V<T>& getContainer() const {
        return mContainer;
    }

    D& getData() {
        return mData;
    }

    void setContainer(V<T> container) {
        VertexContainer::mContainer = container;
    }

    void setData(D data) {
        VertexContainer::mData = data;
    }

private:
    const Vertex &mVertex;
    V<T> mContainer;
    D mData;
};

template<class T>
class TensorNetwork : public Graph {
public:
    explicit TensorNetwork(std::vector<T> &tensorList, std::vector<std::vector<int>> subscriptVectorList);

    ~TensorNetwork() = default;

    const std::vector<VertexContainer<T, std::vector, Leg>> &getVertexLegsSet();

    void doTrace(int vertexId, const std::vector<size_t> &axisA, const std::vector<size_t> &axisB);

private:
    std::vector<VertexContainer<T, std::vector, Leg>> mVertexContainerList;

    std::unordered_map<int, Leg> mLegs;

    void getEdgesFromVerticesLegsSet();
};
