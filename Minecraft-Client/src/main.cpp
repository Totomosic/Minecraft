#include "mcpch.h"
#include "World/WorldRenderer.h"
#include "World/Generators/FlatWorldGenerator.h"
#include "Blocks/BlockDatabase.h"
#include "Textures/TextureAtlas.h"

namespace Minecraft
{

	class MinecraftClient : public Application
	{
	public:
		Vector2f m_MouseSensitivity = { 0.005f, 0.005f };
		EntityHandle m_Player;

		World m_World;
		WorldRenderer* m_WorldRenderer;
		ChunkPos_t m_LastChunk;

	public:
		void Init() override
		{
			Scene& scene = SceneManager::Get().AddScene();
			Layer& mainLayer = scene.AddLayer();
			
			EntityHandle camera = scene.GetFactory().Camera(Matrix4f::Perspective(PI / 3.0f, GetWindow().Aspect(), 0.1f, 1000.0f));
			mainLayer.SetActiveCamera(camera);

			EntityFactory factory = mainLayer.GetFactory();
			m_Player = factory.Cuboid(1, 2, 1, Color::Red, Transform({ 0, 10, 0 }));
			camera.GetTransform()->SetParent(m_Player.GetTransform().Get());

			factory.Cuboid(1, 1, 1, Color::Red, Transform({ 0, 10, -10 }));

			m_World = World(std::make_unique<FlatWorldGenerator>());
			m_WorldRenderer = new WorldRenderer(&m_World, &mainLayer);

			m_LastChunk = { 0, 0 };

			ResourceManager::Get().LoadPack("res/resources.pack", [this](const ResourcePack& pack)
				{
					ResourceExtractor resources(pack);
					ResourcePtr<Texture2D> blockFacesTexture = resources.GetResourcePtr<Texture2D>("BlockFaces");
					TextureAtlas blockFaces(blockFacesTexture, 16, 16);

					BlockData dirt;
					dirt.Id = BlockId::Dirt;
					dirt.Textures.nx = blockFaces.GetImage(2);
					dirt.Textures.px = blockFaces.GetImage(2);
					dirt.Textures.ny = blockFaces.GetImage(2);
					dirt.Textures.py = blockFaces.GetImage(2);
					dirt.Textures.nz = blockFaces.GetImage(2);
					dirt.Textures.pz = blockFaces.GetImage(2);
					BlockDatabase::Register(dirt);

					BlockData grass;
					grass.Id = BlockId::Grass;
					grass.Textures.nx = blockFaces.GetImage(1);
					grass.Textures.px = blockFaces.GetImage(1);
					grass.Textures.ny = blockFaces.GetImage(2);
					grass.Textures.py = blockFaces.GetImage(0);
					grass.Textures.nz = blockFaces.GetImage(1);
					grass.Textures.pz = blockFaces.GetImage(1);
					BlockDatabase::Register(grass);

					BlockData stone;
					stone.Id = BlockId::Stone;
					stone.Textures.nx = blockFaces.GetImage(3);
					stone.Textures.px = blockFaces.GetImage(3);
					stone.Textures.ny = blockFaces.GetImage(3);
					stone.Textures.py = blockFaces.GetImage(3);
					stone.Textures.nz = blockFaces.GetImage(3);
					stone.Textures.pz = blockFaces.GetImage(3);
					BlockDatabase::Register(stone);

					BlockDatabase::SetBlockFaces(blockFacesTexture);

					m_World.LoadChunk({ 0, 0 });
				});

			Input::Get().HideCursor();
		}

		void Update() override
		{
			int chunkRadius = 3;

			ComponentHandle t = m_Player.GetTransform();
			float speed = 10 * Time::Get().RenderingTimeline().DeltaTime();
			if (Input::Get().KeyDown(Keycode::W))
			{
				t->Translate(t->Forward() * speed);
			}
			else if (Input::Get().KeyDown(Keycode::S))
			{
				t->Translate(t->Forward() * -speed);
			}
			else if (Input::Get().KeyDown(Keycode::D))
			{
				t->Translate(t->Right() * speed);
			}
			else if (Input::Get().KeyDown(Keycode::A))
			{
				t->Translate(t->Right() * -speed);
			}
			Vector3f relMouse = Input::Get().RelMousePosition();
			t->Rotate(-relMouse.x * m_MouseSensitivity.x, Vector3f::Up(), Space::World);
			t->Rotate(relMouse.y * m_MouseSensitivity.y, Vector3f::Right(), Space::Local);

			ChunkPos_t chunk = m_World.GetChunkFromBlock((BlockPos_t)t->Position());
			if (chunk != m_LastChunk)
			{
				std::vector<ChunkPos_t> shouldBeLoaded;
				shouldBeLoaded.reserve((1 + 2 * (size_t)chunkRadius) * (1 + 2 * (size_t)chunkRadius));
				for (int i = -chunkRadius; i <= chunkRadius; i++)
					for (int j = -chunkRadius; j <= chunkRadius; j++)
						shouldBeLoaded.push_back({ chunk.x + i, chunk.y + j });
				for (ChunkPos_t c : m_World.GetLoadedChunks())
				{
					if (std::find(shouldBeLoaded.begin(), shouldBeLoaded.end(), c) == shouldBeLoaded.end())
					{
						m_World.UnloadChunk(c);
					}
				}
				for (ChunkPos_t c : shouldBeLoaded)
				{
					m_World.LoadChunk(c);
				}
				m_LastChunk = chunk;
			}
		}

		void Render() override
		{
			Graphics::Get().RenderScene();
		}
	};

}

int main()
{
	EngineCreateInfo eInfo;
	Engine e(eInfo);
	e.SetApplication<Minecraft::MinecraftClient>();
	e.Run();
}