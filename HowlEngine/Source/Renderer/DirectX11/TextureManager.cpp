#include "pch.h"
#include "TextureManager.h"

namespace HEngine
{
	bool TextureManager::LoadTexture(const std::string& name, const std::wstring path, ID3D11Device* pDevice)
	{
		if (mTextures.find(name) != mTextures.end()) return true;

		std::unique_ptr<Texture> texture = std::make_unique<Texture>();
		if (!texture->LoadFromFile(path, pDevice)) return false;

		mTextures[name] = std::move(texture);
		return true;
	}

	ComPtr<ID3D11ShaderResourceView> TextureManager::GetTextureSRV(const std::string& name)
	{
		auto tex = mTextures.find(name);
		if (tex != mTextures.end()) return tex->second->GetSRV();
		return nullptr;
	}

	void TextureManager::Clear()
	{
		mTextures.clear();
	}
}
