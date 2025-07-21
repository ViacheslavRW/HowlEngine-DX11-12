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

		mesh1->modelPath = "Models/Nikke_06.obj";
		mesh1->texturesPath = L"Nikke_06/";
		mesh2->modelPath = "Models/Eve_16_2.obj";
		mesh2->texturesPath = L"Eve_16/";
		mesh3->modelPath = "Models/Light/LightBulb1Glass.obj";
		mesh4->modelPath = "Models/Light/LightBulb1Bottom.obj";

		mMeshLoader->LoadMesh(mesh1.get(), mesh1->modelPath, mesh1->texturesPath);
		mMeshLoader->LoadMesh(mesh2.get(), mesh2->modelPath, mesh2->texturesPath);
		mMeshLoader->LoadMesh(mesh3.get(), mesh3->modelPath, mesh3->texturesPath);
		mMeshLoader->LoadMesh(mesh4.get(), mesh4->modelPath, mesh4->texturesPath);

		meshes.push_back(std::move(mesh1));
		meshes.push_back(std::move(mesh2));
		meshes.push_back(std::move(mesh3));
		meshes.push_back(std::move(mesh4));

		for (int i = 0; i < meshes.size(); ++i)
		{
			meshes[i]->Initialize(cashedViewMatrix, cashedProjMatrix);
		}

		meshes[0]->GetScale() = { 1.2f, 1.2f, 1.2f };
		meshes[0]->GetPosition().y = -0.8f;
		meshes[0]->GetPosition().x = 2.5f;
		meshes[0]->GetRotation().y = -15.0f;

		meshes[1]->GetScale() = {0.4f, 0.4f, 0.4f};
		meshes[1]->GetPosition().y = -0.8f;
		meshes[1]->GetRotation().y = -15.0f;

		meshes[2]->GetPosition().x = -1.8f;
		meshes[2]->GetPosition().y = -0.2f;
		meshes[3]->GetPosition().x = -1.8f;
		meshes[3]->GetPosition().y = -0.2f;
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
		mDeviceContext->PSSetShaderResources(2, 1, &material.metallicSRV);
		mDeviceContext->PSSetShaderResources(3, 1, &material.roughnessSRV);
		mDeviceContext->PSSetShaderResources(4, 1, &material.aoSRV);
		mDeviceContext->PSSetShaderResources(5, 1, &material.ormSRV);
	}

	void MeshManager::Draw(const size_t& indices)
	{
		mDeviceContext->DrawIndexed(indices, 0, 0);
	}

	void MeshManager::Render(const XMMATRIX& viewMatrix)
	{
		for (int i = 0; i < meshes.size(); ++i)
		{
			for (auto& subMesh : meshes[i]->subMeshes)
			{
				Bind(subMesh.mVertexBuffer, subMesh.mIndexBuffer, subMesh.mConstantBuffer, meshes[i]->GetModelMartix(), viewMatrix, subMesh.material, sizeof(TR::PBRVertex));
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

