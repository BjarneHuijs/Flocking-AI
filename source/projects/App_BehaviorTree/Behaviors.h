/*=============================================================================*/
// Copyright 2020-2021 Elite Engine
/*=============================================================================*/
// Behaviors.h: Implementation of certain reusable behaviors for the BT version of the Agario Game
/*=============================================================================*/
#ifndef ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
#define ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "framework/EliteMath/EMath.h"
#include "framework/EliteAI/EliteDecisionMaking/EliteBehaviorTree/EBehaviorTree.h"
#include "../Shared/Agario/AgarioAgent.h"
#include "../Shared/Agario/AgarioFood.h"
#include "../App_Steering/SteeringBehaviors.h"

//-----------------------------------------------------------------
// Behaviors
//-----------------------------------------------------------------
bool IsCloseToFood(Elite::Blackboard* pBlackboard)
{
	AgarioAgent* pAgent = nullptr;
	std::vector<AgarioFood*>* foodVec = nullptr;

	auto dataAvailable = pBlackboard->GetData("Agent", pAgent) &&
		pBlackboard->GetData("FoodVec", foodVec);

	if (!pAgent || !foodVec)
		return false;

	//TODO: Check for food closeby and set target accordingly
	const float detectionRange{ 20.f };
	auto foodIt = std::find_if(foodVec->begin(), foodVec->end(), [&pAgent, &detectionRange](AgarioFood* food) {
		return Elite::DistanceSquared(pAgent->GetPosition(), food->GetPosition()) < Elite::Square(detectionRange);
		});

	if(foodIt != foodVec->end())
	{
		pBlackboard->ChangeData("Target", (*foodIt)->GetPosition());
		return true;
	}

	return false;
}



BehaviorState ChangeToWander(Elite::Blackboard* pBlackboard)
{
	AgarioAgent* pAgent = nullptr;
	auto dataAvailable = pBlackboard->GetData("Agent", pAgent);

	if (!pAgent)
		return Failure;

	pAgent->SetToWander();
	std::cout << "Wandering..." << std::endl;
	return Success;
}

BehaviorState ChangeToSeek(Elite::Blackboard* pBlackboard)
{
	AgarioAgent* pAgent = nullptr;
	Vector2 seekTarget{};
	auto dataAvailable = pBlackboard->GetData("Agent", pAgent) &&
		pBlackboard->GetData("Target", seekTarget);

	if (!pAgent)
		return Failure;
	
	//TODO: Implement Change to seek (Target)
	pAgent->SetToSeek(seekTarget);
	std::cout << "Seeking..." << std::endl;
	return Success;
}

bool IsCloseToBiggerAgent(Elite::Blackboard* pBlackboard)
{
	AgarioAgent* pAgent = nullptr;
	std::vector<AgarioAgent*>* agents = nullptr;

	auto dataAvailable = pBlackboard->GetData("Agent", pAgent) &&
		pBlackboard->GetData("AgentsVec", agents);

	if (!pAgent || !agents)
		return false;

	const float detectionRange{ pAgent->GetRadius() + 20.f };
	auto agentIt = std::find_if(agents->begin(), agents->end(), [&pAgent, &detectionRange](AgarioAgent* pOtherAgent) {
		return Elite::DistanceSquared(pAgent->GetPosition(), pOtherAgent->GetPosition()) < Elite::Square(detectionRange)
			&& pAgent->GetRadius() < pOtherAgent->GetRadius();
		});

	if (agentIt != agents->end())
	{
		pBlackboard->ChangeData("Target", (*agentIt)->GetPosition());
		return true;
	}

	return false;
}

BehaviorState ChangeToRun(Elite::Blackboard* pBlackboard)
{
	AgarioAgent* pAgent = nullptr;
	Vector2 fleetarget{};
	auto dataAvailable = pBlackboard->GetData("Agent", pAgent) &&
		pBlackboard->GetData("Target", fleetarget);

	//Elite::Vector2 vec{ (pAgent->GetPosition() - fleetarget) };
	//Elite::Vector2 fleeTarget = pAgent->GetPosition() + (vec * -1);

	if (!pAgent)
		return Failure;

	pAgent->SetToFlee(fleetarget);
	std::cout << "Fleeing..." << std::endl;
	return Success;
}


bool IsCloseToSmallerAgent(Elite::Blackboard* pBlackboard)
{
	AgarioAgent* pAgent = nullptr;
	std::vector<AgarioAgent*>* agents = nullptr;

	auto dataAvailable = pBlackboard->GetData("Agent", pAgent) &&
		pBlackboard->GetData("AgentsVec", agents);

	if (!pAgent || !agents)
		return false;

	//TODO: Check for food closeby and set target accordingly
	const float detectionRange{ pAgent->GetRadius() + 10.f };
	auto agentIt = std::find_if(agents->begin(), agents->end(), [&pAgent, &detectionRange](AgarioAgent* pOtherAgent) {
		return Elite::DistanceSquared(pAgent->GetPosition(), pOtherAgent->GetPosition()) < Elite::Square(detectionRange) 
			&& pAgent->GetRadius() > pOtherAgent->GetRadius() * 1.1f;
		});

	if (agentIt != agents->end())
	{
		pBlackboard->ChangeData("Target", (*agentIt)->GetPosition());
		return true;
	}

	return false;
}

BehaviorState ChangeToHunt(Elite::Blackboard* pBlackboard)
{
	AgarioAgent* pAgent = nullptr;
	Vector2 killTarget{};
	auto dataAvailable = pBlackboard->GetData("Agent", pAgent) &&
		pBlackboard->GetData("Target", killTarget);

	if (!pAgent)
		return Failure;

	pAgent->SetToSeek(killTarget);
	std::cout << "Hunting..." << std::endl;
	return Success;
}

#endif