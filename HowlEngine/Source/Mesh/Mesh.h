#pragma once

#include "./Binder.h"
#include "../Renderer/RenderTypes/RenderDataTypes.h"

namespace HEngine
{
	struct SubMesh
	{
		std::vector<TR::Vertex3T> vertices;
		std::vector<UINT32> indices;
		std::string texture = "no_texture";

		ComPtr<ID3D11Buffer> mVertexBuffer = nullptr;
		ComPtr<ID3D11Buffer> mIndexBuffer = nullptr;
		ComPtr<ID3D11Buffer> mConstantBuffer = nullptr;
	};

	class Mesh : public Binder<TR::Vertex3T>
	{
	public:
		Mesh(ID3D11Device& device, ID3D11DeviceContext& deviceContext, ID3D11InputLayout& inputLayout, TextureManager& textureManager)
			: Binder(device, deviceContext, inputLayout, textureManager) {
		}
		void Initialize(XMMATRIX& _viewMatrix, XMMATRIX& _projectionMatrix);
		void CreateBuffers();
		void Draw(XMMATRIX& viewMatrix);

		std::vector<SubMesh> subMeshes;
	public:
		XMMATRIX GetModelMartix();

		inline XMFLOAT3& GetPosition() { return mPosition; };
		inline XMFLOAT3& GetRotation() { return mRotation; };
		inline XMFLOAT3& GetScale() { return mScale; };

	private:
		XMFLOAT3 mPosition = { 0.0f, 0.0f, 0.0f };
		XMFLOAT3 mRotation = { 0.0f, 0.0f, 0.0f };
		XMFLOAT3 mScale = { 1.0f, 1.0f, 1.0f };

		XMMATRIX mViewMatrix = {};
		XMMATRIX mProjMatrix = {};
		XMMATRIX mModelMatrix = {};

		bool mDirtyTransform = true;
	};
}


