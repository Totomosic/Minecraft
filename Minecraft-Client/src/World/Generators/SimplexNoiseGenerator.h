#pragma once
#include "WorldGenerator.h"

namespace Minecraft
{

	class SimplexNoiseGenerator : public WorldGenerator
	{
	private:
		SimplexNoise m_Generator;
		int m_Octaves;
		int m_MinHeight;
		int m_MaxHeight;

	public:
		SimplexNoiseGenerator(int minHeight, int maxHeight);

		WorldChunk GenerateChunk(const ChunkPos_t& position);

	};

}