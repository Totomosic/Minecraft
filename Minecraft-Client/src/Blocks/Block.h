#pragma once
#include "../Textures/TextureAtlas.h"

namespace Minecraft
{

	enum class BlockId : uint8_t
	{
		Air,
		Dirt,
		Grass,
		Stone,
		Cobblestone,
		Sand,
		Gravel
	};

	struct BlockFace
	{
	public:
		Vector3f TopLeft;
		Vector3f BottomLeft;
		Vector3f BottomRight;
		Vector3f TopRight;
		Vector3f Normal;
		TextureCoordinate Texture;
	};

}