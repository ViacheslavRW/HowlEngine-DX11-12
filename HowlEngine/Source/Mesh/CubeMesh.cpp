#include "pch.h"
#include "CubeMesh.h"

namespace HEngine
{
    void CubeMeshT::Initialize(float size, XMMATRIX& _viewMatrix, XMMATRIX& _projectionMatrix, std::string textureName)
    {
        vertices = {
            // Front face (-Z)
            {{-size, -size, -size}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}}, 
            {{ size, -size, -size}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}}, 
            {{-size,  size, -size}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}}, 
            {{ size,  size, -size}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}}, 

            // Back face (+Z)
            {{ size, -size,  size}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},  
            {{-size, -size,  size}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},  
            {{ size,  size,  size}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},  
            {{-size,  size,  size}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},  

            // Right face (+X)
            {{ size, -size, -size}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
            {{ size, -size,  size}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
            {{ size,  size, -size}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
            {{ size,  size,  size}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},

            // Left face (-X)
            {{-size, -size,  size}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
            {{-size, -size, -size}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
            {{-size,  size,  size}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
            {{-size,  size, -size}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},

            // Top face (+Y)
            {{-size,  size, -size}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
            {{ size,  size, -size}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
            {{-size,  size,  size}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
            {{ size,  size,  size}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},

            // Bottom face (-Y)
            {{-size, -size,  size}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
            {{ size, -size,  size}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
            {{-size, -size, -size}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
            {{ size, -size, -size}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
        };

        indices = {
            0, 1, 2,  1, 3, 2,    // Front
            4, 5, 6,  5, 7, 6,    // Back
            8, 9,10,  9,11,10,    // Right
           12,13,14, 13,15,14,    // Left
           16,17,18, 17,19,18,    // Top
           20,21,22, 21,23,22     // Bottom
        };

        mViewMatrix = _viewMatrix;
        mProjMatrix = _projectionMatrix;
        texture = textureName;

        CreateVertexBuffer(vertices, mVertexBuffer);
        CreateIndexBuffer(indices, mIndexBuffer);
        CreateConstantBuffer(mConstantBuffer);
    }

    void CubeMeshT::Draw(XMMATRIX& viewMatrix)
    {
        BindRes(mVertexBuffer, mIndexBuffer, mConstantBuffer, GetModelMartix(), viewMatrix, mProjMatrix,
            sizeof(TR::Vertex3T), texture);
        Binder::DrawMesh(indices.size());
    }

    XMMATRIX CubeMeshT::GetModelMartix()
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
}