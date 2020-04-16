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
			BlockPos_t prevBlock = m_World->GetBlockFromWorld(current);
			float distance = 0;
			while (distance <= maxDistance)
			{
				BlockPos_t currentBlock = m_World->GetBlockFromWorld(current);
				if (m_World->GetBlock(currentBlock) != BlockId::Air)
				{
					// Done
					result.Hit = true;
					result.BlockPosition = m_World->GetBlockFromWorld(current);
					result.BlockId = m_World->GetBlock(result.BlockPosition);
					result.Distance = distance;
					result.Normal = prevBlock - currentBlock;
					return result;
				}
				static constexpr float minMove = 0.01f;
				float dx = std::max((Sign(ray.Direction.x) > 0) ? (std::floor(current.x) + 1 - current.x) : (current.x - std::floor(current.x)), minMove);
				float dy = std::max((Sign(ray.Direction.y) > 0) ? (std::floor(current.y) + 1 - current.y) : (current.y - std::floor(current.y)), minMove);
				float dz = std::max((Sign(ray.Direction.z) > 0) ? (std::floor(current.z) + 1 - current.z) : (current.z - std::floor(current.z)), minMove);
				float move = std::min({ dx, dy, dz });
				current += ray.Direction * move;
				distance += move;
				prevBlock = currentBlock;
			}
		}
		return result;
	}

}
