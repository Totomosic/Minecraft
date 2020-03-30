#pragma once
#include "Chunk.h"
#include "World/Generators/WorldGenerator.h"

namespace Minecraft
{

	class World
	{
	private:
		std::unique_ptr<WorldGenerator> m_Generator;
		mutable std::unordered_map<ChunkPos_t, WorldChunk> m_LoadedChunks;

	private:
		struct BlockIndex
		{
		public:
			ChunkPos_t Chunk;
			BlockPos_t Offset;
		};

	public:
		World();
		World(std::unique_ptr<WorldGenerator>&& generator);

		std::vector<ChunkPos_t> GetLoadedChunks() const;

		bool IsChunkLoaded(const ChunkPos_t& position) const;
		const WorldChunk& LoadChunk(const ChunkPos_t& position) const;
		void UnloadChunk(const ChunkPos_t& position) const;
		const WorldChunk* GetChunk(const ChunkPos_t& position) const;
		ChunkNeighbours GetChunkNeighbours(const ChunkPos_t& position) const;

		BlockId GetBlock(const BlockPos_t& position) const;
		void SetBlock(const BlockPos_t& position, BlockId block);

		ChunkPos_t GetChunkFromBlock(const BlockPos_t& position) const;

	private:
		BlockIndex GetBlockIndex(const BlockPos_t& position) const;
		WorldChunk& LoadChunkInternal(const ChunkPos_t& position) const;
	};

}