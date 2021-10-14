/*=============================================================================*/
// Copyright 2020-2021 Elite Engine
/*=============================================================================*/
// StatesAndTransitions.h: Implementation of the state/transition classes
/*=============================================================================*/
#ifndef ELITE_APPLICATION_FSM_STATES_TRANSITIONS
#define ELITE_APPLICATION_FSM_STATES_TRANSITIONS

#include "../Shared/Agario/AgarioAgent.h"
#include "../Shared/Agario/AgarioFood.h"
#include "../App_Steering/SteeringBehaviors.h"

// STATES
//-------

// WanderState
class WanderState : public Elite::FSMState
{
public:
	WanderState() : FSMState() {};
	
	virtual void OnEnter(Blackboard* pBlackboard) override
	{
		AgarioAgent* pAgent = nullptr;
		bool dataAvailable = pBlackboard->GetData("Agent", pAgent);

		if (!dataAvailable) 
			return;

		if (pAgent == nullptr)
			return;

		pAgent->SetToWander();
	}

};

// SeekState
class SeekFoodState : public Elite::FSMState
{
public:
	SeekFoodState() : FSMState() {};
	
	virtual void OnEnter(Blackboard* pBlackboard) override
	{
		AgarioAgent* pAgent = nullptr;
		bool dataAvailable = pBlackboard->GetData("Agent", pAgent);
		if (!dataAvailable) 
			return;

		if (pAgent == nullptr)
			return;

		AgarioFood* pFood = nullptr;
		bool closestFood = pBlackboard->GetData("ClosestFood", pFood);
		
		if (!closestFood) 
			return;

		if (pFood == nullptr)
			return;

		pAgent->SetToSeek(pFood->GetPosition());
	}

};

// TRANSITIONS
//------------

// HasFoodCloseby
class HasFoodCloseby : public Elite::FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override
	{
		//get agent from blackboard
		AgarioAgent* pAgent = nullptr;
		bool agentAvailable = pBlackboard->GetData("Agent", pAgent);

		if (!agentAvailable)
			return false;

		if (pAgent == nullptr)
			return false;

		//get food info from blackboard
		std::vector<AgarioFood*>* pFoodVec = nullptr;
		bool foodAvailable = pBlackboard->GetData("FoodVector", pFoodVec);

		if (!foodAvailable)
			return false;

		if (pFoodVec == nullptr)
			return false;

		//check if food is close to agent (within radius)
		//false if food is too far
		float checkRadius{ pAgent->GetRadius() + m_CheckRadius };
		auto foodFindIt = std::find_if(pFoodVec->begin(), pFoodVec->end(), [&pAgent, &checkRadius](AgarioFood* pFood)
			{
				return Elite::DistanceSquared(pAgent->GetPosition(), pFood->GetPosition()) < Elite::Square(checkRadius);
			});
		
		if(foodFindIt != pFoodVec->end())
		{
			pBlackboard->ChangeData("ClosestFood", *foodFindIt);
			return true;
		}
		else 
		{
			return false;
		}		
	}

private:
	float m_CheckRadius{ 10.f };
};

// No food closeby
class HasNoFoodCloseby : public HasFoodCloseby
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override
	{
		return !HasFoodCloseby::ToTransition(pBlackboard);

		////get agent from blackboard
		//AgarioAgent* pAgent = nullptr;
		//bool agentAvailable = pBlackboard->GetData("Agent", pAgent);

		//if (!agentAvailable)
		//	return false;

		//if (pAgent == nullptr)
		//	return false;

		////get food info from blackboard
		//std::vector<AgarioFood*>* pFoodVec = nullptr;
		//bool foodAvailable = pBlackboard->GetData("FoodVector", pFoodVec);

		//if (!foodAvailable)
		//	return false;

		//if (pFoodVec == nullptr)
		//	return false;

		////check if food is close to agent (within radius)
		////false if food is too far
		//float checkRadius{ pAgent->GetRadius() + m_CheckRadius };
		//auto foodFindIt = std::find_if(pFoodVec->begin(), pFoodVec->end(), [&pAgent, &checkRadius](AgarioFood* pFood)
		//	{
		//		return Elite::DistanceSquared(pAgent->GetPosition(), pFood->GetPosition()) >= Elite::Square(checkRadius);
		//	});
		//
		//if(foodFindIt != pFoodVec->end())
		//{
		//	return true;
		//}
		//else 
		//{
		//	return false;
		//}
	}

private:
	float m_CheckRadius{ 10.f };
};

#endif