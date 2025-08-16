#pragma once

#include "../Renderer/RenderTypes/RenderDataTypes.h"
#include "./MaterialManager.h"

namespace HEngine
{
	using namespace Microsoft::WRL;

	struct Transform
	{
	public:
		XMFLOAT3 mPosition = { 0.0f, 0.0f, 0.0f };
		XMFLOAT3 mRotation = { 0.0f, 0.0f, 0.0f };
		XMFLOAT3 mScale = { 1.0f, 1.0f, 1.0f };

		XMMATRIX mModelMatrix = {};
		bool mDirtyTransform = true;

		XMMATRIX GetModelMartix()
		{
			if (mDirtyTransform)
			{
				XMMATRIX scale = XMMatrixScaling(mScale.x, mScale.y, mScale.z);
				XMMATRIX rotation = XMMatrixRotationRollPitchYaw(mRotation.x, mRotation.y, mRotation.z);
				XMMATRIX position = XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);

				mModelMatrix = scale * rotation * position;
				mDirtyTransform = false;
			}

			return mModelMatrix;
		}

		inline XMFLOAT3& GetPosition() { return mPosition; };
		inline XMFLOAT3& GetRotation() { return mRotation; };
		inline XMFLOAT3& GetScale() { return mScale; };

		void SetPosition(XMFLOAT3 position)
		{
			mPosition = position;
			mDirtyTransform = true;
		}
		void SetRotation(XMFLOAT3& rotation)
		{
			mRotation = rotation;
			mDirtyTransform = true;
		}
		void SetScale(XMFLOAT3& scale)
		{
			mScale = scale;
			mDirtyTransform = true;
		}

		inline void SetDirtyTransform(const bool& isDirty) { mDirtyTransform = isDirty; };
	};

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
		std::vector<PBRSubMesh> subMeshes;
		Transform transform;

	public:
		std::string modelPath = "";
		std::wstring texturesPath = L"";
	};
}
