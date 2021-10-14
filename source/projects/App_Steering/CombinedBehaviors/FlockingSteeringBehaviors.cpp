#include "stdafx.h"
#include "FlockingSteeringBehaviors.h"
#include "../SteeringAgent.h"
#include "../SteeringHelpers.h"
#include "Flock.h"

//*********************
//SEPARATION (FLOCKING)
Seperation::Seperation(Flock* flock)
	: m_pFlock{ flock } 
{}

Seperation::~Seperation() 
{
	m_pFlock = nullptr;
}

SteeringOutput Seperation::CalculateSteering(float deltaT, SteeringAgent* pAgent) 
{
	SteeringOutput steering = {};

	if (m_pFlock != nullptr)
	{
		for (size_t i{}; i < m_pFlock->GetNrOfNeighbors(); i++)
		{
			Elite::Vector2 flee{pAgent->GetPosition() - m_pFlock->GetNeighbors()[i]->GetPosition() };
			flee.Normalize();
			flee *= pAgent->GetMaxLinearSpeed();
			float distance{ Elite::Distance(pAgent->GetPosition(), m_pFlock->GetNeighbors()[i]->GetPosition()) };
			steering.LinearVelocity += (flee / distance);
		}
	}
	//steering.LinearVelocity.Normalize(); // Normalize desired velocity
	//steering.LinearVelocity *= pAgent->GetMaxLinearSpeed(); // Rescale to max speed

	return steering;
}

//*******************
//COHESION (FLOCKING)
Cohesion::Cohesion(Flock* flock)
	: m_pFlock{ flock }
{}

Cohesion::~Cohesion()
{
	m_pFlock = nullptr;
}

SteeringOutput Cohesion::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	pAgent->SetAutoOrient(true);
	SteeringOutput steering = {};
	
	if (m_pFlock != nullptr)
	{
		steering.LinearVelocity = m_pFlock->GetAverageNeighborPos() - pAgent->GetPosition();
	}
	
	steering.LinearVelocity.Normalize(); // Normalize desired velocity
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed(); // Rescale to max speed

	return steering;
}

//*************************
//VELOCITY MATCH (FLOCKING)
VelocityMatch::VelocityMatch(Flock* flock)
	: m_pFlock{ flock }
{}

VelocityMatch::~VelocityMatch()
{
	m_pFlock = nullptr;
}

SteeringOutput VelocityMatch::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	if (m_pFlock != nullptr)
	{
		steering.LinearVelocity = m_pFlock->GetAverageNeighborVelocity() - pAgent->GetPosition(); // Desired velocity;
		steering.LinearVelocity.Normalize(); // Normalize desired velocity
		steering.LinearVelocity *= pAgent->GetMaxLinearSpeed(); // Rescale to max speed
	}

	return steering;
}
