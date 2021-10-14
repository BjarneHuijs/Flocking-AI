#pragma once
#include <stack>

namespace Elite
{
	enum class Eulerianity
	{
		notEulerian,
		semiEulerian,
		eulerian,
	};

	template <class T_NodeType, class T_ConnectionType>
	class EulerianPath
	{
	public:

		EulerianPath(IGraph<T_NodeType, T_ConnectionType>* pGraph);

		Eulerianity IsEulerian() const;
		vector<T_NodeType*> FindPath(Eulerianity& eulerianity) const;

	private:
		void VisitAllNodesDFS(int startIdx, vector<bool>& visited) const;
		bool IsConnected() const;

		IGraph<T_NodeType, T_ConnectionType>* m_pGraph;
	};

	template<class T_NodeType, class T_ConnectionType>
	inline EulerianPath<T_NodeType, T_ConnectionType>::EulerianPath(IGraph<T_NodeType, T_ConnectionType>* pGraph)
		: m_pGraph(pGraph)
	{
	}

	template<class T_NodeType, class T_ConnectionType>
	inline Eulerianity EulerianPath<T_NodeType, T_ConnectionType>::IsEulerian() const
	{
		// If the graph is not connected, there can be no Eulerian Trail
		if (IsConnected() == false)
			return Eulerianity::notEulerian;

		// Count nodes with odd degree 
		int nrOfNodes{ m_pGraph->GetNrOfNodes() };
		int oddCount{};
		for(int i{}; i < nrOfNodes; i++)
		{
			if (m_pGraph->IsNodeValid(i) && (m_pGraph->GetNodeConnections(i).size() & 1))
				oddCount++;
		}

		// A connected graph with more than 2 nodes with an odd degree (an odd amount of connections) is not Eulerian
		if (oddCount > 2)
			return Eulerianity::notEulerian;

		// A connected graph with exactly 2 nodes with an odd degree is Semi-Eulerian (an Euler trail can be made, but only starting and ending in these 2 nodes)
		else if (oddCount == 2 && nrOfNodes != 2)
			return Eulerianity::semiEulerian;

		// A connected graph with no odd nodes is Eulerian
		//if(oddCount < 2)
		else
			return Eulerianity::eulerian;

	}

	template<class T_NodeType, class T_ConnectionType>
	inline vector<T_NodeType*> EulerianPath<T_NodeType, T_ConnectionType>::FindPath(Eulerianity& eulerianity) const
	{
		// Get a copy of the graph because this algorithm involves removing edges
		auto graphCopy = m_pGraph->Clone();
		int nrOfNodes = graphCopy->GetNrOfNodes();
		vector<T_NodeType*> path = vector<T_NodeType*>{};
		vector<T_NodeType*> circuit = vector<T_NodeType*>{};
		

		eulerianity = IsEulerian();

		if (eulerianity != Eulerianity::notEulerian)
		{
			std::vector<int> oddDegreeIndices{};
			int oddCount{};
			for (int i{}; i < nrOfNodes; i++)
			{
				if (graphCopy->IsNodeValid(i) && (graphCopy->GetNodeConnections(i).size() & 1))
				{
					oddCount++;
					oddDegreeIndices.push_back(i);
				}
			}

			int currentIndex{};
			if (eulerianity == Eulerianity::semiEulerian) 
			{
				int i = rand() % 2;
				currentIndex = oddDegreeIndices[i];
			}
			else 
			{
				int i = rand() % nrOfNodes;
				//can start from any node, so choose first node made
				//int i{ 0 };
				currentIndex = m_pGraph->GetAllNodes()[i]->GetIndex();
			}
			
			/*If current vertex has no neighbors 
				- add it to circuit, remove the last vertex from the stack and set it as the current one 
			Otherwise (in case it has neighbors) 
				- add the vertex to the stack, take any of its neighbors, remove the edge between selected neighbor and that vertex, and set that neighbor as the current vertex.
			Repeat step 2 until the current vertex has no more neighbors and the stack is empty.*/
			while(graphCopy->GetNodeConnections(currentIndex).size() > 0)
			{
				circuit.push_back(m_pGraph->GetNode(currentIndex));
				size_t nrOfconnections{ graphCopy->GetNodeConnections(currentIndex).size() };
				int connectionIndex = rand() % nrOfconnections;
				int nextIndex{};
				//int nextIndex{ m_pGraph->GetNodeConnections(currentIndex) };
				int i{};
				for(T_ConnectionType* connection : graphCopy->GetNodeConnections(currentIndex))
				{
					if (i == connectionIndex)
						nextIndex = connection->GetTo();
					i++;
				}


				graphCopy->RemoveNode(currentIndex);
				currentIndex = nextIndex;
			}
			circuit.push_back(m_pGraph->GetNode(currentIndex));
			graphCopy->RemoveNode(currentIndex);

			//path.resize(circuit.size());
			size_t size{ circuit.size() };
			for (size_t i{}; i < size; i++)
			{
				path.push_back(circuit.back());
				circuit.pop_back();
			}
			//circuit.clear();
		}

		std::string out{};
		for(size_t i{}; i < path.size(); i++)
		{
			out += std::to_string(path[i]->GetIndex()) + ", ";
		}
		std::cout << out << std::endl;

		return path;
	}

	template<class T_NodeType, class T_ConnectionType>
	inline void EulerianPath<T_NodeType, T_ConnectionType>::VisitAllNodesDFS(int startIdx, vector<bool>& visited) const
	{
		// mark the visited node
		visited[startIdx] = true;

		// recursively visit any valid connected nodes that were not visited before
		for(T_ConnectionType* connection : m_pGraph->GetNodeConnections(startIdx))
		{
			if(m_pGraph->IsNodeValid(connection->GetTo()) && !visited[connection->GetTo()]) 
			{
				VisitAllNodesDFS(connection->GetTo(), visited);
			}
		}
	}

	template<class T_NodeType, class T_ConnectionType>
	inline bool EulerianPath<T_NodeType, T_ConnectionType>::IsConnected() const
	{
		int nrOfNodes = m_pGraph->GetNrOfNodes();
		vector<bool> visited(nrOfNodes, false);

		// find a valid starting node that has connections
		int connectedIndex{ invalid_node_index };
		for (int i{}; i < nrOfNodes; i++)
		{
			if(m_pGraph->IsNodeValid(i))
			{
				if (m_pGraph->GetNodeConnections(i).size() != 0)
				{
					connectedIndex = i;
					break;
				}
				else
					return false;
			}
		}

		// if no valid node could be found, return false
		if(connectedIndex == invalid_node_index)
		{
			return false;
		}

		// start a depth-first-search traversal from a node that has connections
		VisitAllNodesDFS(connectedIndex, visited);

		// if a node was never visited, this graph is not connected
		for (int i{}; i < nrOfNodes; i++)
		{
			if (m_pGraph->IsNodeValid(i) && visited[i] == false)
				return false;
		}

		return true;
	}

}