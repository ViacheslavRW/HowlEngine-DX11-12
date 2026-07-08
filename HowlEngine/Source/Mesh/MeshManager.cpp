#include "pch.h"
#include "MeshManager.h"

namespace HEngine
{
	void HEngine::MeshManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ID3D11InputLayout* pInputLayout,
		TextureManager* pTextureManager, MeshLoader* pMeshLoader, XMMATRIX& _viewMatrix, XMMATRIX& _projectionMatrix)
	{
		mDevice = pDevice;
		mDeviceContext = pDeviceContext;
		mInputLayout = pInputLayout;
		mTextureManager = pTextureManager;
		mMeshLoader = pMeshLoader;
		cashedViewMatrix = _viewMatrix;
		cashedProjMatrix = _projectionMatrix;
	}

	void MeshManager::InitializeAllMeshes()
	{
		mesh1 = std::make_unique<PBRMesh>();
		mesh2 = std::make_unique<PBRMesh>();
		mesh3 = std::make_unique<PBRMesh>();
		mesh4 = std::make_unique<PBRMesh>();
		mesh5 = std::make_unique<PBRMesh>();
		mesh6 = std::make_unique<PBRMesh>();
	}

	void MeshManager::CreateAllBuffers()
	{
		for (int i = 0; i < meshes.size(); ++i)
		{
			for (auto& subMesh : meshes[i]->subMeshes)
			{
				CreateVertexBuffer(subMesh.vertices, subMesh.mVertexBuffer);
				CreateIndexBuffer(subMesh.indices, subMesh.mIndexBuffer);
				CreateConstantBuffer(subMesh.mConstantBuffer);
			}
		}

		for (int j = 0; j < meshesTransparent.size(); ++j)
		{
			for (auto& subMesh : meshesTransparent[j]->subMeshes)
			{
				CreateVertexBuffer(subMesh.vertices, subMesh.mVertexBuffer);
				CreateIndexBuffer(subMesh.indices, subMesh.mIndexBuffer);
				CreateConstantBuffer(subMesh.mConstantBuffer);
			}
		}
	}

	void MeshManager::CreateVertexBuffer(const std::vector<TR::PBRVertex>& _vertices, ComPtr<ID3D11Buffer>& _vertexBuffer)
	{
		D3D11_BUFFER_DESC bDesc = {};
		bDesc.ByteWidth = static_cast<UINT>(_vertices.size() * sizeof(TR::PBRVertex));
		bDesc.Usage = D3D11_USAGE_DEFAULT;
		bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bDesc.CPUAccessFlags = 0;
		bDesc.MiscFlags = 0;
		bDesc.StructureByteStride = sizeof(TR::PBRVertex);

		D3D11_SUBRESOURCE_DATA srData = {};
		srData.pSysMem = _vertices.data();
		srData.SysMemPitch = 0;
		srData.SysMemSlicePitch = 0;

		mmRes = mDevice->CreateBuffer(&bDesc, &srData, _vertexBuffer.GetAddressOf());
		if (FAILED(mmRes)) std::cout << "FAILED_TO_CREATE_VERTEX_BUFFER" << std::endl;
	}

	void MeshManager::CreateIndexBuffer(const std::vector<UINT32>& indices, ComPtr<ID3D11Buffer>& _indexBuffer)
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

		mmRes = mDevice->CreateBuffer(&indBuffDesc, &iData, _indexBuffer.GetAddressOf());
		if (FAILED(mmRes)) std::cout << "FAILED_TO_CREATE_INDEX_BUFFER" << std::endl;
	}

	void MeshManager::CreateConstantBuffer(ComPtr<ID3D11Buffer>& _constantBuffer)
	{
		D3D11_BUFFER_DESC cbDesc = {};
		cbDesc.ByteWidth = sizeof(CBMatrices);
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0;

	    mmRes = mDevice->CreateBuffer(&cbDesc, nullptr, _constantBuffer.GetAddressOf());
		if (FAILED(mmRes)) std::cout << "FAILED_TO_CREATE_CONSTANT_BUFFER" << std::endl;
	}

	void MeshManager::Bind(const ComPtr<ID3D11Buffer>& _vertexBuffer, const ComPtr<ID3D11Buffer>& _indexBuffer, 
		const ComPtr<ID3D11Buffer>& _constantBuffer, const XMMATRIX& modelMatrix, const XMMATRIX& viewMatrix, const PBRMaterial& material, const UINT& vertexSize)
	{
		CBMatrices matrices;
		matrices.modelMatrix = XMMatrixTranspose(modelMatrix);
		matrices.mvpMatrix = XMMatrixTranspose(modelMatrix * viewMatrix * cashedProjMatrix);

		D3D11_MAPPED_SUBRESOURCE mappedResource = {};
		HRESULT hr = mDeviceContext->Map(_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (SUCCEEDED(hr))
		{
			std::memcpy(mappedResource.pData, &matrices, sizeof(CBMatrices));
			mDeviceContext->Unmap(_constantBuffer.Get(), 0);
		}

		mDeviceContext->VSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());

		UINT stride = vertexSize;
		UINT offset = 0;
		mDeviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
		mDeviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		mDeviceContext->IASetInputLayout(mInputLayout);

		mDeviceContext->PSSetShaderResources(0, 1, &material.albedoSRV);
		mDeviceContext->PSSetShaderResources(1, 1, &material.normalSRV);
		mDeviceContext->PSSetShaderResources(2, 1, &material.ormSRV);
		mDeviceContext->PSSetShaderResources(3, 1, &material.emissiveSRV);
	}

	void MeshManager::Draw(const size_t& indices)
	{
		mDeviceContext->DrawIndexed(indices, 0, 0);
	}

	void MeshManager::Render(const XMMATRIX& viewMatrix)
	{
		for (int i = 0; i < meshes.size(); ++i)
		{
			for (const auto& subMesh : meshes[i]->subMeshes)
			{
				Bind(subMesh.mVertexBuffer, subMesh.mIndexBuffer, subMesh.mConstantBuffer, meshes[i]->transform.GetModelMartix(), viewMatrix, subMesh.material, sizeof(TR::PBRVertex));
				Draw(subMesh.indices.size());
			}
		}
	}

	void MeshManager::RenderTransparent(const XMMATRIX& viewMatrix)
	{
		for (int i = 0; i < meshesTransparent.size(); ++i)
		{
			for (const auto& subMesh : meshesTransparent[i]->subMeshes)
			{
				Bind(subMesh.mVertexBuffer, subMesh.mIndexBuffer, subMesh.mConstantBuffer, meshesTransparent[i]->transform.GetModelMartix(), viewMatrix, subMesh.material, sizeof(TR::PBRVertex));
				Draw(subMesh.indices.size());
			}
		}
	}

	void MeshManager::Release()
	{
		mDevice = nullptr;
		mDeviceContext = nullptr;
		mInputLayout = nullptr;
		mTextureManager = nullptr;
		mMeshLoader = nullptr;
	};
}

