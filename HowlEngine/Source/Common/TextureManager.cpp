#include "pch.h"
#include "TextureManager.h"
#include "DirectXTex.h"

namespace HEngine
{
	bool TextureManager::LoadTexture(const std::string& name, const std::wstring& path, ID3D11Device* pDevice, TextureFormat format, bool generateMips)
	{
		if (mTextures.find(name) != mTextures.end()) return true;

		std::unique_ptr<Texture> texture = std::make_unique<Texture>();
		if (!texture->LoadFromFile(path, pDevice, format, generateMips)) return false;

		mTextures[name] = std::move(texture);
		return true;
	}

	void TextureManager::LoadDefaultTextures(ID3D11Device* pDevice, bool generateMips)
	{
		LoadTexture(defColorTexName, defColorTexNamePath, pDevice, TextureFormat::PNG, generateMips);
		LoadTexture(defNormalTexName, defNormalTexNamePath, pDevice, TextureFormat::PNG, generateMips);
		LoadTexture(defORMTexName, defORMTexNamePath, pDevice, TextureFormat::PNG, generateMips);
	}

	ComPtr<ID3D11ShaderResourceView> TextureManager::GetTextureSRV(const std::string& name)
	{
		auto tex = mTextures.find(name);
		if (tex != mTextures.end()) return tex->second->GetSRV();
		return nullptr;
	}

	ID3D11ShaderResourceView* TextureManager::LoadAndGetSRV(const std::string& name, const std::wstring& path, ID3D11Device* pDevice, TextureFormat format, bool generateMips)
	{
		if (!LoadTexture(name, path, pDevice, format, generateMips))
			return nullptr;

		return GetTextureSRV(name).Get();
	}

	void TextureManager::Clear()
	{
		mTextures.clear();
	}
}
