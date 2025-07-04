#pragma once

namespace HEngine
{
	using namespace Microsoft::WRL;
	using namespace DirectX;

	struct DirectionalLight
	{
		XMFLOAT4 lightDirection;
		XMFLOAT4 lightColor;
		XMFLOAT4 lightAmbient;
		XMFLOAT4 lightDiffuse;
	};

	class LightHelper
	{
	public:
		LightHelper() = default;
		LightHelper(const LightHelper&) = delete;
		LightHelper* operator=(const LightHelper&) = delete;

		void CreateDirectionalLightBuffer(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11Buffer>& pLightBuffer);
		void UpdateDirectionalLightBuffer(ID3D11DeviceContext& pDeviceContext, ComPtr<ID3D11Buffer>& pLightBuffer);
	};
}


