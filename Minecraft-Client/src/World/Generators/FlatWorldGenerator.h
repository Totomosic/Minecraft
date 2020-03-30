#pragma once
#include "WorldGenerator.h"

namespace Minecraft
{

	class FlatWorldGenerator : public WorldGenerator
	{
	public:
		WorldChunk GenerateChunk(const ChunkPos_t& position) override;
	};

}