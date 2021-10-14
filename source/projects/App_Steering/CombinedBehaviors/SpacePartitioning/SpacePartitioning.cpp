#include "stdafx.h"
#include "SpacePartitioning.h"
#include "projects\App_Steering\SteeringAgent.h"

// --- Cell ---
// ------------
Cell::Cell(float left, float bottom, float width, float height)
{
	boundingBox.bottomLeft = { left, bottom };
	boundingBox.width = width;
	boundingBox.height = height;
}

std::vector<Elite::Vector2> Cell::GetRectPoints() const
{
	auto left = boundingBox.bottomLeft.x;
	auto bottom = boundingBox.bottomLeft.y;
	auto width = boundingBox.width;
	auto height = boundingBox.height;

	std::vector<Elite::Vector2> rectPoints =
	{
		{ left , bottom  },
		{ left , bottom + height  },
		{ left + width , bottom + height },
		{ left + width , bottom  },
	};

	return rectPoints;
}

// --- Partitioned Space ---
// -------------------------
CellSpace::CellSpace(float width, float height, int rows, int cols, int maxEntities)
	: m_SpaceWidth(width)
	, m_SpaceHeight(height)
	, m_NrOfRows(rows)
	, m_NrOfCols(cols)
	, m_CellWidth{ width / rows }
	, m_CellHeight{ height / cols }
	, m_Neighbors(maxEntities)
	, m_NrOfNeighbors(0)
{
	float halfWorldWidth{ m_SpaceWidth / 2 };
	float halfWorldHeight{ m_SpaceHeight / 2 };

	for (int r{}; r < m_NrOfRows; r++)
	{
		for (int c{}; c < m_NrOfCols; c++)
		{
			Cell cell{ 0.f + (c * m_CellWidth) - halfWorldWidth, 0.f + (r * m_CellHeight) - halfWorldHeight, m_CellWidth, m_CellHeight };
			m_Cells.push_back(cell);
		}
	}
}

void CellSpace::AddAgent(SteeringAgent* agent)
{
	int index{ PositionToIndex(agent->GetPosition()) };
	m_Cells[index].agents.push_back(agent);
}

void CellSpace::UpdateAgentCell(SteeringAgent* agent, const Elite::Vector2& oldPos)
{
	int currentCell{ PositionToIndex(oldPos) };
	int newCell{ PositionToIndex(agent->GetPosition()) };

	if(newCell != currentCell) 
	{
		m_Cells[newCell].agents.push_back(agent);
		m_Cells[currentCell].agents.remove(agent);
	}
}

void CellSpace::RegisterNeighbors(const Elite::Vector2& pos, float queryRadius)
{
	m_NrOfNeighbors = 0;


	int cellIndex{ PositionToIndex(pos) };
	Elite::Rect neighbourhoodRect{ {pos.x - queryRadius, pos.y - queryRadius}, queryRadius * 2, queryRadius * 2 };

	int minCellindex{ PositionToIndex({pos.x - queryRadius, pos.y - queryRadius}) };
	int maxCellRowindex{ PositionToIndex({pos.x + queryRadius, pos.y - queryRadius}) };
	int maxCellindex{ PositionToIndex({pos.x + queryRadius, pos.y + queryRadius}) };
	int cellsPerRow{ maxCellRowindex - minCellindex };
	std::vector<Elite::Vector2> vertices;

	for (int i{minCellindex}; i < maxCellindex; i++)
	{
		if (Elite::IsOverlapping(m_Cells[i].boundingBox, neighbourhoodRect)) 
		{
			for (SteeringAgent* pAgent : m_Cells[i].agents)
			{
				if (pAgent->GetPosition() != pos) 
				{					
					if(Elite::DistanceSquared(pos, pAgent->GetPosition()) < (queryRadius * queryRadius))
					{
						m_Neighbors[m_NrOfNeighbors] = pAgent;
						m_NrOfNeighbors++;
					}
				}
			}
		}
		int diff{ i - minCellindex };
		if(diff > cellsPerRow - 1)
		{
			minCellindex += (m_NrOfCols);
			i = minCellindex -1;
			//i += (m_NrOfCols - cellsPerRow);
			//maxCellRowindex += (m_NrOfCols);
		}
	}

}

void CellSpace::RenderCells(const Elite::Vector2& pos, float queryRadius) const
{
	std::vector<Elite::Vector2> vertices;
	int nrOfcells{ m_NrOfRows * m_NrOfRows };
	Elite::Rect neighbourhoodRect{ {pos.x - queryRadius, pos.y - queryRadius}, queryRadius * 2, queryRadius * 2 };

	for (size_t i{}; i < nrOfcells; i++)
	{

		//vertices = cell.GetRectPoints();
		vertices = m_Cells[i].GetRectPoints();
		if (Elite::IsOverlapping(neighbourhoodRect, m_Cells[i].boundingBox))
		{
			for (SteeringAgent* agent : m_Cells[i].agents)
			{
				DEBUGRENDERER2D->DrawSolidCircle(agent->GetPosition(), 1.f, { 0.f,0.f }, { 0.f,0.f,1.f }, -0.8f);
			}
			DEBUGRENDERER2D->DrawPolygon(&vertices[0], 4, { 0.f, 0.f, 1.f }, 0);
		}
		else 
		{
			//vertices = m_Cells[i].GetRectPoints();
			DEBUGRENDERER2D->DrawPolygon(&vertices[0], 4, { 1.f,0.f ,0.f }, 0);
		}
		//DEBUGRENDERER2D->DrawString({ cell.boundingBox.bottomLeft.x, cell.boundingBox.bottomLeft.y + 5.f }, std::to_string(cell.agents.size()).c_str());
		DEBUGRENDERER2D->DrawString({ m_Cells[i].boundingBox.bottomLeft.x, m_Cells[i].boundingBox.bottomLeft.y + 5.f }, std::to_string(m_Cells[i].agents.size()).c_str());
		//DEBUGRENDERER2D->DrawString({ m_Cells[i].boundingBox.bottomLeft.x, m_Cells[i].boundingBox.bottomLeft.y + 5.f }, std::to_string(i).c_str());

		

		for (SteeringAgent* agent : m_Cells[i].agents) 
		{
			if (agent->CanRenderBehavior()) 
			{
				//const Elite::Vector2& pos{ agent->GetPosition() };
				Cell c{ neighbourhoodRect.bottomLeft.x, neighbourhoodRect.bottomLeft.y, neighbourhoodRect.width, neighbourhoodRect.height };
				vertices = c.GetRectPoints();

				DEBUGRENDERER2D->DrawPolygon(&vertices[0], 4, { 0.f, 1.f ,0.f }, 0);				
			}
		}
	}
	
	//for (size_t i{}; i < m_NrOfNeighbors; i++)
	//{
	//	/*Elite::Polygon* poly{ new Elite::Polygon(c.GetRectPoints()) };
	//	DEBUGRENDERER2D->DrawSolidPolygon(poly, { 0.f, 0.f, 0.5f });
	//	delete poly;*/

	//	DEBUGRENDERER2D->DrawSolidCircle(m_Neighbors[i]->GetPosition(), 1.f, { 0.f,0.f }, { 0.f,0.f,1.f }, -0.8f);
	//}
}

int CellSpace::PositionToIndex(const Elite::Vector2 pos) const
{
	//works with all, but not optimized
	/*int index{};
	for (size_t i{}; i < m_NrOfRows * m_NrOfCols; i++)
	{
		Elite::Rect cell{ m_Cells[i].boundingBox };
		if (pos.x >= cell.bottomLeft.x && pos.y >= cell.bottomLeft.y) 
		{
			if (pos.x <= cell.bottomLeft.x + cell.width && pos.y <= cell.bottomLeft.y + cell.height)
			{
				index = i;
			}
		}		
	}

	return index;*/

	//assumes world is square
	//assumes cells are square
	int column{ int(pos.x + m_SpaceWidth / 2) / int(m_CellWidth) };
	int row{ int(pos.y + m_SpaceHeight / 2) / int(m_CellHeight) };
	int index{};

	column = Elite::Clamp(column, 0, m_NrOfCols - 1);
	row = Elite::Clamp(row, 0, m_NrOfRows - 1);

	index = row * m_NrOfCols + column;
	return index;
}