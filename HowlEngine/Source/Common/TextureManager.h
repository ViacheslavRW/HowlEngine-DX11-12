#pragma once
#include "./Texture.h"
#include <unordered_map>

namespace HEngine
{
	class TextureManager
	{
	public:
		TextureManager() = default;
		~TextureManager() = default;

		TextureManager(const TextureManager&) = delete;
		TextureManager& operator=(const TextureManager&) = delete;

		bool LoadTexture(const std::string& name, const std::wstring& path, ID3D11Device* pDevice, TextureFormat format);
		ComPtr<ID3D11ShaderResourceView> GetTextureSRV(const std::string& name);
		ID3D11ShaderResourceView* LoadAndGetSRV(const std::string& name, const std::wstring& path, ID3D11Device* pDevice, TextureFormat format);
		void Clear();

	private:
		std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;
	};
}


