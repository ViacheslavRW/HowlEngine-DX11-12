#include "pch.h"
#include "LightHelper.h"

namespace HEngine
{
	void LightHelper::CreateDirectionalLightBuffer(ComPtr<ID3D11Device>& pDevice)
	{
		D3D11_BUFFER_DESC lightBufferDesc = {};
		lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		lightBufferDesc.ByteWidth = sizeof(DirectionalLight);
		lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		lightBufferDesc.MiscFlags = NULL;

		HRESULT hr = pDevice->CreateBuffer(&lightBufferDesc, nullptr, mDirectionalLightBuffer.GetAddressOf());
		if (FAILED(hr)) std::cout << "FAILED_TO_CREATE_DIRECTIONAL_LIGHT_BUFFER" << std::endl;
	}

	void LightHelper::CreatePointLightBuffer(ComPtr<ID3D11Device>& pDevice)
	{
		D3D11_BUFFER_DESC lightBufferDesc = {};
		lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		lightBufferDesc.ByteWidth = sizeof(PointLightBuffer);
		lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		lightBufferDesc.MiscFlags = NULL;

		HRESULT hr = pDevice->CreateBuffer(&lightBufferDesc, nullptr, mPointLightBuffer.GetAddressOf());
		if (FAILED(hr)) std::cout << "FAILED_TO_CREATE_POINT_LIGHT_BUFFER" << std::endl;
	}

	void LightHelper::UpdateDirectionalLightBuffer(ID3D11DeviceContext& pDeviceContext)
	{
		if (!mDirtyDirectionalLight) return;

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = pDeviceContext.Map(mDirectionalLightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		if (SUCCEEDED(hr))
		{
			auto* lightData = reinterpret_cast<DirectionalLight*>(mappedResource.pData);

			lightData->lightDirection = directionalLightParams.lightDirection;
			lightData->lightColor = directionalLightParams.lightColor;
			lightData->lightAmbient = directionalLightParams.lightAmbient;
			lightData->lightDiffuse = directionalLightParams.lightDiffuse;

			pDeviceContext.Unmap(mDirectionalLightBuffer.Get(), 0);
			pDeviceContext.PSSetConstantBuffers(1, 1, mDirectionalLightBuffer.GetAddressOf());

			mDirtyDirectionalLight = false;
		}
	}

	void LightHelper::UpdatePointLightBuffer(ID3D11DeviceContext& pDeviceContext)
	{
		if (!mDirtyPointLight) return;

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = pDeviceContext.Map(mPointLightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		if (SUCCEEDED(hr))
		{
			memcpy(mappedResource.pData, pointLightParams.data(), sizeof(PointLight) * MAX_POINT_LIGHTS);

			pDeviceContext.Unmap(mPointLightBuffer.Get(), 0);
			pDeviceContext.PSSetConstantBuffers(2, 1, mPointLightBuffer.GetAddressOf());

			mDirtyPointLight = false;
		}
	}

	void LightHelper::Release()
	{
		mDirectionalLightBuffer.Reset();
		mPointLightBuffer.Reset();
	}

	void LightHelper::SetPointLightPosition(const USHORT& index, const XMFLOAT3& position)
	{
		if (index < MAX_POINT_LIGHTS)
		{
			mDirtyPointLight = true;
			pointLightParams[index].lightPosition = position;

			if (pointLightSources[index] != nullptr)
			{
				pointLightSources[index]->SetPosition(position);
				/*pointLightSources[index]->GetPosition().x = position.x;
				pointLightSources[index]->GetPosition().y = position.y;
				pointLightSources[index]->GetPosition().z = position.z;
				pointLightSources[index]->SetDirtyTransform(true);*/
			}
		}
	}

	void LightHelper::SetPointLightColor(const USHORT& index, const XMFLOAT4& color)
	{
		if (index < MAX_POINT_LIGHTS)
		{
			mDirtyPointLight = true;
			pointLightParams[index].lightColor = color;
		}
	}

	void LightHelper::SetPointLightAmbient(const USHORT& index, const XMFLOAT4& ambient)
	{
		if (index < MAX_POINT_LIGHTS)
		{
			mDirtyPointLight = true;
			pointLightParams[index].lightAmbient = ambient;
		}
	}

	void LightHelper::SetPointLightDiffuse(const USHORT& index, const XMFLOAT4& diffuse)
	{
		if (index < MAX_POINT_LIGHTS)
		{
			mDirtyPointLight = true;
			pointLightParams[index].lightDiffuse = diffuse;
		}
	}

	void LightHelper::SetPointLightAttenuation(const USHORT& index, const XMFLOAT3& attenuation)
	{
		if (index < MAX_POINT_LIGHTS)
		{
			mDirtyPointLight = true;
			pointLightParams[index].lightAttenuation = attenuation;
		}
	}

	void LightHelper::SetPointLightRange(const USHORT& index, const float& range)
	{
		if (index < MAX_POINT_LIGHTS)
		{
			mDirtyPointLight = true;
			pointLightParams[index].lightRange = range;
		}
	}

	void LightHelper::SetPointLightActive(const USHORT& index, const bool& isActive)
	{
		if (index < MAX_POINT_LIGHTS)
		{
			if (isActive)
			{
				pointLightParams[index].active = 1;
			}
			else pointLightParams[index].active = 0;
		}
	}
}