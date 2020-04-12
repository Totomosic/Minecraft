#include "mcpch.h"
#include "WorldRenderer.h"

#include "../Blocks/BlockDatabase.h"
#include "WorldEvents.h"

namespace Minecraft
{

	WorldRenderer::WorldRenderer(const World* targetWorld, Layer* layer)
		: m_TargetWorld(targetWorld), m_Layer(layer), m_LoadedListener(), m_UnloadedListener(), m_UpdatedListener()
	{
		m_LoadedListener = EventManager::Get().Bus().AddScopedEventListener<ChunkLoaded>([this](Event<ChunkLoaded>& e)
			{
				if (&e.Data.Dimension == m_TargetWorld && m_Entities.find(e.Data.Position) == m_Entities.end())
				{
					ChunkPos_t position = e.Data.Position;
					ChunkNeighbours neighbours = e.Data.Dimension.GetChunkNeighbours(e.Data.Position);
					float x = e.Data.Position.x * WorldChunk::CHUNK_SIZE;
					float z = e.Data.Position.y * WorldChunk::CHUNK_SIZE;
					CreateMeshFromFaces(e.Data.Chunk.GetVisibleFaces({}, neighbours)).ContinueWithOnMainThread([this, x, z, position](std::pair<Mesh, ModelMapping> mesh)
						{
							{
								ModelMapping temp{ std::move(mesh.second) };
							}
							if (m_Entities.find(position) != m_Entities.end() && !m_Entities[position])
							{
								EntityHandle entity = m_Layer->GetFactory().CreateMesh(std::move(mesh.first), Transform({ x, 0, z }));
								m_Entities[position] = entity;
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

	Task<std::pair<Mesh, ModelMapping>> WorldRenderer::CreateMeshFromFaces(std::vector<BlockRenderableFace> faces) const
	{
		Mesh m;
		m.Materials.push_back(ResourceManager::Get().Materials().Texture(BlockDatabase::GetBlockFaces()));
		ModelData data;
		data.Indices = std::make_unique<IndexArray>();
		data.Vertices = std::make_unique<VertexArray>();
		data.Indices->AddIndexBuffer(std::make_unique<IndexBuffer>(faces.size() * 6));
		BufferLayout layout = BufferLayout::Default();
		data.Vertices->CreateVertexBuffer(4 * faces.size() * layout.Size(), layout);

		ModelMapping mapping = data.Map();
			
		return TaskManager::Get().Run(make_shared_function([mesh{ std::move(m) }, mapping{ std::move(mapping) }, faces{ std::move(faces) }, data{ std::move(data) }]() mutable
		{
			const VertexMapping& vMapping = mapping.VertexMap;
			const IndexMapping& iMapping = mapping.IndexMap;
			Vector4<byte> color = Color::White.ToBytes();
			VertexIterator it = vMapping.Begin();
			IndexIterator indices = iMapping.Begin();
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
			mesh.Models.push_back({ ResourcePtr<Model>(new Model(std::move(data)), true), Matrix4f::Identity(), { 0 } });
			return std::pair<Mesh, ModelMapping>{ mesh, std::move(mapping) };
		}));
	}

	void WorldRenderer::UpdateChunk(const ChunkPos_t& position, const WorldChunk* chunk)
	{
		if (chunk != nullptr)
		{
			if (m_Entities.find(position) != m_Entities.end())
			{
				CreateMeshFromFaces(chunk->GetVisibleFaces({}, m_TargetWorld->GetChunkNeighbours(position))).ContinueWithOnMainThread([this, position](std::pair<Mesh, ModelMapping> mesh)
					{
						{
							ModelMapping temp{ std::move(mesh.second) };
						}
						if (m_Entities.find(position) != m_Entities.end())
						{
							EntityHandle entity = m_Entities[position];
							if (entity)
							{
								entity.Assign<Mesh>(std::move(mesh.first));
							}
						}
					});
			}
		}
	}

}
