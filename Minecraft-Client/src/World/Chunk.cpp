#include "mcpch.h"
#include "Chunk.h"

#include "../Blocks/BlockDatabase.h"

namespace Minecraft
{

	WorldChunk::WorldChunk()
		: m_Blocks()
	{
		memset(m_Blocks, (int)BlockId::Air, sizeof(m_Blocks));
	}

	BlockId WorldChunk::GetBlock(const BlockPos_t& position) const
	{
		if (IsValidPosition(position))
		{
			return m_Blocks[GetIndex(position)];
		}
		return BlockId::Air;
	}

	BlockId WorldChunk::GetBlock(const BlockPos_t& position, const ChunkNeighbours& neighbours, BlockId defaultBlock) const
	{
		if (IsValidPosition(position))
		{
			return GetBlock(position);
		}
		if (position.x < 0 && neighbours.nx != nullptr)
		{
			return neighbours.nx->GetBlock({ CHUNK_SIZE + position.x, position.y, position.z });
		}
		if (position.x >= CHUNK_SIZE && neighbours.px != nullptr)
		{
			return neighbours.px->GetBlock({ position.x - CHUNK_SIZE, position.y, position.z });
		}
		if (position.z < 0 && neighbours.nz != nullptr)
		{
			return neighbours.nz->GetBlock({ position.x, position.y, CHUNK_SIZE + position.z });
		}
		if (position.z >= CHUNK_SIZE && neighbours.pz != nullptr)
		{
			return neighbours.pz->GetBlock({ position.x, position.y, position.z - CHUNK_SIZE });
		}
		return defaultBlock;
	}

	void WorldChunk::SetBlock(const BlockPos_t& position, BlockId block)
	{
		if (IsValidPosition(position))
		{
			m_Blocks[GetIndex(position)] = block;
		}
	}

	void WorldChunk::SetLevel(dimension_t y, BlockId block)
	{
		if (y >= 0 && y < CHUNK_HEIGHT)
		{
			memset(m_Blocks + y * CHUNK_SIZE * CHUNK_SIZE, (int)block, sizeof(BlockId) * CHUNK_SIZE * CHUNK_SIZE);
		}
	}

	std::vector<BlockRenderableFace> WorldChunk::GetVisibleFaces(const BlockPos_t& referencePosition, const ChunkNeighbours& neighbours) const
	{
		std::vector<BlockRenderableFace> result;
		for (dimension_t y = 0; y < CHUNK_HEIGHT; y++)
		{
			for (dimension_t z = 0; z < CHUNK_SIZE; z++)
			{
				for (dimension_t x = 0; x < CHUNK_SIZE; x++)
				{
					BlockPos_t position = { x, y, z };
					BlockId block = GetBlock(position);
					if (block != BlockId::Air)
					{
						BlockId nx = GetBlock({ x - 1, y, z }, neighbours, BlockId::Stone);
						BlockId px = GetBlock({ x + 1, y, z }, neighbours, BlockId::Stone);
						BlockId ny = GetBlock({ x, y - 1, z }, neighbours, BlockId::Stone);
						BlockId py = GetBlock({ x, y + 1, z }, neighbours, BlockId::Stone);
						BlockId nz = GetBlock({ x, y, z - 1 }, neighbours, BlockId::Stone);
						BlockId pz = GetBlock({ x, y, z + 1 }, neighbours, BlockId::Stone);
						const BlockData& data = BlockDatabase::GetBlock(block);
						if (nx == BlockId::Air)
						{
							BlockRenderableFace face;
							face.Normal = { -1, 0, 0 };
							face.Texture = data.Textures.nx;
							face.TopLeft = referencePosition + BlockPos_t{ x, y + 1, z };
							face.BottomLeft = referencePosition + BlockPos_t{ x, y, z };
							face.BottomRight = referencePosition + BlockPos_t{ x, y, z + 1 };
							face.TopRight = referencePosition + BlockPos_t{ x, y + 1, z + 1 };
							result.push_back(face);
						}
						if (px == BlockId::Air)
						{
							BlockRenderableFace face;
							face.Normal = { 1, 0, 0 };
							face.Texture = data.Textures.px;
							face.TopLeft = referencePosition + BlockPos_t{ x + 1, y + 1, z + 1 };
							face.BottomLeft = referencePosition + BlockPos_t{ x + 1, y, z + 1 };
							face.BottomRight = referencePosition + BlockPos_t{ x + 1, y, z };
							face.TopRight = referencePosition + BlockPos_t{ x + 1, y + 1, z };
							result.push_back(face);
						}
						if (ny == BlockId::Air)
						{
							BlockRenderableFace face;
							face.Normal = { 0, -1, 0 };
							face.Texture = data.Textures.ny;
							face.TopLeft = referencePosition + BlockPos_t{ x, y, z + 1 };
							face.BottomLeft = referencePosition + BlockPos_t{ x, y, z };
							face.BottomRight = referencePosition + BlockPos_t{ x + 1, y, z };
							face.TopRight = referencePosition + BlockPos_t{ x + 1, y, z + 1 };
							result.push_back(face);
						}
						if (py == BlockId::Air)
						{
							BlockRenderableFace face;
							face.Normal = { 0, 1, 0 };
							face.Texture = data.Textures.py;
							face.TopLeft = referencePosition + BlockPos_t{ x, y + 1, z };
							face.BottomLeft = referencePosition + BlockPos_t{ x, y + 1, z + 1 };
							face.BottomRight = referencePosition + BlockPos_t{ x + 1, y + 1, z + 1 };
							face.TopRight = referencePosition + BlockPos_t{ x + 1, y + 1, z };
							result.push_back(face);
						}
						if (nz == BlockId::Air)
						{
							BlockRenderableFace face;
							face.Normal = { 0, 0, -1 };
							face.Texture = data.Textures.nz;
							face.TopLeft = referencePosition + BlockPos_t{ x + 1, y + 1, z };
							face.BottomLeft = referencePosition + BlockPos_t{ x + 1, y, z };
							face.BottomRight = referencePosition + BlockPos_t{ x, y, z };
							face.TopRight = referencePosition + BlockPos_t{ x, y + 1, z };
							result.push_back(face);
						}
						if (pz == BlockId::Air)
						{
							BlockRenderableFace face;
							face.Normal = { 0, 0, 1 };
							face.Texture = data.Textures.pz;
							face.TopLeft = referencePosition + BlockPos_t{ x, y + 1, z + 1 };
							face.BottomLeft = referencePosition + BlockPos_t{ x, y, z + 1 };
							face.BottomRight = referencePosition + BlockPos_t{ x + 1, y, z + 1 };
							face.TopRight = referencePosition + BlockPos_t{ x + 1, y + 1, z + 1 };
							result.push_back(face);
						}
					}
				}
			}
		}
		return result;
	}

	bool WorldChunk::IsValidPosition(const BlockPos_t& position) const
	{
		return position.x >= 0 && position.x < CHUNK_SIZE && position.z >= 0 && position.z < CHUNK_SIZE && position.y >= 0 && position.y < CHUNK_HEIGHT;
	}

	size_t WorldChunk::GetIndex(const BlockPos_t& position) const
	{
		// Stacked vertically
		return position.y * CHUNK_SIZE * CHUNK_SIZE + position.z * CHUNK_SIZE + position.x;
	}

}
