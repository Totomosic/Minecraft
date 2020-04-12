#pragma once
#include "BoltLib/Math/Vectors/Vector3.h"

namespace Minecraft
{

	using dimension_t = int64_t;
	using BlockPos_t = Bolt::Vector3<dimension_t>;
	using ChunkPos_t = Bolt::Vector2i;

	inline float Sign(float v)
	{
		return (v < 0) ? -1.0f : (v == 0) ? 0 : 1.0f;
	}

}

namespace std
{

	template<>
	struct hash<Minecraft::BlockPos_t>
	{
	public:
		size_t operator()(const Minecraft::BlockPos_t& value) const
		{
			size_t result = value.x;
			result |= ((size_t)value.y << 32);
			result |= ((size_t)value.z << 16);
			return result;
		}
	};

	template<>
	struct hash<Minecraft::ChunkPos_t>
	{
	public:
		size_t operator()(const Minecraft::ChunkPos_t& value) const
		{
			size_t result = value.x;
			result |= ((size_t)value.y << 32);
			return result;
		}
	};

}