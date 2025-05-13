#include "pch.h"
#include "CubeMesh.h"

namespace HEngine
{
    void CubeMeshT::Initialize(float size, XMMATRIX& _viewMatrix, XMMATRIX& _projectionMatrix, std::string textureName)
    {
        vertices = {
            // Front face
            {{-size, -size, -size}, {}, {0.0f, 1.0f}}, // bottom-left
            {{ size, -size, -size}, {}, {1.0f, 1.0f}}, // bottom-right
            {{-size,  size, -size}, {}, {0.0f, 0.0f}}, // top-left
            {{ size,  size, -size}, {}, {1.0f, 0.0f}}, // top-right

            // Back face
            {{-size, -size,  size}, {}, {1.0f, 1.0f}}, // bottom-right (mirrored)
            {{ size, -size,  size}, {}, {0.0f, 1.0f}}, // bottom-left
            {{-size,  size,  size}, {}, {1.0f, 0.0f}}, // top-right
            {{ size,  size,  size}, {}, {0.0f, 0.0f}}, // top-left
        };

        indices = {
            // Front face
            0, 1, 2,  1, 3, 2,
            // Right face
            1, 5, 3,  5, 7, 3,
            // Top face
            2, 3, 6,  3, 7, 6,
            // Back face
            5, 4, 7,  4, 6, 7,
            // Left face
            4, 0, 6,  0, 2, 6,
            // Bottom face
            4, 5, 0,  5, 1, 0
        };

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