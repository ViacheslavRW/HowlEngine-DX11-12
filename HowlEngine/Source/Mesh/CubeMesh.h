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

		void Initialize(std::vector<TR::Vertex3T> _vertices, std::vector<UINT16> _indices, XMMATRIX& _viewMatrix, XMMATRIX& _projectionMatrix, std::string textureName);
		void BindRes(XMMATRIX& viewMatrix);
		void Draw();

		std::vector<TR::Vertex3T> vertices;
		std::vector<UINT16> indices;

		std::string texture;

	public:
		XMMATRIX GetModelMartix() const;

		inline void SetPosition(float x, float y, float z) { mPosition = { x, y, z }; };
		inline void SetRotation(float x, float y, float z) { mRotation = { x, y, z }; };
		inline void SetScale(float x, float y, float z) { mScale = { x, y, z }; };

	private:
		XMFLOAT3 mPosition = {0.0f, 0.0f, 0.0f};
		XMFLOAT3 mRotation = { 0.0f, 0.0f, 0.0f };
		XMFLOAT3 mScale = { 1.0f, 1.0f, 1.0f };

		XMMATRIX mViewMatrix;
		XMMATRIX mProjMatrix;

	protected:
		ComPtr<ID3D11Buffer> mVertexBuffer = nullptr;
		ComPtr<ID3D11Buffer> mIndexBuffer = nullptr;
		ComPtr<ID3D11Buffer> mConstantBuffer = nullptr;
	};

	class CubeMeshC : public Binder<TR::Vertex3C>
	{
	public:
	};
}


