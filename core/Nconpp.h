#pragma once

#include "include/Container.h"
#include "include/Network.h"
#include "include/Tensor.h"

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
    //      list of tensors
    //  legsList:
    //      Nomenclature of the legs of the tensor in tensorList:
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
            std::vector<std::vector<int>> legsList,
            bool skipValidation = true,
            std::vector<int> contractionSequenceLegs = {},
            std::vector<int> finalOrder = {});

//     template<class T>
//     static std::vector<T> decompose(
//             T &container,
//             std::vector<std::vector<int>> legsList,
//             int decompositionIndex,
//             bool skipValidation = true,
//             std::vector<int> decompositionSequenceLegs = {},
//             std::vector<int> finalOrder = {});

// TODO
//    template<class T>
//    static std::vector<T> diagonalize(
//            T &container,
//            std::vector<std::vector<int>> legsList,
//            std::vector<int> decompositionSequenceLegs = {},
//            std::vector<int> finalOrder = {});

private:
    template<class T>
    static void validateInput(
            const std::vector<T> &containerList,
            std::vector<std::vector<int>> &legsIndex,
            std::vector<int> &contractionSequenceLegs,
            std::vector<int> &finalOrder);

    // Network operations
    static int getShortestOfLegsList(
            const std::set<int> &indexSet,
            const std::vector<std::vector<int>> &legsList);

    static int getLongestOfLegsList(
            const std::set<int> &indexSet,
            const std::vector<std::vector<int>> &legsList);

    static int getNewLeg(
            const std::vector<int> &contractionSequenceLegs);

    static void addEdge(
            std::vector<std::vector<int>> &legsList,
            int src,
            int dest);

    template<class T>
    static void expandNetwork(
            int node,
            int leg,
            std::vector<T> &containerList,
            std::vector<std::vector<int>> &legsList
    );

    // trivial connection by extending the network
    // with leg pair of dimension one
    template<class T>
    static void connectDisconnectedComponents(
            std::vector<T> &containerList,
            std::vector<std::vector<int>> &legsList,
            std::vector<int> &contractionSequenceLegs);

    static std::tuple<std::pair<std::size_t, std::vector<std::size_t>>,
        std::pair<std::size_t, std::vector<std::size_t>>>
    findContractionParameters(
            int contractionLeg,
            std::vector<std::vector<int>> &legsList);

    // TensorNetwork operations
    template<class T>
    static void doTrace(
            size_t indexA,
            size_t indexB,
            const std::vector<std::size_t> &axisA,
            const std::vector<std::size_t> &axisB,
            std::vector<T> &containerList,
            std::vector<std::vector<int>> &legsList);

    template<class T>
    static void doTensorProduct(
            size_t indexA,
            size_t indexB,
            const std::vector<std::size_t> &axisA,
            const std::vector<std::size_t> &axisB,
            std::vector<T> &containerList,
            std::vector<std::vector<int>> &legsList);
};
