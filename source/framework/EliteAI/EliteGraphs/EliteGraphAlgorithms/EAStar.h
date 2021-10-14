#pragma once

namespace Elite
{
	template <class T_NodeType, class T_ConnectionType>
	class AStar
	{
	public:
		AStar(IGraph<T_NodeType, T_ConnectionType>* pGraph, Heuristic hFunction);

		// stores the optimal connection to a node and its total costs related to the start and end node of the path
		struct NodeRecord
		{
			T_NodeType* pNode = nullptr;
			T_ConnectionType* pConnection = nullptr;
			float costSoFar = 0.f; // accumulated g-costs of all the connections leading up to this one
			float estimatedTotalCost = 0.f; // f-cost (= costSoFar + h-cost)

			bool operator==(const NodeRecord& other) const
			{
				return pNode == other.pNode
					&& pConnection == other.pConnection
					&& costSoFar == other.costSoFar
					&& estimatedTotalCost == other.estimatedTotalCost;
			};

			bool operator<(const NodeRecord& other) const
			{
				return estimatedTotalCost < other.estimatedTotalCost;
			};
		};

		std::vector<T_NodeType*> FindPath(T_NodeType* pStartNode, T_NodeType* pDestinationNode);

	private:
		float GetHeuristicCost(T_NodeType* pStartNode, T_NodeType* pEndNode) const;

		IGraph<T_NodeType, T_ConnectionType>* m_pGraph;
		Heuristic m_HeuristicFunction;
	};

	template <class T_NodeType, class T_ConnectionType>
	AStar<T_NodeType, T_ConnectionType>::AStar(IGraph<T_NodeType, T_ConnectionType>* pGraph, Heuristic hFunction)
		: m_pGraph(pGraph)
		, m_HeuristicFunction(hFunction)
	{
	}

	template <class T_NodeType, class T_ConnectionType>
	std::vector<T_NodeType*> AStar<T_NodeType, T_ConnectionType>::FindPath(T_NodeType* pStartNode, T_NodeType* pGoalNode)
	{
		NodeRecord currentRecord{ pStartNode, nullptr, 0.f, GetHeuristicCost(pStartNode, pGoalNode) };

		std::vector<T_NodeType*> path;
		std::vector<NodeRecord> openList;
		std::vector<NodeRecord> closedList;

		openList.push_back(currentRecord);

		while (!openList.empty())
		{
			currentRecord = *std::min_element(openList.begin(), openList.end());

			if (currentRecord.pConnection != nullptr && m_pGraph->GetNode(currentRecord.pConnection->GetTo()) == pGoalNode)
			{
				break;
			}

			//get connections of current node
			NodeRecord newRecord{};
			for (T_ConnectionType* connection : m_pGraph->GetNodeConnections(currentRecord.pNode->GetIndex()))
			{
				newRecord.pNode = m_pGraph->GetNode(connection->GetTo());
				newRecord.pConnection = connection;
				newRecord.costSoFar = currentRecord.costSoFar + connection->GetCost();
				newRecord.estimatedTotalCost = newRecord.costSoFar + GetHeuristicCost(newRecord.pNode, pGoalNode);

				// check if connection is already on closed list
				auto checkNode = [newRecord](const NodeRecord& record) { return newRecord.pNode == record.pNode; };
				auto checkIt = std::find_if(closedList.begin(), closedList.end(), checkNode);
				if (checkIt != closedList.end())
				{
					if (checkIt->costSoFar <= newRecord.costSoFar)
					{
						continue;
					}
					else
					{
						closedList.erase(checkIt);
					}
				}

				auto checkItOpen = std::find_if(openList.begin(), openList.end(), checkNode);
				if (checkItOpen != openList.end())
				{
					if (checkItOpen->costSoFar <= newRecord.costSoFar)
					{
						continue;
					}
					else
					{
						openList.erase(checkItOpen);
					}
				}


				//add optimal node to openlist
				openList.push_back(newRecord);
			}


			// remove currentRecord from openlist and add to closed list
			openList.erase(std::remove(openList.begin(), openList.end(), currentRecord));

			closedList.push_back(currentRecord);

		}


		while (currentRecord.pNode != pStartNode)
		{
			path.push_back(currentRecord.pNode);
			for (const NodeRecord& record : closedList)
			{
				if (m_pGraph->GetNode(currentRecord.pConnection->GetFrom()) == record.pNode)
				{
					currentRecord = record;
					break;
				}
			}
		}
		path.push_back(pStartNode);
		std::reverse(path.begin(), path.end());

		if (path.back() != pGoalNode)
		{
			path.clear();
		}

		return path;
	}

	template <class T_NodeType, class T_ConnectionType>
	float Elite::AStar<T_NodeType, T_ConnectionType>::GetHeuristicCost(T_NodeType* pStartNode, T_NodeType* pEndNode) const
	{
		Vector2 toDestination = m_pGraph->GetNodePos(pEndNode) - m_pGraph->GetNodePos(pStartNode);
		return m_HeuristicFunction(abs(toDestination.x), abs(toDestination.y));
	}
}