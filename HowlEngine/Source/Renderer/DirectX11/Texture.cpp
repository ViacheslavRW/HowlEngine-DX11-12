#include "pch.h"
#include "Texture.h"

namespace HEngine
{
	bool Texture::LoadFromFile(const std::wstring& filepath, ID3D11Device* pDevice, TextureFormat format)
	{
		DirectX::ScratchImage image;
		switch (format)
		{
			case TextureFormat::PNG:
			{
				HRESULT hr = DirectX::LoadFromWICFile(filepath.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image);
				if (FAILED(hr))
				{
					std::cout << "FAILED_TO_LOAD_TEXTURE" << filepath.c_str() << std::endl;
					return false;
				}
			}; break;
			case TextureFormat::JPG:
			{
				HRESULT hr = DirectX::LoadFromWICFile(filepath.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image);
				if (FAILED(hr))
				{
					std::cout << "FAILED_TO_LOAD_TEXTURE" << filepath.c_str() << std::endl;
					return false;
				}
			} break;
			case TextureFormat::TGA:
			{
				HRESULT hr = DirectX::LoadFromTGAFile(filepath.c_str(), nullptr, image);
				if (FAILED(hr))
				{
					std::cout << "FAILED_TO_LOAD_TEXTURE" << filepath.c_str() << std::endl;
					return false;
				}
			} break;
		}

		HRESULT hr = DirectX::CreateShaderResourceView(pDevice, image.GetImages(), image.GetImageCount(), image.GetMetadata(), mShaderResView.GetAddressOf());
		if (FAILED(hr))
		{
			std::cout << "FAILED_TO_CREATE_SRV" << std::endl;
			return false;
		}

		return true;
	}
}