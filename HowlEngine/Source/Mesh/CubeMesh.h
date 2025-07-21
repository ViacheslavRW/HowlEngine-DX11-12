#pragma once
#include "pch.h"
#include "./Binder.h"
#include "../Renderer/RenderTypes/RenderDataTypes.h"

namespace HEngine
{
	class CubeMeshT : public Binder<TR::Vertex3T>
	{
	public:
		CubeMeshT(ID3D11Device& device, ID3D11DeviceContext& deviceContext, ID3D11InputLayout& inputLayout, TextureManager& textureManager)
			: Binder(device, deviceContext, inputLayout, textureManager) {}

		void Initialize(float size, XMMATRIX& _viewMatrix, XMMATRIX& _projectionMatrix, std::string textureName);
		void Draw(XMMATRIX& viewMatrix);

		std::vector<TR::Vertex3T> vertices;
		std::vector<UINT32> indices;

		std::string texture;

	public:
		XMMATRIX GetModelMartix();

		inline XMFLOAT3& GetPosition() { return mPosition; };
		inline XMFLOAT3& GetRotation() { return mRotation; };
		inline XMFLOAT3& GetScale() { return mScale; };

	private:
		XMFLOAT3 mPosition = {0.0f, 0.0f, 0.0f};
		XMFLOAT3 mRotation = { 0.0f, 0.0f, 0.0f };
		XMFLOAT3 mScale = { 1.0f, 1.0f, 1.0f };

		XMMATRIX mViewMatrix = {};
		XMMATRIX mProjMatrix = {};
		XMMATRIX mModelMatrix = {};

		bool mDirtyTransform = true;
	protected:
		ComPtr<ID3D11Buffer> mVertexBuffer = nullptr;
		ComPtr<ID3D11Buffer> mIndexBuffer = nullptr;
		ComPtr<ID3D11Buffer> mConstantBuffer = nullptr;
	};
}


