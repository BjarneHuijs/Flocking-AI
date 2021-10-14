#include "stdafx.h"
#include "Flock.h"

#include "../SteeringAgent.h"
#include "../SteeringBehaviors.h"
#include "CombinedSteeringBehaviors.h"
#include "FlockingSteeringBehaviors.h"
#include "SpacePartitioning/SpacePartitioning.h"

using namespace Elite;

//Constructor & Destructor
Flock::Flock(
	int flockSize /*= 50*/, 
	float worldSize /*= 100.f*/, 
	SteeringAgent* pAgentToEvade /*= nullptr*/, 
	bool trimWorld /*= false*/)

	: m_WorldSize{ worldSize }
	, m_FlockSize{ flockSize }
	, m_TrimWorld { trimWorld }
	, m_pAgentToEvade{ pAgentToEvade }
	, m_NeighborhoodRadius{ 5.f }
	, m_NrOfNeighbors{ 0 }
{
	m_Agents = std::vector<SteeringAgent*>{};
	m_Neighbors = std::vector<SteeringAgent*>(flockSize);

	m_pEvade = new Flee();
	m_pCohesion = new Cohesion(this);
	m_pSeperation = new Seperation(this);
	m_pVelocityMatch = new VelocityMatch(this);
	m_pSeek = new Seek();
	m_pWander = new Wander();
	m_pCellSpace = new CellSpace(2 * worldSize, 2 * worldSize, 25, 25, 100);

	m_pBlendedSteering = new BlendedSteering({ { m_pCohesion, 0.5f }, { m_pSeperation, 0.5f }, { m_pVelocityMatch, 0.5f }, { m_pSeek, 0.f }, { m_pWander, 0.2f } });
	m_pPrioritySteering = new PrioritySteering({m_pEvade, m_pBlendedSteering});
	

	m_pAgentToEvade = new SteeringAgent();
	m_pAgentToEvade->SetBodyColor({ 1.f, 0.f, 0.f });
	m_pAgentToEvade->SetSteeringBehavior(m_pSeek);
	m_pAgentToEvade->SetMaxLinearSpeed(m_pAgentToEvade->GetMaxLinearSpeed() * 0.9f);
	

	m_OldPositions = std::vector<Elite::Vector2>(m_FlockSize);
	for (int i{}; i < flockSize; i++) {
		m_Agents.push_back(new SteeringAgent());

		m_Agents[i]->SetPosition(Elite::randomVector2() * m_WorldSize);
		m_Agents[i]->SetSteeringBehavior(m_pPrioritySteering);
		//m_Agents[i]->SetMaxLinearSpeed(m_Agents[i]->GetMaxLinearSpeed() * 1.2f);
		m_pCellSpace->AddAgent(m_Agents[i]);

		m_OldPositions[i] = m_Agents[i]->GetPosition();

	}

}

Flock::~Flock()
{
	// delete cell space
	SAFE_DELETE(m_pCellSpace)

	// delete flock
	for(size_t i{}; i < m_FlockSize; i++)
	{
		SAFE_DELETE(m_Agents[i]);
	}
	SAFE_DELETE(m_pPrioritySteering);
	SAFE_DELETE(m_pBlendedSteering);
	SAFE_DELETE(m_pWander);
	SAFE_DELETE(m_pEvade);
	SAFE_DELETE(m_pCohesion);
	SAFE_DELETE(m_pSeperation);
	SAFE_DELETE(m_pVelocityMatch);

	// delete agent to evade
	SAFE_DELETE(m_pAgentToEvade);
	SAFE_DELETE(m_pSeek);


}

void Flock::Update(float deltaT)
{
	UpdateAndRenderUI();
	// loop over all the boids

	if (INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eLeft))
	{
		auto const mouseData = INPUTMANAGER->GetMouseData(InputType::eMouseButton, InputMouseButton::eLeft);
		m_MouseTarget.Position = DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld({ static_cast<float>(mouseData.X), static_cast<float>(mouseData.Y) });
	}
	
	m_pSeek->SetTarget(m_MouseTarget);
	m_pAgentToEvade->Update(deltaT);
	m_pAgentToEvade->TrimToWorld(m_WorldSize);

	TargetData evadeTarget;
	evadeTarget.LinearVelocity = m_pAgentToEvade->GetLinearVelocity();
	evadeTarget.Position = m_pAgentToEvade->GetPosition();
	m_pEvade->SetTarget(evadeTarget);

	for (size_t i{}; i < m_FlockSize; i++)
	{
		// register its neighbors
		if (m_Partitioning) 
		{
			m_pCellSpace->RegisterNeighbors(m_Agents[i]->GetPosition(), m_NeighborhoodRadius);
			m_pCellSpace->UpdateAgentCell(m_Agents[i], m_OldPositions[i]);
			m_Neighbors = m_pCellSpace->GetNeighbors();
			m_NrOfNeighbors = m_pCellSpace->GetNrOfNeighbors();
			m_OldPositions[i] = m_Agents[i]->GetPosition();

		} else 
		{
			RegisterNeighbors(m_Agents[i]);
		}
		/*if (i == 0)
		{
			std::cout << "cells: " << m_pCellSpace->GetNrOfNeighbors() << std::endl;
			std::cout << "normal: " << m_NrOfNeighbors << std::endl;
		}*/
		
		// update it
		//agent->GetSteeringBehavior()->SetTarget(m_MouseTarget);
		m_Agents[i]->Update(deltaT);		
		
		// trim it to the world
		m_Agents[i]->TrimToWorld(m_WorldSize);
	}

	m_pAgentToEvade->SetRenderBehavior(m_CanDebugRender);
	m_Agents[0]->SetRenderBehavior(m_CanDebugRender);
}

void Flock::Render(float deltaT)
{
	if (m_Partitioning && m_CanDebugRender) 
	{
		m_Agents[0]->SetBodyColor({ 0.f, 1.f, 0.f });
		DEBUGRENDERER2D->DrawCircle(m_Agents[0]->GetPosition(), m_NeighborhoodRadius, { 1.f, 1.f, 0.f }, 0.4f);
		m_pCellSpace->RenderCells(m_Agents[0]->GetPosition(), m_NeighborhoodRadius);
	}

	//m_pAgentToEvade->Render(deltaT);
	DEBUGRENDERER2D->DrawSolidCircle(m_pAgentToEvade->GetPosition(), 1.f, { 0.f,0.f }, { 1.f,0.f,0.f }, -0.8f);

	//for (SteeringAgent* agent : m_Agents)
	//{
	//	agent->Render(deltaT);
	//	//agent->SetRenderBehavior(m_CanDebugRender);
	//}

	//vector<Elite::Vector2> points =
	//{
	//	{ -m_WorldSize,m_WorldSize },
	//	{ m_WorldSize,m_WorldSize },
	//	{ m_WorldSize,-m_WorldSize },
	//	{-m_WorldSize,-m_WorldSize }
	//};
	//DEBUGRENDERER2D->DrawPolygon(&points[0], 4, { 1,0,0,1 }, 0.4f);

	//DEBUG RENDERING
	if (!m_Partitioning && m_CanDebugRender)
	{
		m_Agents[0]->SetBodyColor({ 0.f, 1.f, 0.f });
		DEBUGRENDERER2D->DrawCircle(m_Agents[0]->GetPosition(), m_NeighborhoodRadius, { 1.f, 1.f, 0.f }, 0.4f);
		RegisterNeighbors(m_Agents[0]);
		for (size_t i{}; i < m_NrOfNeighbors; i++)
		{
			DEBUGRENDERER2D->DrawSolidCircle(m_Neighbors[i]->GetPosition(), 1.f, { 0.f,0.f }, { 0.f,0.f,1.f }, -0.8f);
		}
	}
}

void Flock::UpdateAndRenderUI()
{
	//Setup
	int menuWidth = 235;
	int const width = DEBUGRENDERER2D->GetActiveCamera()->GetWidth();
	int const height = DEBUGRENDERER2D->GetActiveCamera()->GetHeight();
	bool windowActive = true;
	ImGui::SetNextWindowPos(ImVec2((float)width - menuWidth - 10, 10));
	ImGui::SetNextWindowSize(ImVec2((float)menuWidth, (float)height - 20));
	ImGui::Begin("Gameplay Programming", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	ImGui::PushAllowKeyboardFocus(false);

	//Elements
	ImGui::Text("CONTROLS");
	ImGui::Indent();
	ImGui::Text("LMB: place target");
	ImGui::Text("RMB: move cam.");
	ImGui::Text("Scrollwheel: zoom cam.");
	ImGui::Unindent();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Text("STATS");
	ImGui::Indent();
	ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
	ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
	ImGui::Unindent();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Text("Flocking");
	ImGui::Spacing();

	// Implement checkboxes and sliders here
	ImGui::Checkbox("Partitioning", &m_Partitioning);
	ImGui::Spacing();
	ImGui::Checkbox("Debug Rendering", &m_CanDebugRender);
	/*ImGui::Checkbox("Trim World", &m_TrimWorld);
	if (m_TrimWorld)
	{
		ImGui::SliderFloat("Trim Size", &m_WorldSize, 0.f, 200.f, "%.1");
	}*/
	ImGui::Spacing();
	ImGui::Text("Behavior Weights");
	ImGui::Spacing();
	ImGui::SliderFloat("Cohesion", &m_pBlendedSteering->m_WeightedBehaviors[0].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Seperation", &m_pBlendedSteering->m_WeightedBehaviors[1].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Velocity Match", &m_pBlendedSteering->m_WeightedBehaviors[2].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Seek", &m_pBlendedSteering->m_WeightedBehaviors[3].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("wander", &m_pBlendedSteering->m_WeightedBehaviors[4].weight, 0.f, 1.f, "%.2");
	//ImGui::SliderFloat("",)

	//End
	ImGui::PopAllowKeyboardFocus();
	ImGui::End();
	
}

void Flock::RegisterNeighbors(SteeringAgent* pAgent)
{
	// register the agents neighboring the currently evaluated agent
	// store how many they are, so you know which part of the vector to loop over

	m_NrOfNeighbors = 0;;
	for (int i{}; i < m_FlockSize; i++)
	{
		//Elite::Vector2 distance{ pAgent->GetPosition() - m_Agents[i]->GetPosition() };
		//const float distance{ Elite::Distance(pAgent->GetPosition(), m_Agents[i]->GetPosition()) };
		//if( distance.x < m_NeighborhoodRadius && distance.x > -m_NeighborhoodRadius && distance.y < m_NeighborhoodRadius && distance.y > -m_NeighborhoodRadius)

		//if(m_Agents[i] != pAgent && distance < m_NeighborhoodRadius)
		if(m_Agents[i] != pAgent && Elite::DistanceSquared(m_Agents[i]->GetPosition(), pAgent->GetPosition()) < (m_NeighborhoodRadius * m_NeighborhoodRadius))
		{
			m_Neighbors[m_NrOfNeighbors] = m_Agents[i];
			m_NrOfNeighbors++;
		}
	}

}

Elite::Vector2 Flock::GetAverageNeighborPos() const
{
	Elite::Vector2 avgNeighborPos{};

	/*if (m_Partitioning) 
	{
		int nrOfNeighbors{ m_pCellSpace->GetNrOfNeighbors() };

		if (nrOfNeighbors == 0) return avgNeighborPos;

		for (size_t i{}; i < nrOfNeighbors; i++)
		{
			if (m_pCellSpace->GetNeighbors()[i] != nullptr)
				avgNeighborPos += m_pCellSpace->GetNeighbors()[i]->GetPosition();
		}

		avgNeighborPos /= nrOfNeighbors;

	}else*/
	{
		if (m_NrOfNeighbors == 0) return avgNeighborPos;

		for (size_t i{}; i < m_NrOfNeighbors; i++)
		{
			if (m_Neighbors[i] != nullptr)
				avgNeighborPos += m_Neighbors[i]->GetPosition();
		}

		avgNeighborPos /= float(m_NrOfNeighbors);
	}

	return avgNeighborPos;

}

Elite::Vector2 Flock::GetAverageNeighborVelocity() const
{
	Elite::Vector2 avgNeighborVelocity{};

	/*if (m_Partitioning) 
	{
		int nrOfNeighbors{ m_pCellSpace->GetNrOfNeighbors() };

		if (nrOfNeighbors == 0) return avgNeighborVelocity;

		for (size_t i{}; i < nrOfNeighbors; i++)
		{
			avgNeighborVelocity += m_pCellSpace->GetNeighbors()[i]->GetLinearVelocity();
		}
		avgNeighborVelocity /= nrOfNeighbors;

	}else */
	{

		if (m_NrOfNeighbors == 0) return avgNeighborVelocity;

		for (size_t i{}; i < m_NrOfNeighbors; i++)
		{
			avgNeighborVelocity += m_Neighbors[i]->GetLinearVelocity();
		}
		avgNeighborVelocity /= float(m_NrOfNeighbors);

	}
	return avgNeighborVelocity;

}


float* Flock::GetWeight(ISteeringBehavior* pBehavior) 
{
	if (m_pBlendedSteering)
	{
		auto& weightedBehaviors = m_pBlendedSteering->m_WeightedBehaviors;
		auto it = find_if(weightedBehaviors.begin(),
			weightedBehaviors.end(),
			[pBehavior](BlendedSteering::WeightedBehavior el)
			{
				return el.pBehavior == pBehavior;
			}
		);

		if(it!= weightedBehaviors.end())
			return &it->weight;
	}

	return nullptr;
}
