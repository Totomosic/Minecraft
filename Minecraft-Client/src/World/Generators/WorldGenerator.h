#pragma once
#include "../Chunk.h"

namespace Minecraft
{

	class WorldGenerator
	{
	public:
		virtual ~WorldGenerator() {}
		virtual WorldChunk GenerateChunk(const ChunkPos_t& position) = 0;
	};

}