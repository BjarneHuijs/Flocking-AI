#pragma once
#include "../SteeringHelpers.h"

class ISteeringBehavior;
class SteeringAgent;
class BlendedSteering;
class PrioritySteering;
class Cohesion;
class Seperation;
class VelocityMatch;
class Seek;
class Evade;
class Flee;
class Wander;
class CellSpace;

class Flock
{
public:
	Flock(
		int flockSize = 50, 
		float worldSize = 100.f, 
		SteeringAgent* pAgentToEvade = nullptr, 
		bool trimWorld = false);

	~Flock();

	void Update(float deltaT);
	void UpdateAndRenderUI();
	void Render(float deltaT);

	void RegisterNeighbors(SteeringAgent* pAgent);
	int GetNrOfNeighbors() const { return m_NrOfNeighbors; }
	const vector<SteeringAgent*>& GetNeighbors() const { return m_Neighbors; }

	Elite::Vector2 GetAverageNeighborPos() const;
	Elite::Vector2 GetAverageNeighborVelocity() const;

private:
	// flock agents
	int m_FlockSize = 0;
	vector<SteeringAgent*> m_Agents;

	// neighborhood agents
	vector<SteeringAgent*> m_Neighbors;
	float m_NeighborhoodRadius = 10.f;
	int m_NrOfNeighbors = 0;

	// evade target
	SteeringAgent* m_pAgentToEvade = nullptr;

	// world info
	bool m_TrimWorld = false;
	float m_WorldSize = 0.f;
	CellSpace* m_pCellSpace = nullptr;
	
	// steering Behaviors
	BlendedSteering* m_pBlendedSteering = nullptr;
	PrioritySteering* m_pPrioritySteering = nullptr;
	Seek* m_pSeek = nullptr;
	Wander* m_pWander = nullptr;
	Flee* m_pEvade = nullptr;
	Cohesion* m_pCohesion = nullptr;
	Seperation* m_pSeperation = nullptr;
	VelocityMatch* m_pVelocityMatch = nullptr;

	// private functions
	float* GetWeight(ISteeringBehavior* pBehaviour);

	bool m_CanDebugRender = false;
	TargetData m_MouseTarget{};
	std::vector<Elite::Vector2> m_OldPositions;
	bool m_Partitioning = true;

private:
	Flock(const Flock& other);
	Flock& operator=(const Flock& other);

};