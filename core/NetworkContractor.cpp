#include "NetworkContractor.h"

#include "utils/Container.h"
#include "utils/ErrorMessages.h"
#include "Search.h"

#include <set>
#include <stdexcept>
#include <vector>

using namespace std;

template <class T, template <class> class C,
	NetworkContractor::check_constraints<C, T>>
	T NetworkContractor::contract(
		vector<T> containerList,
		vector<vector<int>> legsList,
		vector<int> contractionSequenceLegs,
		vector<int> finalOrder)
{
	validateInput(
		containerList,
		legsList,
		contractionSequenceLegs,
		finalOrder
	);

	connectDisconnectedComponents(
		containerList,
		legsList,
		contractionSequenceLegs
	);

	while (!contractionSequenceLegs.empty())
	{
		auto it = contractionSequenceLegs.begin();
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
			doTensorProduct(
				indexA,
				indexB,
				axisA,
				axisB,
				containerList,
				legsList);
		}
		contractionSequenceLegs
			.erase(contractionSequenceLegs.begin());
	}

	return containerList[0];
}

template <class T>
void NetworkContractor::validateInput(
	const vector<T>& tensorList,
	vector<vector<int>>& legsList,
	vector<int>& contractionSequenceLegs,
	vector<int>& finalOrder)
{
	if (legsList.empty())
		throw invalid_argument("LegsList empty. You need to specify a list of legs corresponding to your network.");

	// check sizes of container
	if (tensorList.size() != legsList.size())
	{
		throw invalid_argument(
			"The number of tensors, which is " +
			to_string(tensorList.size()) +
			", does not match the number of legs, which is " +
			to_string(legsList.size()) + ".");
	}

	// check if legs are contractable

	// if empty fill with defaults
	if (contractionSequenceLegs.empty())
	{
		set<int> conSet = Container::allUniqueIntegersSorted(legsList);
		contractionSequenceLegs.assign(conSet.begin(), conSet.end());
		Container::removeNegatives(contractionSequenceLegs);
	}

	if (finalOrder.empty())
	{
		set<int> conSet = Container::allUniqueIntegersSorted(legsList);
		finalOrder.assign(conSet.begin(), conSet.end());
		Container::removePositives(finalOrder);
		reverse(finalOrder.begin(), finalOrder.end());
	}
}


// Network operations
int NetworkContractor::getShortestOfLegsList(const set<int>& indexSet,
	const vector<vector<int>>& legsList)
{
	int shortest = 0;
	for (int index : indexSet)
		if (legsList[index].size() < legsList[shortest].size())
			shortest = index;
	return shortest;
}

int NetworkContractor::getLongestOfLegsList(const set<int>& indexSet,
	const vector<vector<int>>& legsList)
{
	int longest = 0;
	for (int index : indexSet)
		if (legsList[index].size() > legsList[longest].size())
			longest = index;
	return longest;
}

int NetworkContractor::getNewLeg(const vector<int>& contractionSequenceLegs)
{
	int newLeg = *max_element(
		contractionSequenceLegs.begin(),
		contractionSequenceLegs.end());
	newLeg += 1;
	return newLeg;
}

void NetworkContractor::addEdge(
	vector<vector<int>>& legsList,
	int src,
	int dest)
{
	vector<int>& vertexSrc = legsList[src];
	vector<int>& vertexDest = legsList[dest];

	if (Container::getIntersection(vertexSrc, vertexDest).empty())
	{
		auto legs = Container::allUniqueIntegersSorted(legsList);
		int newLegIndex = 0;
		if (!legs.empty())
		{
			auto ind = max_element(legs.begin(), legs.end());
			newLegIndex = *ind + 1;
		}
		vertexSrc.push_back(newLegIndex);
		vertexDest.push_back(newLegIndex);
	}
}

template<class T>
void NetworkContractor::expandNetwork(
	int node,
	int leg,
	vector<T>& containerList,
	vector<vector<int>>& legsList)
{
	T& container_type = containerList[node];
	size_t dim = Tensor::dimension(container_type);
	container_type =
		Tensor::expand_dims(container_type, dim);
	legsList[node].push_back(leg);
}

template <class T>
void NetworkContractor::connectDisconnectedComponents(
	vector<T>& containerList,
	vector<vector<int>>& legsList,
	vector<int>& contractionSequenceLegs)
{
	Network network{ legsList };
	vector<set<int>> connectedComponents = network.getConnectedComponents();

	if (connectedComponents.size() > 1u)
	{
		int lastNode = 0, newLeg, currentNode;
		enum class st { EVEN, ODD }; st s = st::EVEN;
		auto it = connectedComponents.begin();
		while (it != connectedComponents.end())
		{
			currentNode = getShortestOfLegsList(*it, legsList);
			switch (s)
			{
			case (st::EVEN):
				newLeg = getNewLeg(contractionSequenceLegs);
				contractionSequenceLegs.push_back(newLeg);
				s = st::ODD;
				break;
			case (st::ODD):
				network.addEdge(lastNode, currentNode);
				s = st::EVEN;
				break;
			}

			expandNetwork(currentNode, newLeg, containerList, legsList);

			lastNode = currentNode;
			it++;
		}

		if (s == st::ODD)
		{
			// don't consider the last element, as this needs to be connected
			vector<vector<int>> legsSubList(
				legsList.begin(), legsList.end() - 1);
			set<int> subComponents = network.getVertices();
			subComponents.erase(currentNode);

			currentNode = getShortestOfLegsList(
				subComponents,
				legsSubList);

			network.addEdge(lastNode, currentNode);

			expandNetwork(currentNode, newLeg, containerList, legsList);
		}
	}
}

template<class T>
void NetworkContractor::doTrace(
	size_t indexA,
	size_t indexB,
	const vector<size_t>& axisA,
	const vector<size_t>& axisB,
	// TODO network
	vector<T>& containerList,
	vector<vector<int>>& legsList)
{
	auto newTensor = Tensor::trace(
		containerList[indexA], 0, axisA[0], axisB[0]);

	containerList.erase(containerList.begin() + indexA);
	containerList.emplace_back(newTensor);

	auto legs = legsList[indexA];

	// remove indices from legs
	const vector<size_t> indices{ axisA[0], axisB[0] };
	// TODO recheck
	//  -> remove elements by reverse iteration
	//  -> and by iterator not by index
	Container::removeIndicesFromVector(legs, indices);

	// remove old legs
	legsList.erase(legsList.begin() + indexA);
	legsList.emplace_back(legs);
}

template<class T>
void NetworkContractor::doTensorProduct(
	size_t indexA,
	size_t indexB,
	const vector<size_t>& axisA,
	const vector<size_t>& axisB,
	// TODO network
	vector<T>& containerList,
	vector<vector<int>>& legsList)
{
	const auto& newTensor = Tensor::tensordot(
		containerList[indexA], containerList[indexB], axisA, axisB);

	const vector<size_t> indices{ indexA, indexB };

	Container::removeIndicesFromVector(containerList, indices);
	containerList.emplace_back(newTensor);

	auto legsA = legsList[indexA];
	auto legsB = legsList[indexB];

	Container::removeIndicesFromVector(legsA, axisA);
	Container::removeIndicesFromVector(legsB, axisB);

	legsA.insert(legsA.end(), legsB.begin(), legsB.end());

	Container::removeIndicesFromVector(legsList, indices);

	legsList.emplace_back(legsA);
}

pair<pair<size_t, vector<size_t>>, pair<size_t, vector<size_t>>>
NetworkContractor::findContractionParameters(
	int contractionLeg,
	vector<vector<int>>& legsList)
{
	// index to tensor, position of legs to contract
	pair<size_t, vector<size_t>> tensorParamsA, tensorParamsB;
	tensorParamsA.second = {};
	tensorParamsB.second = {};
	bool visitA = false, visitB = false;
	for (int ind = 0; ind < legsList.size(); ++ind)
	{
		for (int leg : legsList[ind])
		{
			if (contractionLeg == leg)
			{
				int legIndex;
				if (!visitA)
				{
					tensorParamsA.first = ind;
					legIndex = *Container::getIndexToElement(legsList[ind], leg);
					tensorParamsA.second.emplace_back((size_t)legIndex);
					visitA = true;
				}
				else if (visitA && !visitB)
				{
					tensorParamsB.first = ind;
					if (tensorParamsB.first == tensorParamsA.first) // trace
						legIndex = *Container::getIndexToElement(legsList[ind], leg, (int)tensorParamsA.second[0] + 1);
					else // tensor product
						legIndex = *Container::getIndexToElement(legsList[ind], leg);
					tensorParamsB.second.emplace_back((size_t)legIndex);
					visitB = true;
				}
				else if (visitA && visitB)
					throw logic_error(ERROR_MESSAGES::MISMATCH);
			}
		}
	}
	return make_pair(tensorParamsA, tensorParamsB);
}
