#include "pch.h"
#include "DX11Renderer.h"
#include "../Config/EngineGlobalSettings.h"
#include "ShaderCompiler.h"
#include "./DirectX11/Core.h"

namespace HEngine
{
    DX11Renderer::~DX11Renderer()
    {
        Release();
    }

    void DX11Renderer::Initialize(HWND hwnd, const UINT width, const UINT height)
    {
        Core::InitializeDeviceAndSwapChain(hwnd, width, height, mDevice, mSwapChain, mDeviceContext);
        Core::InitializeRenderTargetView(mRenderTargetView, mDevice, mSwapChain);

        // viewport
        mViewPort.TopLeftX = 0.0f;
        mViewPort.TopLeftY = 0.0f;
        mViewPort.Width = static_cast<float>(width);
        mViewPort.Height = static_cast<float>(height);
        mViewPort.MinDepth = 0.0f;
        mViewPort.MaxDepth = 1.0f;
        mDeviceContext->RSSetViewports(1, &mViewPort);

        // -- test --
        TR::Vertex3D vertices[] = {
            // front face
            {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}},
            {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}},
            {{-0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}},
            {{0.5f, 0.5f, -0.5f}, {0.5f, 0.0f, 0.0f, 1.0f}},
            // back face
            {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}},
            {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}},
            {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}},
            {{0.5f, 0.5f, 0.5f}, {0.5f, 0.0f, 0.0f, 1.0f}},
        };

        unsigned short indices[] = {
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
        InitCube(vertices, indices);
    }

    void DX11Renderer::Update()
    {
#if defined(DEBUG) || defined(_DEBUG)
        mDebugInfoManager.Set();
#endif

        mDeviceContext->ClearRenderTargetView(mRenderTargetView.Get(), CLEAR_BUFFER_COLOR);

        mDeviceContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), nullptr);

        DrawCube(size, mAngle);

        HRESULT scRes = mSwapChain->Present( 1, 0 );
        if (FAILED(scRes))
        {
            std::cout << "SWAP_CHAIN_ERROR" << std::endl;
            if (scRes == DXGI_ERROR_DEVICE_REMOVED)
            {
                HRESULT reason = mDevice->GetDeviceRemovedReason();
                std::cout << "ERROR_DEVICE_WAS_REMOVED " << std::hex << reason << std::endl;
            }
        }

#if defined(DEBUG) || defined(_DEBUG)
        for (const auto& message : mDebugInfoManager.GetDebugMessages())
        {
            std::cout << "DXGI_DEBUG: " << message << std::endl;
        }
#endif
    }

    void DX11Renderer::Release()
    {
        if (mDeviceContext) mDeviceContext = nullptr;
        if (mSwapChain) mSwapChain = nullptr;
        if (mDevice) mDevice = nullptr;
        if (mRenderTargetView) mRenderTargetView = nullptr;
    }

    // ---- TEST FUNCTIONS ----
    void DX11Renderer::InitCube(TR::Vertex3D vertices[], unsigned short indices[])
    {
        // creating vertex buffer
        D3D11_BUFFER_DESC bDesc = {};
        bDesc.ByteWidth = sizeof(TR::Vertex3D) * 8;
        bDesc.Usage = D3D11_USAGE_DEFAULT;
        bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bDesc.CPUAccessFlags = 0;
        bDesc.MiscFlags = 0;
        bDesc.StructureByteStride = sizeof(TR::Vertex3D);

        D3D11_SUBRESOURCE_DATA srData = {};
        srData.pSysMem = vertices;
        srData.SysMemPitch = 0;
        srData.SysMemSlicePitch = 0;

        HRESULT res = mDevice->CreateBuffer(&bDesc, &srData, &mVertexBuffer);
        if (FAILED(res)) std::cout << "FAILED_TO_CREATE_VERTEX_BUFFER" << std::endl;

        // create index buffer
        D3D11_BUFFER_DESC indBuffDesc = {};
        indBuffDesc.ByteWidth = sizeof(unsigned short) * 36;
        indBuffDesc.Usage = D3D11_USAGE_DEFAULT;
        indBuffDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        indBuffDesc.CPUAccessFlags = 0;
        indBuffDesc.MiscFlags = 0;
        indBuffDesc.StructureByteStride = sizeof(unsigned short);

        D3D11_SUBRESOURCE_DATA iData = {};
        iData.pSysMem = indices;

        HRESULT res2 = mDevice->CreateBuffer(&indBuffDesc, &iData, &mIndexBuffer);
        if (FAILED(res2)) std::cout << "FAILED_TO_CREATE_INDEX_BUFFER" << std::endl;

        // create constant buffer
        D3D11_BUFFER_DESC cbDesc = {};
        cbDesc.ByteWidth = sizeof(XMMATRIX);
        cbDesc.Usage = D3D11_USAGE_DEFAULT;
        cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbDesc.CPUAccessFlags = 0;
        cbDesc.MiscFlags = 0;
        cbDesc.StructureByteStride = sizeof(XMMATRIX);

        HRESULT res3 = mDevice->CreateBuffer(&cbDesc, nullptr, &mConstantBuffer);
        if (FAILED(res3)) std::cout << "FAILED_TO_CREATE_CONSTANT_BUFFER" << std::endl;

        // compile shaders to bytecode
        Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
        Microsoft::WRL::ComPtr<ID3DBlob> psBlob;
        ShaderCompiler compiler;
        compiler.Compile(L"shaders/VertexShader.hlsl", ShaderCompiler::ShaderType::VERTEX, GraphicsAPI::DirectX11, &vsBlob);
        compiler.Compile(L"shaders/PixelShader.hlsl", ShaderCompiler::ShaderType::PIXEL, GraphicsAPI::DirectX11, &psBlob);

        // create and bind vertex shader and pixel shader
        HRESULT vsRes = mDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &mVertexShader);
        if (FAILED(vsRes)) std::cout << "FAILED_TO_CREATE_VERTEX_SHADER" << std::endl;
        HRESULT psRes = mDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &mPixelShader);
        if (FAILED(psRes)) std::cout << "FAILED_TO_CREATE_PIXEL_SHADER" << std::endl;

        // create input layout
        D3D11_INPUT_ELEMENT_DESC layout[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
                  D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
                    D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        HRESULT ciRes = mDevice->CreateInputLayout(layout, (UINT)std::size(layout), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &mInputLayout);
        if (FAILED(ciRes)) std::cout << "FAILED_TO_CREATE_INPUT_LAYOUT" << std::endl;
    }

    void DX11Renderer::DrawCube(UINT indices, float angle)
    {
        XMMATRIX modelMatrix = XMMatrixRotationZ(angle) * XMMatrixRotationX(angle);
        XMMATRIX viewMatrix = XMMatrixTranslation(0.0f, 0.0f, 2.3f); 
        XMMATRIX projectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), 1.0f / 0.75f, 0.1f, 10.0f);

        XMMATRIX mvp = modelMatrix * viewMatrix * projectionMatrix;
        mViewProjectionMatrix = XMMatrixTranspose(mvp);

        mDeviceContext->UpdateSubresource(mConstantBuffer.Get(), 0, nullptr, &mViewProjectionMatrix, 0, 0);
        mDeviceContext->VSSetConstantBuffers(0, 1, mConstantBuffer.GetAddressOf());
        // set input layout and vertex buffer
        UINT stride = sizeof(TR::Vertex3D);
        UINT offset = 0;
        mDeviceContext->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &stride, &offset);
        mDeviceContext->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
        mDeviceContext->IASetInputLayout(mInputLayout.Get());
        // specifing primitive type
        mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        // set shaders
        mDeviceContext->VSSetShader(mVertexShader.Get(), nullptr, 0);
        mDeviceContext->PSSetShader(mPixelShader.Get(), nullptr, 0);
        // drawing 
        mDeviceContext->DrawIndexed(indices, 0, 0);

        mAngle += 0.0001f;
    }
}