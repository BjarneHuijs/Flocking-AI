#ifndef AGARIO_GAME_APPLICATION_H
#define AGARIO_GAME_APPLICATION_H
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "framework/EliteInterfaces/EIApp.h"
#include "framework\EliteAI\EliteGraphs\EInfluenceMap.h"
#include "framework\EliteAI\EliteGraphs\EGraph2D.h"
#include "framework\EliteAI\EliteGraphs\EGridGraph.h"
#include "framework\EliteAI\EliteGraphs\EliteGraphUtilities\EGraphRenderer.h"

class AgarioFood;
class AgarioAgent;
class AgarioContactListener;
class NavigationColliderElement;

class App_AgarioGame_IMap final : public IApp
{
public:
	App_AgarioGame_IMap();
	~App_AgarioGame_IMap();

	void Start() override;
	void Update(float deltaTime) override;
	void Render(float deltaTime) const override;
private:
	float m_TrimWorldSize = 70.f;
	const int m_AmountOfAgents{ 20 };
	std::vector<AgarioAgent*> m_pAgentVec{};

	AgarioAgent* m_pUberAgent = nullptr;

	const int m_AmountOfFood{ 40 };
	const float m_FoodSpawnDelay{ 2.f };
	float m_TimeSinceLastFoodSpawn{ 0.f };
	std::vector<AgarioFood*> m_pFoodVec{};

	AgarioContactListener* m_pContactListener = nullptr;
	bool m_GameOver = false;

	//--Level--
	std::vector<NavigationColliderElement*> m_vNavigationColliders = {};
private:	
	template<class T_AgarioType>
	void UpdateAgarioEntities(vector<T_AgarioType*>& entities, float deltaTime, bool useWayPointGraph);

	Elite::Blackboard* CreateBlackboard(AgarioAgent* a);
	void UpdateImGui();

	using InfluenceGrid = Elite::GridGraph<Elite::InfluenceNode, Elite::GraphConnection>;
	using InfluenceGraph = Elite::Graph2D<Elite::InfluenceNode, Elite::GraphConnection2D>;
private:
	Elite::InfluenceMap<InfluenceGrid>* m_pInfluenceGrid = nullptr;
	Elite::InfluenceMap<InfluenceGraph>* m_pInfluenceGraph2D = nullptr;
	Elite::EGraphRenderer m_GraphRenderer{};

	bool m_RenderIMap = false;
	bool m_RenderAsGraph = false;

	//C++ make the class non-copyable
	App_AgarioGame_IMap(const App_AgarioGame_IMap&) {};
	App_AgarioGame_IMap& operator=(const App_AgarioGame_IMap&) {};
};

#endif

template<class T_AgarioType>
inline void App_AgarioGame_IMap::UpdateAgarioEntities(vector<T_AgarioType*>& entities, float deltaTime, bool negativeInfluence)
{

	for (auto& e : entities)
	{
		e->Update(deltaTime);
		
		float influence{e->GetRadius()};

		if (negativeInfluence) 
		{
			influence *= -1;
		}
		
		/*if (useWayPointGraph)
			m_pInfluenceGraph2D->SetInfluenceAtPosition(e->GetPosition(), e->GetRadius());
		else*/
		m_pInfluenceGrid->SetInfluenceAtPosition(e->GetPosition(), influence * 50);

		//Trimming Disabled
		/*auto agent = dynamic_cast<AgarioAgent*>(e);
		if (agent)
			agent->TrimToWorld(m_TrimWorldSize);*/

		if (e->CanBeDestroyed())
			SAFE_DELETE(e);
	}

	auto toRemoveEntityIt = std::remove_if(entities.begin(), entities.end(),
		[](T_AgarioType* e) {return e == nullptr; });
	if (toRemoveEntityIt != entities.end())
	{
		entities.erase(toRemoveEntityIt, entities.end());
	}
}
