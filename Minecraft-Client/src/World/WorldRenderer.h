#pragma once
#include "World.h"

namespace Minecraft
{

	class WorldRenderer
	{
	private:
		const World* m_TargetWorld;
		Layer* m_Layer;
		std::unordered_map<ChunkPos_t, EntityHandle> m_Entities;
		
		ScopedEventListener m_LoadedListener;
		ScopedEventListener m_UnloadedListener;
		ScopedEventListener m_UpdatedListener;

	public:
		WorldRenderer(const World* targetWorld, Layer* layer);

	private:
		Task<Mesh> CreateMeshFromFaces(std::vector<BlockFace> faces) const;
		void UpdateChunk(const ChunkPos_t& position, const WorldChunk* chunk);

	};

}