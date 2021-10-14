//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "SteeringBehaviors.h"
#include "SteeringAgent.h"

//SEEK
//****

SteeringOutput Seek::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	pAgent->SetAutoOrient(true);
	SteeringOutput steering = {};
	steering.LinearVelocity = (m_Target).Position - pAgent->GetPosition(); // Desired velocity
	steering.LinearVelocity.Normalize(); // Normalize desired velocity
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed(); // Rescale to max speed
	//DEBUG RENDERING
	if (pAgent->CanRenderBehavior())
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0, 1, 0 }, 0.4f);
	
	return steering;
}

//FLEE
//****

SteeringOutput Flee::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	pAgent->SetAutoOrient(true);
	/*SteeringOutput fleeing = { Seek::CalculateSteering(deltaT, pAgent) };
	fleeing.LinearVelocity *= -1;*/
	
	auto distance{ Distance(pAgent->GetPosition(), m_Target.Position) };
	if (distance > m_FleeRadius)
	{
		SteeringOutput steering;
		steering.IsValid = false;
		return steering;
	}
	
	SteeringOutput steering = {};

	steering.LinearVelocity = ((m_Target).Position - pAgent->GetPosition()); // Desired velocity;
	//steering.LinearVelocity = (pAgent->GetPosition() - (m_Target).Position); same result
	steering.LinearVelocity.Normalize(); // Normalize desired velocity
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed(); // Rescale to max speed
	steering.LinearVelocity *= -1; // Reverse speed

	//DEBUG RENDERING
	if (pAgent->CanRenderBehavior())
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0, 1, 0 }, 0.4f);


	return steering;
}

//ARRIVE
//****

SteeringOutput Arrive::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	pAgent->SetAutoOrient(true);
	
	const float slowRadius{ 10.f };
	const float distance{ Distance(m_Target.Position, pAgent->GetPosition()) };

	SteeringOutput steering = {};
	

	//if((m_Target).Position.Distance(pAgent->GetPosition()) < slowRadius)
	if(distance < slowRadius)
	{
		steering.LinearVelocity = (m_Target).Position - pAgent->GetPosition(); // Desired velocity;
		steering.LinearVelocity.Normalize(); // Normalize desired velocity
		steering.LinearVelocity *= pAgent->GetMaxLinearSpeed(); // Rescale to max speed
		steering.LinearVelocity *= (distance / slowRadius); // Scale down velocity to stop at target
	}
	else 
	{
		steering.LinearVelocity = (m_Target).Position - pAgent->GetPosition(); // Desired velocity;
		steering.LinearVelocity.Normalize(); // Normalize desired velocity
		steering.LinearVelocity *= pAgent->GetMaxLinearSpeed(); // Rescale to max speed
	}

	//DEBUG RENDERING
	if (pAgent->CanRenderBehavior())
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0, 1, 0 }, 0.4f);

	return steering;
}

void Arrive::SetTargetRadius(float radius) 
{
	m_TargetRadius = radius;
}

void Arrive::SetSlowRadius(float radius)
{
	m_SlowRadius = radius;
}

//FACE
//****

SteeringOutput Face::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	/*old version
	pAgent->SetAutoOrient(false);
	SteeringOutput steering = {};
	
	float currentAngle = pAgent->GetOrientation();
	steering.LinearVelocity = (m_Target).Position - pAgent->GetPosition(); // Desired velocity
	float targetAngle = GetOrientationFromVelocity(steering.LinearVelocity);
	
	steering.LinearVelocity *= 0; // Rescale to max speed
	
	float degTargetAngle = (ToDegrees(targetAngle));
	float degCurrentAngle = (ToDegrees(currentAngle));
	Clamp(degCurrentAngle, 0.f, 359.f);

	float rotationdiff = degTargetAngle - degCurrentAngle;

	if(rotationdiff > 0.1f)
		steering.AngularVelocity += rotationdiff;
		
	//DEBUG RENDERING
	if (pAgent->CanRenderBehavior())
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0, 1, 0 }, 0.4f);

	return steering;*/

	//Nillo version
	SteeringOutput steering{};

	Elite::Vector2 toTarget{ pAgent->GetPosition() - m_Target.Position };

	float targetAngle{ atan2(toTarget.y, toTarget.x) + float(E_PI_2) };
	float desiredAngle{ targetAngle - pAgent->GetOrientation() };
	if (desiredAngle > E_PI) {
		desiredAngle -= E_PI;
	}
	else
	{
		desiredAngle += E_PI;
	}

	steering.AngularVelocity = desiredAngle / float(E_PI) / 2.f * pAgent->GetMaxAngularSpeed();

	pAgent->SetAutoOrient(false);

	//Debug rendering
	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, steering.LinearVelocity.Magnitude(), { 0.f, 1.f, 0.f, 0.5f }, 0.4f);
	}

	return steering;
		
}

//WANDER (base> SEEK)
//******
SteeringOutput Wander::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	pAgent->SetAutoOrient(true);
	SteeringOutput steering = {};

	//SetWanderOffset(6.f);
	//SetWanderRadius(4.f);
		
	Elite::Vector2 targetCenter{ pAgent->GetPosition() + pAgent->GetDirection() * m_Offset };

	const float deltaAngle = m_AngleChange * (randomFloat(2.f) - 1.f);
	m_WanderAngle += deltaAngle;

	Elite::Vector2 targetVector{ cos(m_WanderAngle) * m_Radius, sin(m_WanderAngle) * m_Radius };

	m_Target.Position = { targetCenter.x + targetVector.x, targetCenter.y + targetVector.y };

	steering = Seek::CalculateSteering(deltaT, pAgent);

	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0, 1, 0 }, 0.4f);
		//DEBUGRENDERER2D->DrawCircle(targetCenter, m_Radius, { 1, 0, 0 }, 0.4f);
	}
	
	return steering;
}

//EVADE
//****
SteeringOutput Evade::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	pAgent->SetAutoOrient(true);

	const float predictionRadius{ 4.f };

	auto distance{ Distance(pAgent->GetPosition(), m_Target.Position) };
	if (distance > m_FleeRadius) 
	{
		SteeringOutput steering;
		steering.IsValid = false;
		return steering;
	}	
	float offset{ distance / (pAgent->GetMaxLinearSpeed() * 0.9f) };
	Elite::Vector2 targetCenter{ m_Target.Position + m_Target.LinearVelocity * offset };

	m_Target.Position = targetCenter;

	SteeringOutput steering = {};
	//steering = Flee::CalculateSteering(deltaT, pAgent);

	steering.LinearVelocity = -1 * ((m_Target).Position - pAgent->GetPosition()); // Desired velocity;
	//steering.LinearVelocity = (pAgent->GetPosition() - (m_Target).Position); same result
	steering.LinearVelocity.Normalize(); // Normalize desired velocity
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed(); // Rescale to max speed

	//steering.LinearVelocity *= 0.9f;

	//steering.LinearVelocity = -1 * (targetCenter - pAgent->GetPosition()); // Desired velocity;
	//steering.LinearVelocity.Normalize(); // Normalize desired velocity
	//steering.LinearVelocity *= pAgent->GetMaxLinearSpeed() * 0.9f; // Rescale to max speed
	

	//DEBUG RENDERING
	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0, 1, 0 }, 0.4f);
		DEBUGRENDERER2D->DrawCircle(targetCenter, predictionRadius, { 1, 0, 0 }, 0.4f);
	}

	return steering;
}

//PURSUIT
//****

SteeringOutput Pursuit::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	pAgent->SetAutoOrient(true);

	const float predictionRadius{ 4.f };

	float distance{ Distance(m_Target.Position, pAgent->GetPosition()) };
	float offset{ distance / (pAgent->GetMaxLinearSpeed() * 0.9f) };

	Elite::Vector2 targetCenter{ m_Target.Position + m_Target.LinearVelocity * offset };
	m_Target.Position = targetCenter;

	SteeringOutput steering = {};	
	steering = Seek::CalculateSteering(deltaT, pAgent);
	steering.LinearVelocity *= 0.9f;

	//DEBUG RENDERING
	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0, 1, 0 }, 0.4f);
		DEBUGRENDERER2D->DrawCircle(targetCenter, predictionRadius, { 1, 0, 0 }, 0.4f);
	}

	return steering;
}