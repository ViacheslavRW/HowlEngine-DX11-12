#include "pch.h"
#include "PBRMesh.h"

namespace HEngine
{
    void PBRMesh::Initialize(XMMATRIX& _viewMatrix, XMMATRIX& _projectionMatrix)
    {
        mViewMatrix = _viewMatrix;
        mProjMatrix = _projectionMatrix;
    }

    XMMATRIX PBRMesh::GetModelMartix()
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

    void PBRMesh::SetPosition(XMFLOAT3 position)
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

    void PBRMesh::SetRotation(XMFLOAT3& rotation)
    {
        mRotation.x = rotation.x;
        mRotation.y = rotation.y;
        mRotation.z = rotation.z;
        if (childMesh != nullptr) childMesh->SetRotation(rotation);
        mDirtyTransform = true;
    }

    void PBRMesh::SetScale(XMFLOAT3& scale)
    {
        mScale.x = scale.x;
        mScale.y = scale.y;
        mScale.z = scale.z;
        if (childMesh != nullptr) childMesh->SetScale(scale);
        mDirtyTransform = true;
    }
}