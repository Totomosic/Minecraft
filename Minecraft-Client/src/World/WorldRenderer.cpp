#include "mcpch.h"
#include "WorldRenderer.h"

#include "../Blocks/BlockDatabase.h"
#include "WorldEvents.h"

namespace Minecraft
{

	WorldRenderer::WorldRenderer(const World* targetWorld, Layer* layer)
		: m_TargetWorld(targetWorld), m_Layer(layer), m_LoadedListener(), m_UnloadedListener(), m_UpdatedListener(), m_ModelQueue(), m_ModelsPerFrame(2), m_TimeSinceLastModel(1.0f / m_ModelsPerFrame)
	{
		m_LoadedListener = EventManager::Get().Bus().AddScopedEventListener<ChunkLoaded>([this](Event<ChunkLoaded>& e)
			{
				if (&e.Data.Dimension == m_TargetWorld && m_Entities.find(e.Data.Position) == m_Entities.end())
				{
					ChunkPos_t position = e.Data.Position;
					ChunkNeighbours neighbours = e.Data.Dimension.GetChunkNeighbours(e.Data.Position);
					float x = e.Data.Position.x * WorldChunk::CHUNK_SIZE;
					float z = e.Data.Position.y * WorldChunk::CHUNK_SIZE;
					std::vector<BlockRenderableFace> faces = e.Data.Chunk.GetVisibleFaces({}, neighbours);
					Model model = CreateChunkModel(faces);
					const VertexBuffer* vb = &model.Meshes[0].Mesh->Data().Vertices->GetVertexBuffer(0);
					const IndexBuffer* ib = model.Meshes[0].Mesh->Data().Indices->GetIndexBuffer(0).get();
					CreateMeshFromFaces(faces, vb, ib).ContinueWithOnMainThread([this, x, z, position, model{ std::move(model) }]() mutable
						{
							if (m_Entities.find(position) != m_Entities.end() && !m_Entities[position])
							{
								EntityHandle entity = m_Layer->GetFactory().CreateTransform(Transform({ x, 0, z }));
								LoadModel(entity, std::move(model));
								m_Entities[position] = entity;
							}
							else
							{
								CleanupModel(model);
							}
						});
					m_Entities[position];
					UpdateChunk({ position.x - 1, position.y }, neighbours.nx);
					UpdateChunk({ position.x + 1, position.y }, neighbours.px);
					UpdateChunk({ position.x, position.y - 1 }, neighbours.nz);
					UpdateChunk({ position.x, position.y + 1 }, neighbours.pz);
				}
			});
		m_UnloadedListener = EventManager::Get().Bus().AddScopedEventListener<ChunkUnloaded>([this](Event<ChunkUnloaded>& e)
			{
				if (&e.Data.Dimension == m_TargetWorld && m_Entities.find(e.Data.Position) != m_Entities.end())
				{
					EntityHandle entity = m_Entities[e.Data.Position];
					if (entity)
					{
						entity.Destroy();
					}
					m_Entities.erase(e.Data.Position);
				}
			});
		m_UpdatedListener = EventManager::Get().Bus().AddScopedEventListener<ChunkUpdated>([this](Event<ChunkUpdated>& e)
			{
				if (&e.Data.Dimension == m_TargetWorld && m_Entities.find(e.Data.Position) != m_Entities.end())
				{
					UpdateChunk(e.Data.Position, &e.Data.Chunk);
					for (const auto& neighbour : e.Data.Neighbours)
					{
						UpdateChunk(neighbour.first, neighbour.second);
					}
				}
			});
	}

	void WorldRenderer::Update()
	{
		float timePerMesh = 1.0f / m_ModelsPerFrame;
		m_TimeSinceLastModel = std::min(m_TimeSinceLastModel + 1, std::max(timePerMesh, m_ModelsPerFrame));
		while (!m_ModelQueue.empty() && m_TimeSinceLastModel >= timePerMesh)
		{
			PendingModel& pair = m_ModelQueue.front();
			CleanupModel(pair.model);
			pair.entity.Assign<Model>(std::move(pair.model));
			m_ModelQueue.pop();
			m_TimeSinceLastModel -= timePerMesh;
		}
	}

	Task<void> WorldRenderer::CreateMeshFromFaces(std::vector<BlockRenderableFace> faces, const VertexBuffer* buffer, const IndexBuffer* indexBuffer) const
	{		
		void* vertexPtr = buffer->Map(Access::Write);
		void* indexPtr = indexBuffer->Map(Access::Write);
		return TaskManager::Get().Run(make_shared_function([faces{ std::move(faces) }, buffer, indexBuffer, vertexPtr, indexPtr]() mutable
		{
			Vector4<byte> color = Color::White.ToBytes();
			DefaultVertexIterator it(vertexPtr, &buffer->Layout());
			IndexIterator<uint32_t> indices((uint32_t*)indexPtr);
			for (int i = 0; i < faces.size(); i++)
			{
				const BlockRenderableFace& face = faces.at(i);
				it.Position() = face.TopLeft;
				it.Normal() = face.Normal;
				it.TexCoord() = Vector2f(face.Texture.Min.x, face.Texture.Max.y);
				it.Color() = color;
				it++;
				it.Position() = face.BottomLeft;
				it.Normal() = face.Normal;
				it.TexCoord() = face.Texture.Min;
				it.Color() = color;
				it++;
				it.Position() = face.BottomRight;
				it.Normal() = face.Normal;
				it.TexCoord() = Vector2f(face.Texture.Max.x, face.Texture.Min.y);
				it.Color() = color;
				it++;
				it.Position() = face.TopRight;
				it.Normal() = face.Normal;
				it.TexCoord() = face.Texture.Max;
				it.Color() = color;
				it++;
				*indices = (uint32_t)i * 4 + 0;
				indices++;
				*indices = (uint32_t)i * 4 + 1;
				indices++;
				*indices = (uint32_t)i * 4 + 2;
				indices++;
				*indices = (uint32_t)i * 4 + 0;
				indices++;
				*indices = (uint32_t)i * 4 + 2;
				indices++;
				*indices = (uint32_t)i * 4 + 3;
				indices++;
			}
		}));
	}

	void WorldRenderer::UpdateChunk(const ChunkPos_t& position, const WorldChunk* chunk)
	{
		if (chunk != nullptr)
		{
			if (m_Entities.find(position) != m_Entities.end())
			{
				std::vector<BlockRenderableFace> faces = chunk->GetVisibleFaces({}, m_TargetWorld->GetChunkNeighbours(position));
				Model model = CreateChunkModel(faces);
				CreateMeshFromFaces(faces, &model.Meshes[0].Mesh->Data().Vertices->GetVertexBuffer(0), model.Meshes[0].Mesh->Data().Indices->GetIndexBuffer(0).get()).ContinueWithOnMainThread([this, position, model{ std::move(model) }]() mutable
					{
						if (m_Entities.find(position) != m_Entities.end())
						{
							EntityHandle entity = m_Entities[position];
							if (entity)
							{
								LoadModel(entity, std::move(model));
								return;
							}
						}
						CleanupModel(model);
					});
			}
		}
	}

	Model WorldRenderer::CreateChunkModel(const std::vector<BlockRenderableFace>& faces) const
	{
		MeshData meshData;
		meshData.Vertices = std::make_unique<VertexArray>();
		meshData.Indices = std::make_unique<IndexArray>();
		BufferLayout layout = BufferLayout::Default();
		meshData.Vertices->CreateVertexBuffer(faces.size() * 4 * layout.Size(), layout, BufferUsage::StaticDraw);
		meshData.Indices->AddIndexBuffer(std::make_unique<IndexBuffer>(faces.size() * 6, BufferUsage::StaticDraw));
		Model model;
		model.Materials.push_back(AssetManager::Get().Materials().Texture(BlockDatabase::GetBlockFaces()));
		model.Meshes.push_back({ AssetHandle<Mesh>(new Mesh(std::move(meshData), false), true), Matrix4f::Identity(), { 0 } });
		return model;
	}

	void WorldRenderer::LoadModel(const EntityHandle& entity, Model&& model)
	{
		m_ModelQueue.push({ std::move(model), entity });
	}

	void WorldRenderer::CleanupModel(const Model& model)
	{
		model.Meshes[0].Mesh->Data().Vertices->GetVertexBuffer(0).Unmap();
		model.Meshes[0].Mesh->Data().Indices->GetIndexBuffer(0)->Unmap();
	}

}
