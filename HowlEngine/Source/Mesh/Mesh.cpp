#include "pch.h"
#include "Mesh.h"

namespace HEngine
{
	void Mesh::Initialize(XMMATRIX& _viewMatrix, XMMATRIX& _projectionMatrix)
	{
		mViewMatrix = _viewMatrix;
		mProjMatrix = _projectionMatrix;
	}

	void Mesh::CreateBuffers()
	{
		CreateVertexBuffer(vertices, mVertexBuffer);
		CreateIndexBuffer(indices, mIndexBuffer);
		CreateConstantBuffer(mConstantBuffer);
	}

    void Mesh::Bind(XMMATRIX& viewMatrix)
    {
        BindRes(mVertexBuffer, mIndexBuffer, mConstantBuffer, GetModelMartix(), viewMatrix, mProjMatrix, sizeof(TR::Vertex3T), texture);
    }

    void Mesh::Draw()
    {
        Binder::DrawMesh(indices.size());
    }

    XMMATRIX Mesh::GetModelMartix() const
    {
        XMMATRIX scale = XMMatrixScaling(mScale.x, mScale.y, mScale.z);
        XMMATRIX rotation = XMMatrixRotationRollPitchYaw(mRotation.x, mRotation.y, mRotation.z);
        XMMATRIX position = XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);

        return scale * rotation * position;
    }
}