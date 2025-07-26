#pragma once

#include "../Renderer/RenderTypes/RenderDataTypes.h"
#include "./MaterialManager.h"

namespace HEngine
{
	using namespace Microsoft::WRL;

	struct PBRSubMesh
	{
		std::vector<TR::PBRVertex> vertices;
		std::vector<UINT32> indices;
		PBRMaterial material;

		ComPtr<ID3D11Buffer> mVertexBuffer = nullptr;
		ComPtr<ID3D11Buffer> mIndexBuffer = nullptr;
		ComPtr<ID3D11Buffer> mConstantBuffer = nullptr;
	};

	struct PBRMesh
	{
	public:
		void Initialize(XMMATRIX& _viewMatrix, XMMATRIX& _projectionMatrix);

		std::vector<PBRSubMesh> subMeshes;
		PBRMesh* childMesh = nullptr;
	public:
		XMMATRIX GetModelMartix();

		inline XMFLOAT3& GetPosition() { return mPosition; };
		inline XMFLOAT3& GetRotation() { return mRotation; };
		inline XMFLOAT3& GetScale() { return mScale; };
		void SetPosition(XMFLOAT3 position);
		void SetRotation(XMFLOAT3& rotation);
		void SetScale(XMFLOAT3& scale);

		inline void SetDirtyTransform(const bool& isDirty) { mDirtyTransform = isDirty; };
	public:
		std::string modelPath = "";
		std::wstring texturesPath = L"";

		XMMATRIX mViewMatrix = {};
		XMMATRIX mProjMatrix = {};
		XMMATRIX mModelMatrix = {};
	public:
		XMFLOAT3 mPosition = { 0.0f, 0.0f, 0.0f };
		XMFLOAT3 mRotation = { 0.0f, 0.0f, 0.0f };
		XMFLOAT3 mScale = { 1.0f, 1.0f, 1.0f };

		bool mDirtyTransform = true;
	};
}
