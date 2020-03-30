#include "mcpch.h"
#include "FlatWorldGenerator.h"

namespace Minecraft
{

	WorldChunk FlatWorldGenerator::GenerateChunk(const ChunkPos_t& position)
	{
		WorldChunk result;
		result.SetLevel(0, BlockId::Stone);
		result.SetLevel(1, BlockId::Stone);
		result.SetLevel(2, BlockId::Stone);
		result.SetLevel(3, BlockId::Dirt);
		result.SetLevel(4, BlockId::Dirt);
		result.SetLevel(5, BlockId::Dirt);
		result.SetLevel(6, BlockId::Grass);
		return result;
	}

}
