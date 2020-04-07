#include "mcpch.h"
#include "World.h"

#include "WorldEvents.h"

namespace Minecraft
{

	World::World()
		: m_Generator(), m_LoadedChunks()
	{
	}

	World::World(std::unique_ptr<WorldGenerator>&& generator)
		: m_Generator(std::move(generator)), m_LoadedChunks()
	{
	}

	std::vector<ChunkPos_t> World::GetLoadedChunks() const
	{
		std::vector<ChunkPos_t> result;
		result.reserve(m_LoadedChunks.size());
		for (const auto& pair : m_LoadedChunks)
		{
			result.push_back(pair.first);
		}
		return result;
	}

	bool World::IsChunkLoaded(const ChunkPos_t& position) const
	{
		return m_LoadedChunks.find(position) != m_LoadedChunks.end();
	}

	const WorldChunk& World::LoadChunk(const ChunkPos_t& position) const
	{
		return (const WorldChunk&)LoadChunkInternal(position);
	}

	void World::UnloadChunk(const ChunkPos_t& position) const
	{
		if (m_LoadedChunks.find(position) != m_LoadedChunks.end())
		{
			m_LoadedChunks.erase(position);
			EventManager::Get().Bus().Emit(ChunkUnloaded{ *this, position });
		}
	}

	const WorldChunk* World::GetChunk(const ChunkPos_t& position) const
	{
		if (m_LoadedChunks.find(position) != m_LoadedChunks.end())
		{
			return &m_LoadedChunks[position];
		}
		return nullptr;
	}

	ChunkNeighbours World::GetChunkNeighbours(const ChunkPos_t& position) const
	{
		ChunkNeighbours result;
		result.nx = GetChunk({ position.x - 1, position.y });
		result.px = GetChunk({ position.x + 1, position.y });
		result.nz = GetChunk({ position.x, position.y - 1 });
		result.pz = GetChunk({ position.x, position.y + 1 });
		return result;
	}

	BlockId World::GetBlock(const BlockPos_t& position) const
	{
		BlockIndex index = GetBlockIndex(position);
		const WorldChunk& chunk = LoadChunk(index.Chunk);
		return chunk.GetBlock(index.Offset);
	}

	void World::SetBlock(const BlockPos_t& position, BlockId block)
	{
		BlockIndex index = GetBlockIndex(position);
		WorldChunk& chunk = LoadChunkInternal(index.Chunk);
		chunk.SetBlock(index.Offset, block);
		EventManager::Get().Bus().Emit(ChunkUpdated{ *this, index.Chunk, (const WorldChunk&)chunk });
	}

	BlockPos_t World::GetBlockFromWorld(const Vector3f& worldPosition) const
	{
		return BlockPos_t{ (dimension_t)floor(worldPosition.x), (dimension_t)floor(worldPosition.y), (dimension_t)floor(worldPosition.z) };
	}

	ChunkPos_t World::GetChunkFromBlock(const BlockPos_t& position) const
	{
		return GetBlockIndex(position).Chunk;
	}

	World::BlockIndex World::GetBlockIndex(const BlockPos_t& position) const
	{
		BlockIndex result;

		double x = position.x / (double)WorldChunk::CHUNK_SIZE;
		double z = position.z / (double)WorldChunk::CHUNK_SIZE;
		result.Chunk.x = (int)floor(x);
		result.Chunk.y = (int)floor(z);
		result.Offset.x = position.x - result.Chunk.x * WorldChunk::CHUNK_SIZE;
		result.Offset.y = position.y;
		result.Offset.z = position.z - result.Chunk.y * WorldChunk::CHUNK_SIZE;
		return result;
	}

	WorldChunk& World::LoadChunkInternal(const ChunkPos_t& position) const
	{
		if (m_LoadedChunks.find(position) == m_LoadedChunks.end())
		{
			m_LoadedChunks[position] = m_Generator->GenerateChunk(position);
			const WorldChunk& chunk = m_LoadedChunks[position];
			EventManager::Get().Bus().Emit(ChunkLoaded{ *this, position, chunk });
		}
		return m_LoadedChunks[position];
	}

}
