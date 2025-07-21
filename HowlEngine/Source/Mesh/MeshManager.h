#pragma once
#include "../Common/TextureManager.h"
#include "./PBRMesh.h"
#include "./MeshLoader.h"

namespace HEngine
{
	struct CBMatrices
	{
		XMMATRIX mvpMatrix;
		XMMATRIX modelMatrix;
	};

	class MeshManager
	{
	public:
		MeshManager() = default;
		~MeshManager() = default;

		MeshManager(const MeshManager&) = delete;
		MeshManager& operator=(const MeshManager&) = delete;

		void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ID3D11InputLayout* pInputLayout,
			TextureManager* pTextureManager, MeshLoader* pMeshLoader, XMMATRIX& _viewMatrix, XMMATRIX& _projectionMatrix);
		void InitializeAllMeshes();

		void CreateAllBuffers();
		void CreateVertexBuffer(const std::vector<TR::PBRVertex>& _vertices, ComPtr<ID3D11Buffer>& _vertexBuffer);
		void CreateIndexBuffer(const std::vector<UINT32>& indices, ComPtr<ID3D11Buffer>& _indexBuffer);
		void CreateConstantBuffer(ComPtr<ID3D11Buffer>& _constantBuffer);

		void Bind(const ComPtr<ID3D11Buffer>& _vertexBuffer, const ComPtr<ID3D11Buffer>& _indexBuffer, const ComPtr<ID3D11Buffer>& _constantBuffer,
			const XMMATRIX& modelMatrix, const XMMATRIX& viewMatrix, const PBRMaterial& material, const UINT& vertexSize);
		void Draw(const size_t& indices);
		void Render(const XMMATRIX& viewMatrix);

		void Release();
		std::vector<std::unique_ptr<PBRMesh>> meshes;

	private:
		XMMATRIX cashedViewMatrix;
		XMMATRIX cashedProjMatrix;

		ID3D11Device* mDevice;
		ID3D11DeviceContext* mDeviceContext;
		ID3D11InputLayout* mInputLayout;
		TextureManager* mTextureManager;
		MeshLoader* mMeshLoader;

		HRESULT mmRes;
		std::unique_ptr<PBRMesh> mesh1;
		std::unique_ptr<PBRMesh> mesh2;
		std::unique_ptr<PBRMesh> mesh3;
		std::unique_ptr<PBRMesh> mesh4;
	};
}


