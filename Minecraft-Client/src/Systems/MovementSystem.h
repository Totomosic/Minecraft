#pragma once
#include "mcpch.h"
#include "../World/World.h"

namespace Minecraft
{

	class MovementSystem : public System<MovementSystem>
	{
	private:
		const World* m_World;

	public:
		MovementSystem(const World* world);
		virtual void Update(EntityManager& entities, TimeDelta dt) override;
	};

}