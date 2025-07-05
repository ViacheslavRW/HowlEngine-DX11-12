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
		void UpdateDirectionalLightBuffer(ID3D11DeviceContext& pDeviceContext, ComPtr<ID3D11Buffer>& pLightBuffer, DirectionalLight& dirLightParams);

	public:
		inline XMFLOAT4& SetDirectionalLightDirection() { return directionalLightParams.lightDirection; };
		inline XMFLOAT4& SetDirectionalLightColor() { return directionalLightParams.lightColor; };
		inline XMFLOAT4& SetDirectionalLightAmbient() { return directionalLightParams.lightAmbient; };
		inline XMFLOAT4& SetDirectionalLightDiffuse() { return directionalLightParams.lightDiffuse; };
		inline DirectionalLight& GetDirectionalLightParams() { return directionalLightParams; }
	private:
		DirectionalLight directionalLightParams = {};
	};
}


