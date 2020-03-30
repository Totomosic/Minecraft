#include "mcpch.h"
#include "BlockDatabase.h"

namespace Minecraft
{

	std::unordered_map<BlockId, BlockData> BlockDatabase::s_Blocks;
	ResourcePtr<Texture2D> BlockDatabase::s_BlockFaces;

	const ResourcePtr<Texture2D>& BlockDatabase::GetBlockFaces()
	{
		return s_BlockFaces;
	}

	const BlockData& BlockDatabase::GetBlock(BlockId id)
	{
		return s_Blocks.at(id);
	}

	void BlockDatabase::Register(const BlockData& data)
	{
		s_Blocks[data.Id] = data;
	}

	void BlockDatabase::SetBlockFaces(const ResourcePtr<Texture2D>& faces)
	{
		s_BlockFaces = faces;
	}

}
