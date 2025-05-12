#include "pch.h"
#include "CubeMesh.h"

namespace HEngine
{
    void CubeMeshT::Initialize(std::vector<TR::Vertex3T> _vertices, std::vector<UINT16> _indices,
        XMMATRIX& _viewMatrix, XMMATRIX& _projectionMatrix, std::string textureName)
    {
        vertices = std::move(_vertices);
        indices = std::move(_indices);
        mViewMatrix = _viewMatrix;
        mProjMatrix = _projectionMatrix;
        texture = textureName;

        CreateVertexBuffer(vertices, mVertexBuffer);
        CreateIndexBuffer(indices, mIndexBuffer);
        CreateConstantBuffer(mConstantBuffer);
    }

    void CubeMeshT::Bind(XMMATRIX& viewMatrix)
    {
        BindRes(mVertexBuffer, mIndexBuffer, mConstantBuffer, GetModelMartix(), viewMatrix, mProjMatrix, sizeof(TR::Vertex3T), texture);
    }

    void CubeMeshT::Draw()
    {
        Binder::DrawMesh(indices.size());
    }

    XMMATRIX CubeMeshT::GetModelMartix() const
    {
        XMMATRIX scale = XMMatrixScaling(mScale.x, mScale.y, mScale.z);
        XMMATRIX rotation = XMMatrixRotationRollPitchYaw(mRotation.x, mRotation.y, mRotation.z);
        XMMATRIX position = XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);

        return scale * rotation * position;
    }
}