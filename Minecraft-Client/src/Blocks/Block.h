#pragma once
#include "../Textures/TextureAtlas.h"

namespace Minecraft
{

	enum class BlockFace : uint8_t
	{
		NegativeX,
		PositiveX,
		NegativeY,
		PositiveY,
		NegativeZ,
		PositiveZ
	};

	inline BlockPos_t GetBlockNormal(BlockFace face)
	{
		switch (face)
		{
		case BlockFace::NegativeX:
			return { -1, 0, 0 };
		case BlockFace::PositiveX:
			return { 1, 0, 0 };
		case BlockFace::NegativeY:
			return { 0, -1, 0 };
		case BlockFace::PositiveY:
			return { 0, 1, 0 };
		case BlockFace::NegativeZ:
			return { 0, 0, -1 };
		case BlockFace::PositiveZ:
			return { 0, 0, 1 };
		}
		return {};
	}

	inline BlockFace FaceFromNormal(const BlockPos_t& normal)
	{
	
	}

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

	struct BlockRenderableFace
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