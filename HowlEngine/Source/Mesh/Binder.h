#pragma once
#include "pch.h"
#include "../Renderer/DirectX11/TextureManager.h"

namespace HEngine
{
	using namespace Microsoft::WRL;
	using namespace DirectX;

	template<typename T>
	class Binder
	{
	public:
		Binder(ID3D11Device& device, ID3D11DeviceContext& deviceContext, ID3D11InputLayout& inputLayout, TextureManager& textureManager)
			: mDevice(device),
			mDeviceContext(deviceContext),
			mInputLayout(inputLayout),
			mTextureManager(textureManager)
		{
		}

		void CreateVertexBuffer(const std::vector<T>& vertices, ComPtr<ID3D11Buffer>& pVertexBuffer)
		{
			D3D11_BUFFER_DESC bDesc = {};
			bDesc.ByteWidth = static_cast<UINT>(vertices.size() * sizeof(T));
			bDesc.Usage = D3D11_USAGE_DEFAULT;
			bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bDesc.CPUAccessFlags = 0;
			bDesc.MiscFlags = 0;
			bDesc.StructureByteStride = sizeof(T);

			D3D11_SUBRESOURCE_DATA srData = {};
			srData.pSysMem = vertices.data();
			srData.SysMemPitch = 0;
			srData.SysMemSlicePitch = 0;

			HRESULT res = mDevice.CreateBuffer(&bDesc, &srData, pVertexBuffer.GetAddressOf());
			if (FAILED(res)) std::cout << "FAILED_TO_CREATE_VERTEX_BUFFER" << std::endl;
		}

		void CreateIndexBuffer(const std::vector<UINT32>& indices, ComPtr<ID3D11Buffer>& pIndexBuffer)
		{
			D3D11_BUFFER_DESC indBuffDesc = {};
			indBuffDesc.ByteWidth = static_cast<UINT>(sizeof(UINT32) * indices.size());
			indBuffDesc.Usage = D3D11_USAGE_DEFAULT;
			indBuffDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			indBuffDesc.CPUAccessFlags = 0;
			indBuffDesc.MiscFlags = 0;
			indBuffDesc.StructureByteStride = sizeof(UINT32);

			D3D11_SUBRESOURCE_DATA iData = {};
			iData.pSysMem = indices.data();

			HRESULT res = mDevice.CreateBuffer(&indBuffDesc, &iData, pIndexBuffer.GetAddressOf());
			if (FAILED(res)) std::cout << "FAILED_TO_CREATE_INDEX_BUFFER" << std::endl;
		}

		void CreateConstantBuffer(ComPtr<ID3D11Buffer>& pConstantBuffer)
		{
			D3D11_BUFFER_DESC cbDesc = {};
			cbDesc.ByteWidth = sizeof(XMMATRIX);
			cbDesc.Usage = D3D11_USAGE_DEFAULT;
			cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbDesc.CPUAccessFlags = 0;
			cbDesc.MiscFlags = 0;
			cbDesc.StructureByteStride = sizeof(XMMATRIX);

			HRESULT res = mDevice.CreateBuffer(&cbDesc, nullptr, pConstantBuffer.GetAddressOf());
			if (FAILED(res)) std::cout << "FAILED_TO_CREATE_CONSTANT_BUFFER" << std::endl;
		}

		void BindRes(ComPtr<ID3D11Buffer>& pVertexBuffer, ComPtr<ID3D11Buffer>& pIndexBuffer, ComPtr<ID3D11Buffer>& pConstantBuffer, 
			const XMMATRIX& modelMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projMatrix, UINT vertexSize, const std::string& textureName)
		{
			XMMATRIX _mvp = modelMatrix * viewMatrix * projMatrix;
			XMMATRIX m = XMMatrixTranspose(_mvp);
			mDeviceContext.UpdateSubresource(pConstantBuffer.Get(), 0, nullptr, &m, 0, 0);
			mDeviceContext.VSSetConstantBuffers(0, 1, pConstantBuffer.GetAddressOf());

			UINT stride = vertexSize;
			UINT offset = 0;
			mDeviceContext.IASetVertexBuffers(0, 1, pVertexBuffer.GetAddressOf(), &stride, &offset);
			mDeviceContext.IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
			mDeviceContext.IASetInputLayout(&mInputLayout);

			mDeviceContext.PSSetShaderResources(0, 1, mTextureManager.GetTextureSRV(textureName).GetAddressOf());
		}

		void DrawMesh(const size_t& indices)
		{
			mDeviceContext.DrawIndexed(indices, 0, 0);
		}

	protected:
		ID3D11Device& mDevice;
		ID3D11DeviceContext& mDeviceContext;
		ID3D11InputLayout& mInputLayout;
		TextureManager& mTextureManager;
	};
}
