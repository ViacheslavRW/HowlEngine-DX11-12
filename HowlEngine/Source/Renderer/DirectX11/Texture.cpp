#include "pch.h"
#include "Texture.h"

namespace HEngine
{
	bool Texture::LoadFromFile(const std::wstring& filepath, ID3D11Device* pDevice, TextureFormat format)
	{
		DirectX::ScratchImage image;
		DirectX::TexMetadata metadata;

		HRESULT hr = E_FAIL;

		switch (format)
		{
			case TextureFormat::PNG:
			case TextureFormat::JPG:
			{
				hr = DirectX::LoadFromWICFile(filepath.c_str(), DirectX::WIC_FLAGS_NONE, &metadata, image);
			} break;
			case TextureFormat::TGA:
			{
				hr = DirectX::LoadFromTGAFile(filepath.c_str(), &metadata, image);
			} break;
		}

		if (FAILED(hr))
		{
			std::wcout << L"FAILED_TO_LOAD_TEXTURE: " << filepath << std::endl;
			return false;
		}

		// check for alpha
		//bool hasAlpha = DirectX::HasAlpha(metadata.format);
		//if (hasAlpha)
		//	std::wcout << L"Texture has alpha: " << filepath << std::endl;
		//else
		//	std::wcout << L"Texture has NO alpha: " << filepath << std::endl;

		hr = DirectX::CreateShaderResourceView(pDevice, image.GetImages(), image.GetImageCount(), image.GetMetadata(), mShaderResView.GetAddressOf());
		if (FAILED(hr))
		{
			std::cout << "FAILED_TO_CREATE_SRV" << std::endl;
			return false;
		}

		return true;
	}
}