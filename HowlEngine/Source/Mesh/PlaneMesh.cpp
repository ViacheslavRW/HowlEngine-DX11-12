#include "pch.h"
#include "PlaneMesh.h"

namespace HEngine
{
	void PlaneMeshT::Initialize(float width, float height, UINT16 divisionsX, UINT16 divisionsZ,
		XMMATRIX& _viewMatrix, XMMATRIX& _projectionMatrix, std::string textureName)
	{
		// values
		const UINT16 vertX = divisionsX + 1;
		const UINT16 vertZ = divisionsZ + 1;

		const float halfW = width * 0.5f;
		const float halfH = height * 0.5f;

		const float stepX = width / static_cast<float>(divisionsX);
		const float stepZ = height / static_cast<float>(divisionsZ);

		mViewMatrix = _viewMatrix;
		mProjMatrix = _projectionMatrix;
		texture = textureName;

		// vertices
		for (UINT z = 0; z < vertZ; ++z)
		{
			for (UINT x = 0; x < vertX; ++x)
			{
				float px = (-halfW) + x * stepX;
				float pz = (-halfH) + z * stepZ;

				TR::Vertex3T v = {};
				v.position = { px, 0.0f, pz };
				v.normal = { 0.0f, 0.0f, 0.0f };
				v.textureCoord = { static_cast<float>(x) / divisionsX, static_cast<float>(z) / divisionsZ };

				vertices.push_back(v);
			}
		}
		// indices
		auto idx = [vertX](UINT16 x, UINT16 z) -> UINT16
		{
			return z * vertX + x;
		};

		for (UINT z = 0; z < divisionsZ; ++z)
		{
			for (UINT x = 0; x < divisionsX; ++x)
			{
				UINT16 i0 = idx(x, z);
				UINT16 i1 = idx(x + 1, z);
				UINT16 i2 = idx(x, z + 1);
				UINT16 i3 = idx(x + 1, z + 1);

				indices.push_back(i0);
				indices.push_back(i1);
				indices.push_back(i2);

				indices.push_back(i2);
				indices.push_back(i1);
				indices.push_back(i3);
			}
		}
		// buffers
		CreateVertexBuffer(vertices, mVertexBuffer);
		CreateIndexBuffer(indices, mIndexBuffer);
		CreateConstantBuffer(mConstantBuffer);
	}

	void PlaneMeshT::Bind(XMMATRIX& viewMatrix)
	{
		BindRes(mVertexBuffer, mIndexBuffer, mConstantBuffer, GetModelMartix(), viewMatrix, mProjMatrix, sizeof(TR::Vertex3T), texture);
	}

	void PlaneMeshT::Draw()
	{
		Binder::DrawMesh(indices.size());
	}

	XMMATRIX PlaneMeshT::GetModelMartix() const
	{
		XMMATRIX scale = XMMatrixScaling(mScale.x, mScale.y, mScale.z);
		XMMATRIX rotation = XMMatrixRotationRollPitchYaw(mRotation.x, mRotation.y, mRotation.z);
		XMMATRIX position = XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);

		return scale * rotation * position;
	}
}