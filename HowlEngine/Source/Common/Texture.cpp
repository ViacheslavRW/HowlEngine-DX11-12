#include "pch.h"
#include "Texture.h"

namespace HEngine
{
	bool Texture::LoadFromFile(const std::wstring& filepath, ID3D11Device* pDevice, TextureFormat format, bool generateMips)
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

		if (generateMips && image.GetImageCount() == 1)
		{
			DirectX::ScratchImage mipChain;
			hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_DEFAULT, 0, mipChain);

			if (SUCCEEDED(hr))
			{
				image = std::move(mipChain);
			}
		}

		hr = DirectX::CreateShaderResourceView(pDevice, image.GetImages(), image.GetImageCount(), image.GetMetadata(), mShaderResView.GetAddressOf());
		if (FAILED(hr))
		{
			std::cout << "FAILED_TO_CREATE_SRV" << std::endl;
			return false;
		}

		return true;
	}
}