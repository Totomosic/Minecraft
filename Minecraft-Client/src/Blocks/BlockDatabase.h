#pragma once
#include "Block.h"
#include "../Textures/TextureAtlas.h"

namespace Minecraft
{

	struct TextureCoordinateSet
	{
	public:
		TextureCoordinate nx;
		TextureCoordinate px;
		TextureCoordinate ny;
		TextureCoordinate py;
		TextureCoordinate nz;
		TextureCoordinate pz;
	};

	struct BlockData
	{
	public:
		BlockId Id;
		TextureCoordinateSet Textures;
	};

	class BlockDatabase
	{
	private:
		static std::unordered_map<BlockId, BlockData> s_Blocks;
		static ResourcePtr<Texture2D> s_BlockFaces;

	public:
		static const ResourcePtr<Texture2D>& GetBlockFaces();
		static const BlockData& GetBlock(BlockId id);
		static void Register(const BlockData& data);
		static void SetBlockFaces(const ResourcePtr<Texture2D>& faces);
	};

}