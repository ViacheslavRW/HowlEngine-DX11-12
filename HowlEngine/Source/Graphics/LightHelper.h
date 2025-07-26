#pragma once
#include <array>
#include "../Mesh/PBRMesh.h"

namespace HEngine
{
	using namespace Microsoft::WRL;
	using namespace DirectX;

	struct DirectionalLight
	{
		XMFLOAT4 lightDirection = { -0.5f, -1.0f, -0.5f, 0.0f };
		XMFLOAT4 lightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		XMFLOAT4 lightAmbient = { 0.1f, 0.1f, 0.1f, 1.0f };
		XMFLOAT4 lightDiffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
	};

	struct PointLight
	{
		XMFLOAT3 lightPosition = { 0.0f, 0.0f, 0.0f };
		UINT active = 1; // bool may be unpredictable in HLSL
		XMFLOAT4 lightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		XMFLOAT4 lightAmbient = { 0.7f, 0.7f, 0.7f, 1.0f };
		XMFLOAT4 lightDiffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
		XMFLOAT3 lightAttenuation = { 1.0f, 0.14f, 0.07f };
		float lightRange = 15.0f;
	};

	struct PointLightBuffer
	{
		PointLight pointLights[MAX_POINT_LIGHTS];
	};

	class LightHelper
	{
	public:
		LightHelper() = default;
		LightHelper(const LightHelper&) = delete;
		LightHelper* operator=(const LightHelper&) = delete;

		void CreateDirectionalLightBuffer(ComPtr<ID3D11Device>& pDevice);
		void CreatePointLightBuffer(ComPtr<ID3D11Device>& pDevice);

		void UpdateDirectionalLightBuffer(ID3D11DeviceContext& pDeviceContext);
		void UpdatePointLightBuffer(ID3D11DeviceContext& pDeviceContext);

		void Release();
	public:
		// dir light
		inline XMFLOAT4& SetDirectionalLightDirection() { mDirtyDirectionalLight = true; return directionalLightParams.lightDirection; };
		inline XMFLOAT4& SetDirectionalLightColor() { mDirtyDirectionalLight = true; return directionalLightParams.lightColor; };
		inline XMFLOAT4& SetDirectionalLightAmbient() { mDirtyDirectionalLight = true; return directionalLightParams.lightAmbient; };
		inline XMFLOAT4& SetDirectionalLightDiffuse() { mDirtyDirectionalLight = true; return directionalLightParams.lightDiffuse; };
		inline DirectionalLight& GetDirectionalLightParams() { return directionalLightParams; }
		// point light
		void SetPointLightPosition(const USHORT& index, const XMFLOAT3& position);
		void SetPointLightColor(const USHORT& index, const XMFLOAT4& color);
		void SetPointLightAmbient(const USHORT& index, const XMFLOAT4& ambient);
		void SetPointLightDiffuse(const USHORT& index, const XMFLOAT4& diffuse);
		void SetPointLightAttenuation(const USHORT& index, const XMFLOAT3& attenuation);
		void SetPointLightRange(const USHORT& index, const float& range);
		void SetPointLightActive(const USHORT& index, const bool& isActive);
		inline PointLight& GetPointLightParams(const USHORT& index) { return pointLightParams[index]; };
		inline void SetPointLightSource(const USHORT& index, PBRMesh& pMesh) { pointLightSources[index] = &pMesh; };
		inline void RemovePointLightSource(const USHORT& index) { pointLightSources[index] = nullptr; };

	public:
		bool mDirtyDirectionalLight = true;
		bool mDirtyPointLight = true;
	private:
		ComPtr<ID3D11Buffer> mDirectionalLightBuffer = nullptr;
		ComPtr<ID3D11Buffer> mPointLightBuffer = nullptr;
	private:
		DirectionalLight directionalLightParams;
		std::array<PointLight, MAX_POINT_LIGHTS> pointLightParams = {};
		std::array<PBRMesh*, MAX_POINT_LIGHTS> pointLightSources = {};
	};
}


