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
        for (auto& subMesh : subMeshes)
        {
            CreateVertexBuffer(subMesh.vertices, subMesh.mVertexBuffer);
            CreateIndexBuffer(subMesh.indices, subMesh.mIndexBuffer);
            CreateConstantBuffer(subMesh.mConstantBuffer);
        }
	}

    void Mesh::Draw(XMMATRIX& viewMatrix)
    {
        for (auto& subMesh : subMeshes)
        {
            BindRes(subMesh.mVertexBuffer, subMesh.mIndexBuffer, subMesh.mConstantBuffer, GetModelMartix(), viewMatrix, mProjMatrix, sizeof(TR::Vertex3T), subMesh.texture);
            Binder::DrawMesh(subMesh.indices.size());
        }
    }

    XMMATRIX Mesh::GetModelMartix() const
    {
        XMMATRIX scale = XMMatrixScaling(mScale.x, mScale.y, mScale.z);
        XMMATRIX rotation = XMMatrixRotationRollPitchYaw(mRotation.x, mRotation.y, mRotation.z);
        XMMATRIX position = XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);

        return scale * rotation * position;
    }
}