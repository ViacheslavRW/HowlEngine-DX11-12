#include "pch.h"
#include "MeshManager.h"

namespace HEngine
{
	void HEngine::MeshManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext,
		TextureManager* pTextureManager, MeshLoader* pMeshLoader, XMMATRIX& _viewMatrix, XMMATRIX& _projectionMatrix)
	{
		mDevice = pDevice;
		mDeviceContext = pDeviceContext;
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
		mDeviceContext->IASetInputLayout(mInputLayoutPBR);

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
		mInputLayoutPBR = nullptr;
		mInputLayoutBox = nullptr;
		mTextureManager = nullptr;
		mMeshLoader = nullptr;
	}

	bool MeshManager::RayAABBIntersect(const Ray& ray, const XMFLOAT3& aabbMin, const XMFLOAT3& aabbMax, float& t)
	{
		XMFLOAT3 origin;
		XMFLOAT3 dir;

		XMStoreFloat3(&origin, ray.origin);
		XMStoreFloat3(&dir, ray.direction);

		float tmin = 0.0f;
		float tmax = FLT_MAX;

		const float epsilon = 1e-8f;

		for (int i = 0; i < 3; i++)
		{
			float o, d, mn, mx;

			if (i == 0)
			{
				o = origin.x;
				d = dir.x;
				mn = aabbMin.x;
				mx = aabbMax.x;
			}
			else if (i == 1)
			{
				o = origin.y;
				d = dir.y;
				mn = aabbMin.y;
				mx = aabbMax.y;
			}
			else
			{
				o = origin.z;
				d = dir.z;
				mn = aabbMin.z;
				mx = aabbMax.z;
			}

			if (fabs(d) < epsilon)
			{
				if (o < mn || o > mx) return false;
			}
			else
			{
				float inv = 1.0f / d;

				float t1 = (mn - o) * inv;
				float t2 = (mx - o) * inv;

				if (t1 > t2) std::swap(t1, t2);

				tmin = std::max(tmin, t1);
				tmax = std::min(tmax, t2);

				if (tmin > tmax) return false;
			}
		}
		t = tmin;

		return true;
	}

	void MeshManager::CreateDebugBoxBuffers()
	{
		// vert buffer
		D3D11_BUFFER_DESC vbDesc = {};
		vbDesc.ByteWidth = sizeof(TR::DebugVertex) * 2048;
		vbDesc.Usage = D3D11_USAGE_DYNAMIC;
		vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		mmRes = mDevice->CreateBuffer(&vbDesc, nullptr, mDebugBoxSharedVBuffer.GetAddressOf());
		if (FAILED(mmRes)) std::cout << "FAILED_TO_CREATE_VERTEX_BOX_BUFFER" << std::endl;

		// const buffer
		D3D11_BUFFER_DESC cbDesc = {};
		cbDesc.ByteWidth = sizeof(XMMATRIX);
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		mmRes = mDevice->CreateBuffer(&cbDesc, nullptr, mDebugBoxSharedCBuffer.GetAddressOf());
		if (FAILED(mmRes)) std::cout << "FAILED_TO_CREATE_CONSTANT_BOX_BUFFER" << std::endl;

		mDebugVertices.reserve(2048);
	}

	void MeshManager::DrawDebugBox(const XMFLOAT3& min, const XMFLOAT3& max)
	{
		static constexpr int edges[12][2] =
		{
			{0,1},{1,2},{2,3},{3,0},
			{4,5},{5,6},{6,7},{7,4},
			{0,4},{1,5},{2,6},{3,7}
		};

		XMFLOAT3 p[8] = {
			{min.x, min.y, min.z}, {max.x, min.y, min.z},
			{max.x, max.y, min.z}, {min.x, max.y, min.z},
			{min.x, min.y, max.z}, {max.x, min.y, max.z},
			{max.x, max.y, max.z}, {min.x, max.y, max.z}
		};

		for (int i = 0; i < 12; ++i)
		{
			mDebugVertices.push_back({ p[edges[i][0]], boxColor });
			mDebugVertices.push_back({ p[edges[i][1]], boxColor });
		}
	}

	void MeshManager::UploadDebugVertices()
	{

		if (mDebugVertices.empty()) return;

		D3D11_MAPPED_SUBRESOURCE mapped;

		mDeviceContext->Map(mDebugBoxSharedVBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
		memcpy(mapped.pData, mDebugVertices.data(), mDebugVertices.size() * sizeof(TR::DebugVertex));
		mDeviceContext->Unmap(mDebugBoxSharedVBuffer.Get(),0);
	}

	void MeshManager::RenderDebugVertices()
	{
		if (mDebugVertices.empty()) return;

		UINT stride = sizeof(TR::DebugVertex);
		UINT offset = 0;

		mDeviceContext->IASetInputLayout(mInputLayoutBox);
		mDeviceContext->IASetVertexBuffers(0, 1, mDebugBoxSharedVBuffer.GetAddressOf(), &stride, &offset);
		mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		mDeviceContext->Draw(static_cast<UINT>(mDebugVertices.size()),0);
	}

	void MeshManager::DebugDrawAllAABBs(const XMMATRIX& viewMatrix)
	{
		mDebugVertices.clear();

		XMMATRIX viewProj = XMMatrixTranspose(viewMatrix * cashedProjMatrix);
		D3D11_MAPPED_SUBRESOURCE mapped;
		mDeviceContext->Map(mDebugBoxSharedCBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
		memcpy(mapped.pData, &viewProj, sizeof(XMMATRIX));
		mDeviceContext->Unmap(mDebugBoxSharedCBuffer.Get(), 0);
		mDeviceContext->VSSetConstantBuffers(0, 1, mDebugBoxSharedCBuffer.GetAddressOf());

		for (const auto& mesh : meshes)
		{
			XMMATRIX world = mesh->transform.GetModelMartix();

			for (const auto& sub : mesh->subMeshes)
			{
				XMVECTOR localMin = XMLoadFloat3(&sub.aabb.min);
				XMVECTOR localMax = XMLoadFloat3(&sub.aabb.max);

				XMVECTOR wMin = XMVector3Transform(localMin, world);
				XMVECTOR wMax = XMVector3Transform(localMax, world);

				XMFLOAT3 min, max;
				XMStoreFloat3(&min, wMin);
				XMStoreFloat3(&max, wMax);

				DrawDebugBox(min, max);
			}
		}

		for (const auto& meshTr : meshesTransparent)
		{
			XMMATRIX world = meshTr->transform.GetModelMartix();

			for (const auto& subTr : meshTr->subMeshes)
			{
				XMVECTOR localMin = XMLoadFloat3(&subTr.aabb.min);
				XMVECTOR localMax = XMLoadFloat3(&subTr.aabb.max);

				XMVECTOR wMin = XMVector3Transform(localMin, world);
				XMVECTOR wMax = XMVector3Transform(localMax, world);

				XMFLOAT3 min, max;
				XMStoreFloat3(&min, wMin);
				XMStoreFloat3(&max, wMax);

				DrawDebugBox(min, max);
			}
		}

		UploadDebugVertices();
		RenderDebugVertices();
	}
}

