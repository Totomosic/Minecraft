#include "mcpch.h"
#include "SimplexNoiseGenerator.h"

namespace Minecraft
{

	SimplexNoiseGenerator::SimplexNoiseGenerator(int minHeight, int maxHeight)
		: m_Generator(), m_Octaves(8), m_MinHeight(minHeight), m_MaxHeight(maxHeight)
	{
	}

	WorldChunk SimplexNoiseGenerator::GenerateChunk(const ChunkPos_t& position)
	{
		WorldChunk chunk;
		dimension_t startX = position.x * WorldChunk::CHUNK_SIZE;
		dimension_t startZ = position.y * WorldChunk::CHUNK_SIZE;
		for (dimension_t x = 0; x < WorldChunk::CHUNK_SIZE; x++)
		{
			for (dimension_t z = 0; z < WorldChunk::CHUNK_SIZE; z++)
			{
				float scaling = 1.0f / 64.0f;
				int height = (int)(Map<float>(m_Generator.Generate(m_Octaves, (x + startX) * scaling, (z + startZ) * scaling), -1.0f, 1.0f, m_MinHeight, m_MaxHeight));
				for (int i = 0; i < height - 4; i++)
				{
					chunk.SetBlock({ x, i, z }, BlockId::Stone);
				}
				chunk.SetBlock({ x, height - 4, z }, BlockId::Dirt);
				chunk.SetBlock({ x, height - 3, z }, BlockId::Dirt);
				chunk.SetBlock({ x, height - 2, z }, BlockId::Dirt);
				chunk.SetBlock({ x, height - 1, z }, BlockId::Grass);
			}
		}
		return chunk;
	}

}
