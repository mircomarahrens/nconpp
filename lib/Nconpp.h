#pragma once

#include "Utils.h"
#include "TensorNetwork.h"
#include "Tensor.h"

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

private:
    static auto findContractionParameters(
            int contractionLeg,
            std::vector<std::vector<int>> &legsList);
};
