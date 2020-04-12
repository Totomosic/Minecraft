#include "mcpch.h"
#include "Raycaster.h"

namespace Minecraft
{

	Raycaster::Raycaster(const World* world)
		: m_World(world)
	{
	}

	RaycastHit Raycaster::Cast(const Ray& ray, float maxDistance) const
	{
		RaycastHit result;
		result.Hit = false;
		if (m_World != nullptr)
		{
			Vector3f current = ray.Origin;
			float maxDirection = std::max(std::max(std::abs(ray.Direction.x), std::abs(ray.Direction.y)), std::abs(ray.Direction.z));
			if (maxDirection != 0)
			{
				float scale = 1.0f / maxDirection;
				scale = 0.1f;
				float distance = 0;
				while (distance <= maxDistance)
				{
					if (m_World->GetBlock(m_World->GetBlockFromWorld(current)) != BlockId::Air)
					{
						// Done
						result.Hit = true;
						result.BlockPosition = m_World->GetBlockFromWorld(current);
						result.BlockId = m_World->GetBlock(result.BlockPosition);
						result.Distance = distance;
						result.Normal = { 0, 0, 0 };
						while (distance >= 0)
						{
							current -= ray.Direction * scale;
							if (m_World->GetBlock(m_World->GetBlockFromWorld(current)) == BlockId::Air)
							{
								result.Normal = m_World->GetBlockFromWorld(current) - result.BlockPosition;
								break;
							}
						}
						return result;
					}
					current += ray.Direction * scale;
					distance += scale;
				}
			}
		}
		return result;
	}

}
