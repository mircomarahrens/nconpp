#pragma once

#include "Network.h"
#include "utils/Container.h"
#include "utils/Tensor.h"

#include <vector>
#include <optional>
#include <set>

class NetworkContractor
{
public:
	NetworkContractor() = default;
	~NetworkContractor() = default;

	//static int add(int i, int j) {
	//	return i + j;
	//};

	//template<typename T>
	//static T add(T i, T j) {
	//	return i + j;
	//};

	template <class T>
	static T add(std::vector<T> i, std::vector<T> j)
	{
		return i[0] + j[0];
	};

	template <class T>
	static T contract_debug(std::vector<T>& containerList,
		std::vector<std::vector<int>> legsList,
		std::optional<std::vector<int>> contractionSequenceLegs ,
		std::optional<std::vector<int>> finalOrder)
	{
		// if empty fill with defaults
		std::vector<int> _contractionSequenceLegs = {};
		if (!contractionSequenceLegs.has_value()) {
			set<int> conSet = Container::allUniqueIntegersSorted(legsList);
			_contractionSequenceLegs.assign(conSet.begin(), conSet.end());
			Container::removeNegatives(_contractionSequenceLegs);
		}
		else
		{
			_contractionSequenceLegs = contractionSequenceLegs.value();
		}
		std::vector<int> _finalOrder = {};
		if (!finalOrder.has_value())
		{
			set<int> conSet = Container::allUniqueIntegersSorted(legsList);
			_finalOrder.assign(conSet.begin(), conSet.end());
			Container::removePositives(_finalOrder);
			std::reverse(_finalOrder.begin(), _finalOrder.end());
		}
		else
		{
			_finalOrder = finalOrder.value();
		}

		validateInput(
			containerList,
			legsList,
			_contractionSequenceLegs,
			_finalOrder
		);

		connectDisconnectedComponents(
			containerList,
			legsList,
			_contractionSequenceLegs
		);

		while (!_contractionSequenceLegs.empty())
		{
			auto it = _contractionSequenceLegs.begin();
			// 1. get contraction parameters:
			// TODO add multi index contraction
			auto contractionParams = findContractionParameters(
				*it,
				legsList);

			size_t indexA = contractionParams.first.first;
			size_t indexB = contractionParams.second.first;
			const auto& axisA = contractionParams.first.second;
			const auto& axisB = contractionParams.second.second;

			// 2. perform contraction of tensors
			if (indexA == indexB) // do trace
			{
				doTrace(
					indexA,
					indexB,
					axisA,
					axisB,
					containerList,
					legsList);
			}
			else // do tensor product
			{
				// TODO here is the problem
				// ImportError: DLL load failed while importing _nconpp: The specified module could not be found.
				// DependencyWalker: It seems to be that openblas.dll cannot be found.
				doTensorProduct(
					indexA,
					indexB,
					axisA,
					axisB,
					containerList,
					legsList);
			}
			_contractionSequenceLegs
				.erase(_contractionSequenceLegs.begin());
		}

		return containerList[0];
	};

	// contracts multiple tensors to one tensor, if possible.
	//
	// @params:
	//  containerList:
	//      list of containers, i.e. tensors to contract
	//  legsList:
	//      Nomenclature of the legs of the tensors in tensorList:
	//          - the legs are named by integers
	//          - contractable legs have the same positive integer as name,
	//            hence occuring in pairs
	//          - legs with negative integers won't be contracted, so called
	//            dangling legs
	//  contractionSequenceTensors (optional):
	//      order in which the tensors shall be contracted
	//  finalOrder (optional):
	//      Permutation of the legs of the final tensor.
	//
	// @return:
	//  the final contracted container
	template<class T>
	static T contract(
		std::vector<T>& containerList,
		std::vector<std::vector<int>> legsList,
		std::vector<int> contractionSequenceLegs = {},
		std::vector<int> finalOrder = {});

private:
	template <class T>
	static void validateInput(
		const std::vector<T>& containerList,
		std::vector<std::vector<int>>& legsIndex,
		std::vector<int>& contractionSequenceLegs,
		std::vector<int>& finalOrder);


	// Network operations
	static int getShortestOfLegsList(
		const std::set<int>& indexSet,
		const std::vector<std::vector<int>>& legsList);

	static int getLongestOfLegsList(
		const std::set<int>& indexSet,
		const std::vector<std::vector<int>>& legsList);

	static int getNewLeg(
		const std::vector<int>& contractionSequenceLegs);

	static void addEdge(
		std::vector<std::vector<int>>& legsList,
		int src,
		int dest);

	template <class T>
	static void expandNetwork(
		int node,
		int leg,
		std::vector<T>& containerList,
		std::vector<std::vector<int>>& legsList
	);

	// trivial connection by extending the network
	// with leg pair of dimension one
	template <class T>
	static void connectDisconnectedComponents(
		std::vector<T>& containerList,
		std::vector<std::vector<int>>& legsList,
		std::vector<int>& contractionSequenceLegs);

	static std::pair<std::pair<std::size_t, std::vector<std::size_t>>,
		std::pair<std::size_t, std::vector<std::size_t>>>
		findContractionParameters(
			int contractionLeg,
			std::vector<std::vector<int>>& legsList);

	// TensorNetwork operations
	template <class T>
	static void doTrace(
		size_t indexA,
		size_t indexB,
		const std::vector<std::size_t>& axisA,
		const std::vector<std::size_t>& axisB,
		std::vector<T>& containerList,
		std::vector<std::vector<int>>& legsList);

	template <class T>
	static void doTensorProduct(
		size_t indexA,
		size_t indexB,
		const std::vector<std::size_t>& axisA,
		const std::vector<std::size_t>& axisB,
		std::vector<T>& containerList,
		std::vector<std::vector<int>>& legsList);
};
