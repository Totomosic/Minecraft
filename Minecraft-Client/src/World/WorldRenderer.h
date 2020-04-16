#pragma once
#include "World.h"

namespace Minecraft
{

	class WorldRenderer
	{
	private:
		struct PendingMesh
		{
		public:
			Mesh mesh;
			EntityHandle entity;
		};

	private:
		const World* m_TargetWorld;
		Layer* m_Layer;
		std::unordered_map<ChunkPos_t, EntityHandle> m_Entities;
		std::queue<PendingMesh> m_MeshQueue;
		float m_MeshesPerFrame;
		float m_TimeSinceLastMesh;

		ScopedEventListener m_LoadedListener;
		ScopedEventListener m_UnloadedListener;
		ScopedEventListener m_UpdatedListener;

	public:
		WorldRenderer(const World* targetWorld, Layer* layer);

		void Update();

	private:
		Task<void> CreateMeshFromFaces(std::vector<BlockRenderableFace> faces, const VertexBuffer* buffer, const IndexBuffer* indexBuffer) const;
		void UpdateChunk(const ChunkPos_t& position, const WorldChunk* chunk);
		Mesh CreateChunkMesh(const std::vector<BlockRenderableFace>& faces) const;
		void LoadMesh(const EntityHandle& entity, Mesh&& mesh);
		void CleanupMesh(const Mesh& mesh);

	};

}