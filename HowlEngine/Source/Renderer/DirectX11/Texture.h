#pragma once

namespace HEngine
{
	using namespace Microsoft::WRL;

	class Texture
	{
	public:
		bool LoadFromFile(const std::wstring& filepath, ID3D11Device* pDevice);

		inline ID3D11ShaderResourceView* GetSRV() const { return mShaderResView.Get(); };
	private:
		ComPtr<ID3D11ShaderResourceView> mShaderResView;
	};
}


