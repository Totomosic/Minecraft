#include "mcpch.h"
#include "MovementSystem.h"
#include "CMovement.h"
#include "CDimensions.h"

namespace Minecraft
{

	MovementSystem::MovementSystem(const World* world)
		: m_World(world)
	{
	}

	void MovementSystem::Update(EntityManager& entities, TimeDelta dt)
	{
		for (EntityHandle entity : entities.GetEntitiesWith<Transform, CMovement>())
		{
			ComponentHandle transform = entity.GetTransform();
			ComponentHandle movement = entity.GetComponent<CMovement>();

			movement->Velocity += movement->Acceleration * dt.Seconds();
			if (movement->Velocity.MagnitudeSqrd() > 1e-9)
			{
				if (entity.HasComponent<CDimensions>())
				{
					Vector3f size = entity.GetComponent<CDimensions>()->Size / 2.0f;
					Vector3f move = movement->Velocity * dt.Seconds();
					Vector3f nextPos = transform->Position() + move;

					int blockHeight = (int)std::ceil(size.y * 2.0f);

					// Collide XZ direction with bottom block of entity
					for (int height = 0; height < blockHeight && (movement->Velocity.x != 0 && movement->Velocity.z != 0); height++)
					{
						BlockPos_t current = m_World->GetBlockFromWorld(transform->Position());
						BlockPos_t xBlock0 = m_World->GetBlockFromWorld(transform->Position() + Vector3f{ move.x + size.x * Sign(move.x), (float)height, size.z });
						BlockPos_t xBlock1 = m_World->GetBlockFromWorld(transform->Position() + Vector3f{ move.x + size.x * Sign(move.x), (float)height, -size.z });
						BlockPos_t zBlock0 = m_World->GetBlockFromWorld(transform->Position() + Vector3f{ size.x, (float)height, move.z + size.z * Sign(move.z) });
						BlockPos_t zBlock1 = m_World->GetBlockFromWorld(transform->Position() + Vector3f{ -size.x, (float)height, move.z + size.z * Sign(move.z) });
						if (m_World->GetBlock(xBlock0) != BlockId::Air || m_World->GetBlock(xBlock1) != BlockId::Air)
							movement->Velocity.x = 0;
						if (m_World->GetBlock(zBlock0) != BlockId::Air || m_World->GetBlock(zBlock1) != BlockId::Air)
							movement->Velocity.z = 0;
					}

					move = movement->Velocity * dt.Seconds();
					nextPos = transform->Position() + move;

					// Collide Y direction with bottom block of entity
					BlockPos_t nextBL = m_World->GetBlockFromWorld(nextPos + Vector3f{ -size.x, 0, -size.z });
					BlockPos_t nextBR = m_World->GetBlockFromWorld(nextPos + Vector3f{ size.x, 0, -size.z });
					BlockPos_t nextTL = m_World->GetBlockFromWorld(nextPos + Vector3f{ -size.x, 0, size.z });
					BlockPos_t nextTR = m_World->GetBlockFromWorld(nextPos + Vector3f{ size.x, 0, size.z });
					if (m_World->GetBlock(nextBL) != BlockId::Air || m_World->GetBlock(nextBR) != BlockId::Air || m_World->GetBlock(nextTL) != BlockId::Air || m_World->GetBlock(nextTR) != BlockId::Air)
					{
						movement->Velocity.y = 0;
					}
					else
					{
						BlockPos_t nextBL = m_World->GetBlockFromWorld(nextPos + Vector3f{ -size.x, size.y * 2.0f, -size.z });
						BlockPos_t nextBR = m_World->GetBlockFromWorld(nextPos + Vector3f{ size.x, size.y * 2.0f, -size.z });
						BlockPos_t nextTL = m_World->GetBlockFromWorld(nextPos + Vector3f{ -size.x, size.y * 2.0f, size.z });
						BlockPos_t nextTR = m_World->GetBlockFromWorld(nextPos + Vector3f{ size.x, size.y * 2.0f, size.z });
						if (m_World->GetBlock(nextBL) != BlockId::Air || m_World->GetBlock(nextBR) != BlockId::Air || m_World->GetBlock(nextTL) != BlockId::Air || m_World->GetBlock(nextTR) != BlockId::Air)
						{
							movement->Velocity.y = 0;
						}
					}
				}
				transform->Translate(movement->Velocity * dt.Seconds());
			}
		}
	}

}
