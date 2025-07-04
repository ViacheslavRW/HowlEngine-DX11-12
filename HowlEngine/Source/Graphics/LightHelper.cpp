#include "pch.h"
#include "LightHelper.h"

namespace HEngine
{
	void LightHelper::CreateDirectionalLightBuffer(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11Buffer>& pLightBuffer)
	{
		D3D11_BUFFER_DESC lightBufferDesc = {};
		lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		lightBufferDesc.ByteWidth = sizeof(DirectionalLight);
		lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		lightBufferDesc.MiscFlags = NULL;

		HRESULT hr = pDevice->CreateBuffer(&lightBufferDesc, nullptr, pLightBuffer.GetAddressOf());
		if (FAILED(hr)) std::cout << "FAILED_TO_CREATE_LIGHT_BUFFER" << std::endl;
	}

	void LightHelper::UpdateDirectionalLightBuffer(ID3D11DeviceContext& pDeviceContext, ComPtr<ID3D11Buffer>& pLightBuffer)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = pDeviceContext.Map(pLightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		if (SUCCEEDED(hr))
		{
			auto* lightData = reinterpret_cast<DirectionalLight*>(mappedResource.pData);

			lightData->lightDirection = { -0.5f, -1.0f, 0.5f, 0.0f };
			lightData->lightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
			lightData->lightAmbient = { 0.5f, 0.5f, 0.5f, 1.0f };
			lightData->lightDiffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

			pDeviceContext.Unmap(pLightBuffer.Get(), 0);
			pDeviceContext.PSSetConstantBuffers(1, 1, pLightBuffer.GetAddressOf());
		}
	}
}