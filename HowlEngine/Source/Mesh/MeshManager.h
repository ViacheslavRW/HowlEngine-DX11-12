#pragma once
#include "../Common/TextureManager.h"
#include "./PBRMesh.h"
#include "./MeshLoader.h"
#include "../Input/InputManager.h"

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

		void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext,
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
		void RenderTransparent(const XMMATRIX& viewMatrix);

		void Release();

		inline void SetInputLayoutPBR(ID3D11InputLayout* pInputLayout) { mInputLayoutPBR = pInputLayout; };
		inline void SetInputLayoutBox(ID3D11InputLayout* pInputLayout) { mInputLayoutBox = pInputLayout; };

	public:
		// AABB
		bool RayAABBIntersect(const Ray& ray, const XMFLOAT3& aabbMin, const XMFLOAT3& aabbMax, float& t);
	
		void CreateDebugBoxBuffers();
		void DrawDebugBox(const XMFLOAT3& min, const XMFLOAT3& max);
		void UploadDebugVertices();
		void RenderDebugVertices();
		void DebugDrawAllAABBs(const XMMATRIX& viewMatrix);

		std::vector<TR::DebugVertex> mDebugVertices;
		const XMFLOAT4 boxColor = { 1.0f, 0.1f, 0.1f, 1.0f };
		ComPtr<ID3D11Buffer> mDebugBoxSharedVBuffer = nullptr;
		ComPtr<ID3D11Buffer> mDebugBoxSharedCBuffer = nullptr;
	public:
		std::vector<std::unique_ptr<PBRMesh>> meshes;
		std::vector<std::unique_ptr<PBRMesh>> meshesTransparent;
		MeshLoader* mMeshLoader;

	private:
		XMMATRIX cashedViewMatrix;
		XMMATRIX cashedProjMatrix;

		ID3D11Device* mDevice;
		ID3D11DeviceContext* mDeviceContext;
		ID3D11InputLayout* mInputLayoutPBR;
		ID3D11InputLayout* mInputLayoutBox;
		TextureManager* mTextureManager;

		HRESULT mmRes;
		std::unique_ptr<PBRMesh> mesh1;
		std::unique_ptr<PBRMesh> mesh2;
		std::unique_ptr<PBRMesh> mesh3;
		std::unique_ptr<PBRMesh> mesh4;
		std::unique_ptr<PBRMesh> mesh5;
		std::unique_ptr<PBRMesh> mesh6;
		std::unique_ptr<PBRMesh> mesh7;
	};
}


