#pragma once
#include "Chunk.h"

namespace Minecraft
{

	class World;

	struct ChunkLoaded
	{
	public:
		const World& Dimesion;
		ChunkPos_t Position;
		const WorldChunk& Chunk;
	};

	struct ChunkUnloaded
	{
	public:
		const World& Dimension;
		ChunkPos_t Position;
	};

	struct ChunkUpdated
	{
	public:
		const World& Dimesion;
		ChunkPos_t Position;
		const WorldChunk& Chunk;
	};

}