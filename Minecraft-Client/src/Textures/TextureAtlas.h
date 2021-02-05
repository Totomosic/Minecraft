#pragma once
#include "mcpch.h"

namespace Minecraft
{

	struct TextureCoordinate
	{
	public:
		Vector2f Min;
		Vector2f Max;
	};

	class TextureAtlas
	{
	private:
		AssetHandle<Texture2D> m_Texture;
		int m_IndividualWidth;
		int m_IndividualHeight;

	public:
		TextureAtlas();
		TextureAtlas(const AssetHandle<Texture2D>& texture, int individualWidth, int individualHeight);

		int GetIndividualWidth() const;
		int GetIndividualHeight() const;
		const AssetHandle<Texture2D>& GetTexture() const;
		int GetImagesPerRow() const;
		int GetImagesPerCol() const;
		int GetTotalImages() const;
		TextureCoordinate GetImage(int index) const;

	private:
		Vector2i GetXYIndicesFromIndex(int index) const;

	};

}