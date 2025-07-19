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
            BindRes(subMesh.mVertexBuffer, subMesh.mIndexBuffer, subMesh.mConstantBuffer, GetModelMartix(), viewMatrix, mProjMatrix,
                sizeof(TR::Vertex3T), subMesh.texture);
            Binder::DrawMesh(subMesh.indices.size());
        }
    }

    XMMATRIX Mesh::GetModelMartix()
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

    void Mesh::SetPosition(XMFLOAT3 position)
    {
        mPosition.x = position.x;
        mPosition.y = position.y;
        mPosition.z = position.z;
        if (childMesh != nullptr)
        {
            childMesh->GetPosition().x = position.x;
            childMesh->GetPosition().y = position.y;
            childMesh->GetPosition().z = position.z;
            childMesh->mDirtyTransform = true;
        }
        mDirtyTransform = true;
    }

    void Mesh::SetRotation(XMFLOAT3& rotation)
    {
        mRotation.x = rotation.x;
        mRotation.y = rotation.y;
        mRotation.z = rotation.z;
        if (childMesh != nullptr) childMesh->SetRotation(rotation);
        mDirtyTransform = true;
    }

    void Mesh::SetScale(XMFLOAT3& scale)
    {
        mScale.x = scale.x;
        mScale.y = scale.y;
        mScale.z = scale.z;
        if (childMesh != nullptr) childMesh->SetScale(scale);
        mDirtyTransform = true;
    }
}