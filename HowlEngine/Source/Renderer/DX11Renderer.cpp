#include "pch.h"
#include "DX11Renderer.h"
#include "../Config/EngineGlobalSettings.h"
#include "ShaderCompiler.h"
#include "./DirectX11/Core.h"
#include "../Mesh/Binder.h"


namespace HEngine
{
    DX11Renderer::~DX11Renderer()
    {
        Release();
    }

    void DX11Renderer::Initialize(HWND hwnd, const UINT width, const UINT height, Camera* _camera)
    {
        pCamera = _camera;
        Core::InitializeDeviceAndSwapChain(hwnd, width, height, mDevice, mSwapChain, mDeviceContext);
        Core::InitializeRenderTargetView(mRenderTargetView, mDevice, mSwapChain);
        Core::SetViewPort(width, height, mDeviceContext, mViewPort);
        Core::InitializeDepthStencilView(mDepthStencilState, mDepthStencilTexture, mDepthStencilView, mDevice, width, height);
        Core::InitializeRasterizer(mDevice, mDeviceContext, mRasterizer);

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

        // set compiled shaders
        mDeviceContext->VSSetShader(mVertexShader.Get(), nullptr, 0);
        mDeviceContext->PSSetShader(mPixelShader.Get(), nullptr, 0);

        // create input layout
        D3D11_INPUT_ELEMENT_DESC layout[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
                  D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
                    D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24,
            D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        HRESULT ciRes = mDevice->CreateInputLayout(layout, (UINT)std::size(layout), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &mInputLayout);
        if (FAILED(ciRes)) std::cout << "FAILED_TO_CREATE_INPUT_LAYOUT" << std::endl;

        // create light buffers
        mLightHelper.CreateDirectionalLightBuffer(mDevice, mDirectionalLightBuffer);
        
        // load textures
        mTextureManager.LoadTexture("crumpled_paper", L"Assets/Textures/crumpled_paper.jpg", mDevice.Get(), TextureFormat::JPG);
        mTextureManager.LoadTexture("brick", L"Assets/Textures/brick.jpg", mDevice.Get(), TextureFormat::JPG);
        mTextureManager.LoadTexture("metal", L"Assets/Textures/metal.jpg", mDevice.Get(), TextureFormat::JPG);
        mTextureManager.LoadTexture("desert", L"Assets/Textures/desert.jpg", mDevice.Get(), TextureFormat::JPG);
        mTextureManager.LoadTexture("no_texture", L"Assets/Textures/no_texture.png", mDevice.Get(), TextureFormat::PNG);

        // texture sampler
        D3D11_SAMPLER_DESC sampDesc = {};
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP ;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

        mDevice->CreateSamplerState(&sampDesc, mSamplerState.GetAddressOf());
        mDeviceContext->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());

        // load external mesh
        mesh1 = std::make_unique<Mesh>(*mDevice.Get(), *mDeviceContext.Get(), *mInputLayout.Get(), mTextureManager);
        mesh1->Initialize(pCamera->GetViewMatrix(), pCamera->GetProjMatrix());
        mMeshLoader.LoadMesh(mesh1.get(), "Models/Monitor.obj", L"", &mTextureManager, mDevice.Get());
        mesh1->CreateBuffers();
        mesh1->GetScale() = { 0.3f, 0.3f, 0.3f };
        mesh1->GetPosition().x = 1.8f;
        mesh1->GetPosition().y = -1.0f;

        mesh2 = std::make_unique<Mesh>(*mDevice.Get(), *mDeviceContext.Get(), *mInputLayout.Get(), mTextureManager);
        mesh2->Initialize(pCamera->GetViewMatrix(), pCamera->GetProjMatrix());
        mMeshLoader.LoadMesh(mesh2.get(), "Models/Eve_16_2.obj", L"/Eve_16/", &mTextureManager, mDevice.Get());
        mesh2->CreateBuffers();
        mesh2->GetScale() = { 0.2f, 0.2f, 0.2f };
        mesh2->GetPosition().y = -1.5f;
        mesh2->GetRotation().y = 1.5f;
        
        // create mesh
        cube1 = std::make_unique<CubeMeshT>(*mDevice.Get(), *mDeviceContext.Get(), *mInputLayout.Get(), mTextureManager);
        cube1->Initialize(0.5f, pCamera->GetViewMatrix(), pCamera->GetProjMatrix(), "metal");
        cube1->GetPosition().x = -1.5f;

        plane1 = std::make_unique<PlaneMeshT>(*mDevice.Get(), *mDeviceContext.Get(), *mInputLayout.Get(), mTextureManager);
        plane1->Initialize(6.0f, 6.0f, 1, 1, pCamera->GetViewMatrix(), pCamera->GetProjMatrix(), "desert");
        plane1->GetPosition().z = 1.0f;
        plane1->GetPosition().y = -1.5f;
    }

    void DX11Renderer::Update(const float deltaTime)
    {
#if defined(DEBUG) || defined(_DEBUG)
        mDebugInfoManager.Set();
#endif

        mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        mDeviceContext->ClearRenderTargetView(mRenderTargetView.Get(), CLEAR_BUFFER_COLOR);
        mDeviceContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

        mDeviceContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), mDepthStencilView.Get());
        mDeviceContext->OMSetDepthStencilState(mDepthStencilState.Get(), 1);

        mLightHelper.SetDirectionalLightColor().x += 0.2f * deltaTime;
        if (mLightHelper.SetDirectionalLightColor().x >= 1.0f) mLightHelper.SetDirectionalLightColor().x = -1.0f;
        mLightHelper.SetDirectionalLightColor().y -= 0.25f * deltaTime;
        if (mLightHelper.SetDirectionalLightColor().y <= -1.0f) mLightHelper.SetDirectionalLightColor().y = 1.0f;
        mLightHelper.SetDirectionalLightColor().z += 0.35f * deltaTime;
        if (mLightHelper.SetDirectionalLightColor().z >= 1.0f) mLightHelper.SetDirectionalLightColor().z = -1.0f;
        mLightHelper.UpdateDirectionalLightBuffer(*mDeviceContext.Get(), mDirectionalLightBuffer, mLightHelper.GetDirectionalLightParams());

        // drawing world objects
        mesh1->Draw(pCamera->GetViewMatrix());
        mesh2->Draw(pCamera->GetViewMatrix());

        cube1->Bind(pCamera->GetViewMatrix());
        cube1->Draw();

        plane1->Bind(pCamera->GetViewMatrix());
        plane1->Draw();

        // present frames
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

        mTextureManager.Clear();
    }
}