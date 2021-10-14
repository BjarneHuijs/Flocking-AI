#pragma once
#include "../SteeringBehaviors.h"
#include <vector>

class Flock;

//SEPARATION - FLOCKING
//*********************
class Seperation : public Flee
{
public:
	Seperation(Flock* flock);
	~Seperation();

	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

private:
	Flock* m_pFlock;
};

//COHESION - FLOCKING
//*******************
class Cohesion : public ISteeringBehavior
{
public:
	Cohesion(Flock* flock);
	~Cohesion();

	//void SetTarget(const TargetData& pTarget) override {};

	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

private:
	Flock* m_pFlock;
};

//VELOCITY MATCH - FLOCKING
//************************
class VelocityMatch : public ISteeringBehavior
{
public:
	VelocityMatch(Flock* flock);
	~VelocityMatch();

	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

private:
	Flock* m_pFlock;
};