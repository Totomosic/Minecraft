#pragma once
#include "World.h"

namespace Minecraft
{

	class WorldRenderer
	{
	private:
		struct PendingModel
		{
		public:
			Model model;
			EntityHandle entity;
		};

	private:
		const World* m_TargetWorld;
		Layer* m_Layer;
		std::unordered_map<ChunkPos_t, EntityHandle> m_Entities;
		std::queue<PendingModel> m_ModelQueue;
		float m_ModelsPerFrame;
		float m_TimeSinceLastModel;

		ScopedEventListener m_LoadedListener;
		ScopedEventListener m_UnloadedListener;
		ScopedEventListener m_UpdatedListener;

	public:
		WorldRenderer(const World* targetWorld, Layer* layer);

		void Update();

	private:
		Task<void> CreateMeshFromFaces(std::vector<BlockRenderableFace> faces, const VertexBuffer* buffer, const IndexBuffer* indexBuffer) const;
		void UpdateChunk(const ChunkPos_t& position, const WorldChunk* chunk);
		Model CreateChunkModel(const std::vector<BlockRenderableFace>& faces) const;
		void LoadModel(const EntityHandle& entity, Model&& model);
		void CleanupModel(const Model& model);

	};

}