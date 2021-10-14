#include "stdafx.h"
#include "ENavGraph.h"
#include "framework\EliteAI\EliteGraphs\EliteGraphAlgorithms\EAStar.h"

using namespace Elite;

Elite::NavGraph::NavGraph(const Polygon& contourMesh, float playerRadius = 1.0f) :
	Graph2D(false),
	m_pNavMeshPolygon(nullptr)
{
	//Create the navigation mesh (polygon of navigatable area= Contour - Static Shapes)
	m_pNavMeshPolygon = new Polygon(contourMesh); // Create copy on heap

	//Get all shapes from all static rigidbodies with NavigationCollider flag
	auto vShapes = PHYSICSWORLD->GetAllStaticShapesInWorld(PhysicsFlags::NavigationCollider);

	//Store all children
	for (auto shape : vShapes)
	{
		shape.ExpandShape(playerRadius);
		m_pNavMeshPolygon->AddChild(shape);
	}

	//Triangulate
	m_pNavMeshPolygon->Triangulate();

	//Create the actual graph (nodes & connections) from the navigation mesh
	CreateNavigationGraph();
}

Elite::NavGraph::~NavGraph()
{
	delete m_pNavMeshPolygon; 
	m_pNavMeshPolygon = nullptr;
}

int Elite::NavGraph::GetNodeIdxFromLineIdx(int lineIdx) const
{
	auto nodeIt = std::find_if(m_Nodes.begin(), m_Nodes.end(), [lineIdx](const NavGraphNode* n) { return n->GetLineIndex() == lineIdx; });
	if (nodeIt != m_Nodes.end())
	{
		return (*nodeIt)->GetIndex();
	}

	return invalid_node_index;
}

Elite::Polygon* Elite::NavGraph::GetNavMeshPolygon() const
{
	return m_pNavMeshPolygon;
}

void Elite::NavGraph::CreateNavigationGraph()
{
	//1. Go over all the edges of the navigationmesh and create nodes
	for(Line* line : m_pNavMeshPolygon->GetLines())
	{
		auto triangles = m_pNavMeshPolygon->GetTrianglesFromLineIndex(line->index);
		if(triangles.size() > 1)
		{
			Vector2 pos{ ( (line->p1.x + line->p2.x) / 2 ), ( (line->p1.y + line->p2.y) / 2 ) };
			//m_Nodes.push_back(new NavGraphNode{ nodeIdx, line->index, pos });
			AddNode(new NavGraphNode{ GetNextFreeNodeIndex(), line->index, pos });
			
		}
	}
	
	//2. Create connections now that every node is created
	for(Triangle* triangle : m_pNavMeshPolygon->GetTriangles())
	{
		std::vector<NavGraphNode*> triangleNodes{};
		for(auto index : triangle->metaData.IndexLines)
		{

			for(auto* node : m_Nodes)
			{
				if(node->GetLineIndex() == index)
				{
					triangleNodes.push_back(node);
				}
			}
			
		}
		
		size_t nrOfNodes{ triangleNodes.size() };
		if(nrOfNodes == 2 )
		{
				
			std::list<GraphConnection2D*> connections{};

			AddConnection(new GraphConnection2D{ triangleNodes[0]->GetIndex(), triangleNodes[1]->GetIndex() });
		}
		else if (nrOfNodes == 3)
		{
			std::list<GraphConnection2D*> connections{};
			AddConnection(new GraphConnection2D{ triangleNodes[0]->GetIndex(), triangleNodes[1]->GetIndex() });
			AddConnection(new GraphConnection2D{ triangleNodes[1]->GetIndex(), triangleNodes[2]->GetIndex() });
			AddConnection(new GraphConnection2D{ triangleNodes[2]->GetIndex(), triangleNodes[0]->GetIndex() });
		}

	}

	//3. Set the connections cost to the actual distance
	SetConnectionCostsToDistance();

}

