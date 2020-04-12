#pragma once
#include "World.h"

namespace Minecraft
{

	struct RaycastHit
	{
	public:
		bool Hit;
		BlockPos_t BlockPosition;
		Minecraft::BlockId BlockId;
		BlockPos_t Normal;
		float Distance;
	};

	class Raycaster
	{
	private:
		const World* m_World;

	public:
		Raycaster(const World* world);
		
		RaycastHit Cast(const Ray& ray, float maxDistance = 10) const;
	};

}