#include "mcpch.h"
#include "World/WorldRenderer.h"
#include "World/Generators/FlatWorldGenerator.h"
#include "World/Generators/SimplexNoiseGenerator.h"
#include "Blocks/BlockDatabase.h"
#include "Textures/TextureAtlas.h"

#include "Systems/MovementSystem.h"
#include "Systems/CMovement.h"
#include "Systems/CDimensions.h"
#include "Systems/CameraSystem.h"

#include "World/Raycaster.h"

namespace Minecraft
{

	class MinecraftClient : public Application
	{
	public:
		static constexpr int RENDER_DISTANCE = 12;
		static constexpr float PLAYER_SPEED = 5.0f;
		static constexpr float REACH = 4.5f;

	public:
		Vector2f m_MouseSensitivity = { 0.002f, 0.002f };
		EntityHandle m_Camera;
		EntityHandle m_Player;
		Vector3f m_PlayerSize = { 0.75f, 1.8f, 0.75f };

		World m_World;
		WorldRenderer* m_WorldRenderer;
		ChunkPos_t m_LastChunk;
		RaycastHit m_SelectedBlock;

		UIText* m_PositionText;

	public:
		void Init() override
		{
			GetWindow().EnableVSync();
			GetWindow().SetClearColor(Color::CornflowerBlue);
			Scene& scene = SceneManager::Get().AddScene();
			Layer& mainLayer = scene.AddLayer();
			Layer& uiLayer = scene.AddLayer();
			EntityHandle uiCamera = scene.GetFactory().Camera(Matrix4f::Orthographic(0, 1280, 0, 720, -100, 100));
			uiLayer.SetActiveCamera(uiCamera);
			
			m_Camera = scene.GetFactory().Camera(Matrix4f::Perspective(PI / 3.0f, GetWindow().Aspect(), 0.1f, 1000.0f));
			mainLayer.SetActiveCamera(m_Camera);

			EntityFactory factory = mainLayer.GetFactory();
			m_Player = factory.Cuboid(m_PlayerSize.x, m_PlayerSize.y, m_PlayerSize.z, Color::Red, Transform({ 0, 1000, 0 }));
			m_Player.GetComponent<Mesh>()->Models[0].Transform = Matrix4f::Translation({ 0, m_PlayerSize.y / 2.0f, 0 }) * m_Player.GetComponent<Mesh>()->Models[0].Transform;
			m_Camera.GetTransform()->SetParent(m_Player.GetTransform().Get());
			m_Camera.GetTransform()->SetLocalPosition({ 0, m_PlayerSize.y - 0.2f, 0 });
			m_Camera.Assign<CCameraController>(CCameraController{ m_MouseSensitivity });
			m_Player.Assign<CDimensions>(CDimensions{ m_PlayerSize });
			m_Player.Assign<CMovement>();

			m_World = World(std::make_unique<SimplexNoiseGenerator>(55, 75));
			m_WorldRenderer = new WorldRenderer(&m_World, &mainLayer);
			mainLayer.Systems().Add<MovementSystem>(&m_World);
			scene.Systems().Add<CameraSystem>();

			m_PositionText = &uiLayer.GetUI().GetRoot().CreateText("", ResourceManager::Get().Fonts().Arial(20), Color::Black, Transform({ 5, 720 - 5, 0 }), AlignH::Left, AlignV::Top);
			UIRectangle& crosshairX = uiLayer.GetUI().GetRoot().CreateRectangle(20, 2, Color::White, Transform({ 1280 / 2, 720 / 2, 0 }));
			UIRectangle& crosshairZ = uiLayer.GetUI().GetRoot().CreateRectangle(2, 20, Color::White, Transform({ 1280 / 2, 720 / 2, 0 }));

			m_LastChunk = { INT_MAX, INT_MAX };
			ResourceManager::Get().LoadPack("res/resources.pack", [this](const ResourcePack& pack)
				{
					ResourceExtractor resources(pack);
					ResourcePtr<Texture2D> blockFacesTexture = resources.GetResourcePtr<Texture2D>("BlockFaces");
					TextureAtlas blockFaces(blockFacesTexture, 16, 16);

					BlockData air;
					air.Id = BlockId::Air;
					BlockDatabase::Register(air);

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

					BlockData oakLog;
					oakLog.Id = BlockId::OakLog;
					oakLog.Textures.nx = blockFaces.GetImage(4);
					oakLog.Textures.px = blockFaces.GetImage(4);
					oakLog.Textures.ny = blockFaces.GetImage(5);
					oakLog.Textures.py = blockFaces.GetImage(5);
					oakLog.Textures.nz = blockFaces.GetImage(4);
					oakLog.Textures.pz = blockFaces.GetImage(4);
					BlockDatabase::Register(oakLog);

					BlockData oakLeaves;
					oakLeaves.Id = BlockId::OakLeaves;
					oakLeaves.Textures.nx = blockFaces.GetImage(6);
					oakLeaves.Textures.px = blockFaces.GetImage(6);
					oakLeaves.Textures.ny = blockFaces.GetImage(6);
					oakLeaves.Textures.py = blockFaces.GetImage(6);
					oakLeaves.Textures.nz = blockFaces.GetImage(6);
					oakLeaves.Textures.pz = blockFaces.GetImage(6);
					BlockDatabase::Register(oakLeaves);

					BlockDatabase::SetBlockFaces(blockFacesTexture);
				});

			Input::Get().HideCursor();
		}

		void Update() override
		{
			m_WorldRenderer->Update();
			ComponentHandle t = m_Camera.GetTransform();
			ComponentHandle movement = m_Player.GetComponent<CMovement>();

			Raycaster caster(&m_World);
			m_SelectedBlock = caster.Cast(Ray(t->Position(), t->Forward()), REACH);

			m_PositionText->SetText("x: " + std::to_string(t->Position().x) + " y: " + std::to_string(t->Position().y) + " z: " + std::to_string(t->Position().z));

			Vector3f forward = t->Forward();
			forward.y = 0;
			forward = forward.Normalize();
			Vector3f right = t->Right();
			right.y = 0;
			right = right.Normalize();

			movement->Velocity.xz() = Vector2f{ 0, 0 };
			movement->Acceleration = { 0, -30, 0 };

			if (Input::Get().MouseButtonPressed(MouseButton::Left))
			{				
				if (m_SelectedBlock.Hit)
				{
					m_World.SetBlock(m_SelectedBlock.BlockPosition, BlockId::Air);
				}
			}
			else if (Input::Get().MouseButtonPressed(MouseButton::Right))
			{
				if (m_SelectedBlock.Hit)
				{
					m_World.SetBlock(m_SelectedBlock.BlockPosition + m_SelectedBlock.Normal, m_SelectedBlock.BlockId);
				}
			}

			bool set = false;
			if (Input::Get().KeyDown(Keycode::W))
			{
				movement->Velocity += forward;
				set = true;
			}
			if (Input::Get().KeyDown(Keycode::S))
			{
				movement->Velocity += -forward;
				set = true;
			}
			if (Input::Get().KeyDown(Keycode::D))
			{
				movement->Velocity += right;
				set = true;
			}
			if (Input::Get().KeyDown(Keycode::A))
			{
				movement->Velocity += -right;
				set = true;
			}
			if (set)
			{
				movement->Velocity.xz() = movement->Velocity.xz().Normalize() * PLAYER_SPEED;
			}

			if (Input::Get().KeyDown(Keycode::Space) && abs(movement->Velocity.y) < 1e-6)
			{
				movement->Velocity.y = 10;
			}
			// Load chunks
			if (BlockDatabase::GetBlockFaces() != nullptr)
			{
				ChunkPos_t chunk = m_World.GetChunkFromBlock((BlockPos_t)t->Position());
				if (chunk != m_LastChunk)
				{
					std::vector<ChunkPos_t> shouldBeLoaded;
					shouldBeLoaded.reserve((1 + 2 * (size_t)RENDER_DISTANCE) * (1 + 2 * (size_t)RENDER_DISTANCE));
					for (int i = -RENDER_DISTANCE; i <= RENDER_DISTANCE; i++)
						for (int j = -RENDER_DISTANCE; j <= RENDER_DISTANCE; j++)
							shouldBeLoaded.push_back({ chunk.x + i, chunk.y + j });

					for (ChunkPos_t c : m_World.GetLoadedChunks())
						if (std::find(shouldBeLoaded.begin(), shouldBeLoaded.end(), c) == shouldBeLoaded.end())
							m_World.UnloadChunk(c);

					for (ChunkPos_t c : shouldBeLoaded)
						m_World.LoadChunk(c);

					m_LastChunk = chunk;
				}
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