#pragma once

#include <optional>
#include <set>
#include <vector>

class Nconpp {
public:
    Nconpp() = default;

    ~Nconpp() = default;

    // contracts multiple tensors to one tensor, if possible.
    //
    // @params:
    //  tensorList:
    //      a list of tensors
    //  subscriptVectorList:
    //      - aka LegsList
    //      - Nomenclature of the legs of the tensor in tensorList:
    //          - the legs are named by integers
    //          - contractable legs have the same positive integer as name,
    //            hence occuring in pairs
    //          - legs with negative integers won't be contracted, so called
    //            dangling legs
    //  contractionSequenceLegs (optional):
    //      order by legs in which the tensors shall be contracted
    //  finalOrder (optional):
    //      Permutation of the legs of the final tensor.
    //
    // @return:
    //  the final contracted tensor
    template<class T>
    static T contract(
            std::vector<T> &tensorList,
            std::vector<std::vector<int>> subscriptVectorList,
            std::vector<int> contractionSequence = {},
            std::vector<int> finalOrder = {});

    template<class T>
    static void connectDisconnectedComponents(
            std::vector<T> &tensorList,
            std::vector<std::vector<int>> subscriptVectorList,
            std::vector<int> contractionSequence);

private:
    static void fillContractionSequence(std::vector<int> &contractionSequence, const std::vector<int> &legIndices);

    static void fillFinalOrder(std::vector<int> &finalOrder, const std::vector<int> &legIndices);

    static std::vector<int> retrieveLegIndices(const std::vector<std::vector<int>> &subscriptVectorList);

    static bool isNegative(int i) {
        return i < 0;
    };

    static bool isPositive(int i) {
        return i > 0;
    };

    // Network operations
    static int getShortestOfSubscriptVectorList(
            const std::vector<int> &indexSet,
            const std::vector<std::vector<int>> &subscriptVectorList);

    static int getLongestOfSubscriptVectorList(
            const std::vector<int> &indexSet,
            const std::vector<std::vector<int>> &subscriptVectorList);

    static int getNewLeg(
            const std::vector<int> &contractionSequenceLegs);

    template<class T>
    static void expandRawTensorNetwork(
            int vertexIndex,
            int legIndex,
            std::vector<T> &tensorList,
            std::vector<std::vector<int>> &subscriptVectorList
    );
};
