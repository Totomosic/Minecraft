#pragma once
#include "mcpch.h"
#include "../Blocks/Block.h"

namespace Minecraft
{

	class WorldChunk;

	struct ChunkNeighbours
	{
	public:
		const WorldChunk* nx = nullptr;
		const WorldChunk* px = nullptr;
		const WorldChunk* nz = nullptr;
		const WorldChunk* pz = nullptr;
	};

	class WorldChunk
	{
	public:
		static constexpr dimension_t CHUNK_SIZE = 16;
		static constexpr dimension_t CHUNK_HEIGHT = 256;

	private:
		BlockId m_Blocks[CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT];

	public:
		WorldChunk();

		BlockId GetBlock(const BlockPos_t& position) const;
		BlockId GetBlock(const BlockPos_t& position, const ChunkNeighbours& neighbours, BlockId defaultBlock = BlockId::Air) const;

		void SetBlock(const BlockPos_t& position, BlockId block);
		void SetLevel(dimension_t y, BlockId block);

		std::vector<BlockFace> GetVisibleFaces(const BlockPos_t& referencePosition, const ChunkNeighbours& neighbours) const;

	private:
		bool IsValidPosition(const BlockPos_t& position) const;
		size_t GetIndex(const BlockPos_t& position) const;
	};

}