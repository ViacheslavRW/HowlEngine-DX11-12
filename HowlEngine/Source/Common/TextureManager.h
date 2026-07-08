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

		bool LoadTexture(const std::string& name, const std::wstring& path, ID3D11Device* pDevice, TextureFormat format, bool generateMips = true);
		void LoadDefaultTextures(ID3D11Device* pDevice, bool generateMips = true);
		ComPtr<ID3D11ShaderResourceView> GetTextureSRV(const std::string& name);
		ID3D11ShaderResourceView* LoadAndGetSRV(const std::string& name, const std::wstring& path, ID3D11Device* pDevice, TextureFormat format, bool generateMips = true);
		void Clear();

	private:
		std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;

		const std::string defColorTexName = "Def_NoTexture";
		const std::wstring defColorTexNamePath = L"Assets/Textures/Default/Def_NoTexture.png";

		const std::string defNormalTexName = "Def_NoTexture_N";
		const std::wstring defNormalTexNamePath = L"Assets/Textures/Default/Def_NoTexture_N.png";

		const std::string defORMTexName = "Def_NoTexture_ORM";
		const std::wstring defORMTexNamePath = L"Assets/Textures/Default/Def_NoTexture_ORM.png";

		const std::string defEmissiveTexName = "Def_NoTexture_E";
		const std::wstring defEmissiveTexNamePath = L"Assets/Textures/Default/Def_NoTexture_E.png";
	};
}


