#include "pch.h"
#include "DX11Renderer.h"
#include "../Config/EngineGlobalSettings.h"
#include "./DirectX11/Core.h"
#include "../Mesh/Binder.h"

#include "../Editor/ImGui/imgui.h"
#include "../Editor/ImGui/imgui_impl_win32.h"
#include "../Editor/ImGui/imgui_impl_dx11.h"

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
        Core::InitializeDepthStencilViewTransparent(mDepthStencilStateTransparent, mDevice);
        Core::InitializeRasterizer(mDevice, mDeviceContext, mRasterizer);

        // init imgui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX11_Init(mDevice.Get(), mDeviceContext.Get());

        // compile shaders
        mShaderCompiler.CompileAll();
        mShaderCompiler.CreateAll(mDevice);

        // create input layout
        Core::InitializeInputLayout(InputLayoutType::Universal, mShaderCompiler.vsBlobUniversal, mInputLayout, mDevice);
        Core::InitializeInputLayout(InputLayoutType::Glass, mShaderCompiler.vsBlobGlass, mInputLayoutGlass, mDevice);
        // blend state
        Core::InitializeBlendState(mDevice, mBlendState);

        // create light buffers
        mLightHelper.CreateDirectionalLightBuffer(mDevice);
        mLightHelper.CreatePointLightBuffer(mDevice);

        mLightHelper.SetPointLightPosition(0, { -1.8f, -0.2f, 0.0f });
        
        mLightHelper.SetPointLightPosition(1, { 1.0f, 0.5f, -0.3f });
        mLightHelper.SetPointLightColor(1, { 0.0f, 0.0f, 1.0f, 1.0f });

        // load textures
        //mTextureManager.LoadTexture("metal", L"Assets/Textures/metal.jpg", mDevice.Get(), TextureFormat::JPG);
        mTextureManager.LoadTexture("desert", L"Assets/Textures/desert.jpg", mDevice.Get(), TextureFormat::JPG);
        //mTextureManager.LoadTexture("white", L"Assets/Textures/white.jpg", mDevice.Get(), TextureFormat::JPG);
        mTextureManager.LoadTexture("no_texture", L"Assets/Textures/no_texture.png", mDevice.Get(), TextureFormat::PNG);

        // texture sampler
        Core::InitializeTextureSampler(mDevice, mDeviceContext, mSamplerState);

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
        mMeshLoader.LoadMesh(mesh2.get(), "Models/Eve_16_2.obj", L"Eve_16/", &mTextureManager, mDevice.Get());
        mesh2->CreateBuffers();
        mesh2->GetScale() = { 0.2f, 0.2f, 0.2f };
        mesh2->GetPosition().y = -1.5f;
        mesh2->GetRotation().y = 1.5f;
        
        mesh3 = std::make_unique<Mesh>(*mDevice.Get(), *mDeviceContext.Get(), *mInputLayoutGlass.Get(), mTextureManager);
        mesh3->Initialize(pCamera->GetViewMatrix(), pCamera->GetProjMatrix());
        mMeshLoader.LoadMesh(mesh3.get(), "Models/Light/LightBulb1Glass.obj", L"", &mTextureManager, mDevice.Get());
        mesh3->CreateBuffers();
        mesh3->GetPosition().x = -1.8f;
        mesh3->GetPosition().y = -0.2f;

        mesh4 = std::make_unique<Mesh>(*mDevice.Get(), *mDeviceContext.Get(), *mInputLayout.Get(), mTextureManager);
        mesh4->Initialize(pCamera->GetViewMatrix(), pCamera->GetProjMatrix());
        mMeshLoader.LoadMesh(mesh4.get(), "Models/Light/LightBulb1Bottom.obj", L"", &mTextureManager, mDevice.Get());
        mesh4->CreateBuffers();
        mesh4->GetPosition().x = -1.8f;
        mesh4->GetPosition().y = -0.2f;

        // link meshes
        mLightHelper.SetPointLightSource(0, *mesh3);
        mesh3->childMesh = mesh4.get();
        // create mesh
        plane1 = std::make_unique<PlaneMeshT>(*mDevice.Get(), *mDeviceContext.Get(), *mInputLayout.Get(), mTextureManager);
        plane1->Initialize(6.0f, 6.0f, 1, 1, pCamera->GetViewMatrix(), pCamera->GetProjMatrix(), "desert");
        plane1->GetPosition().z = 1.0f;
        plane1->GetPosition().y = -1.5f;
    }

    void DX11Renderer::Update(const float& deltaTime)
    {
#if defined(DEBUG) || defined(_DEBUG)
        mDebugInfoManager.Set();
#endif

        mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        mDeviceContext->ClearRenderTargetView(mRenderTargetView.Get(), CLEAR_BUFFER_COLOR);
        mDeviceContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

        mDeviceContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), mDepthStencilView.Get());
        mDeviceContext->OMSetDepthStencilState(mDepthStencilState.Get(), 1);

        mLightHelper.UpdateDirectionalLightBuffer(*mDeviceContext.Get());
        mLightHelper.UpdatePointLightBuffer(*mDeviceContext.Get());
        
        // render world objects
        mShaderCompiler.SetShadersToPipeline(mDeviceContext, ShaderCompiler::ShaderPipeline::UNIVERSAL);
        mesh1->Draw(pCamera->GetViewMatrix());
        mesh2->Draw(pCamera->GetViewMatrix());
        mesh4->Draw(pCamera->GetViewMatrix());
        plane1->Draw(pCamera->GetViewMatrix());

        // render glass objects
        float blendFactor[4] = { 0, 0, 0, 0 };
        mDeviceContext->OMSetBlendState(mBlendState.Get(), blendFactor, 0xffffffff);
        mShaderCompiler.SetShadersToPipeline(mDeviceContext, ShaderCompiler::ShaderPipeline::GLASS);
        mesh3->Draw(pCamera->GetViewMatrix());

        // draw ui
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Point Light 1");
        if (ImGui::SliderFloat3("Position", &mLightHelper.GetPointLightParams(0).lightPosition.x, -10.0f, 10.0f))
        {
            mLightHelper.SetPointLightPosition(0, mLightHelper.GetPointLightParams(0).lightPosition);
        }
        ImGui::ColorEdit4("Color", &mLightHelper.GetPointLightParams(0).lightColor.x);
        ImGui::SliderFloat4("Ambient", &mLightHelper.GetPointLightParams(0).lightAmbient.x, 0.0f, 1.0f);
        ImGui::SliderFloat4("Diffuse", &mLightHelper.GetPointLightParams(0).lightDiffuse.x, 0.0f, 1.0f);
        ImGui::SliderFloat3("Attenuation", &mLightHelper.GetPointLightParams(0).lightAttenuation.x, 0.0f, 1.0f);
        ImGui::SliderFloat("Range", &mLightHelper.GetPointLightParams(0).lightRange, 0.0f, 10.0f);
        ImGui::End();

        ImGui::Begin("Point Light 2");
        ImGui::SliderFloat3("Position", &mLightHelper.GetPointLightParams(1).lightPosition.x, -10.0f, 10.0f);
        ImGui::ColorEdit4("Color", &mLightHelper.GetPointLightParams(1).lightColor.x);
        ImGui::SliderFloat4("Ambient", &mLightHelper.GetPointLightParams(1).lightAmbient.x, 0.0f, 1.0f);
        ImGui::SliderFloat4("Diffuse", &mLightHelper.GetPointLightParams(1).lightDiffuse.x, 0.0f, 1.0f);
        ImGui::SliderFloat3("Attenuation", &mLightHelper.GetPointLightParams(1).lightAttenuation.x, 0.0f, 1.0f);
        ImGui::SliderFloat("Range", &mLightHelper.GetPointLightParams(1).lightRange, 0.0f, 10.0f);
        ImGui::End();

        mLightHelper.mDirtyPointLight = true;
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // present frames
        hRes = mSwapChain->Present( 1, 0 );
        if (FAILED(hRes))
        {
            std::cout << "SWAP_CHAIN_ERROR" << std::endl;
            if (hRes == DXGI_ERROR_DEVICE_REMOVED)
            {
                hRes = mDevice->GetDeviceRemovedReason();
                std::cout << "ERROR_DEVICE_WAS_REMOVED " << std::hex << hRes << std::endl;
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
#if defined(DEBUG) || defined(_DEBUG)
        mDebugInfoManager.Release();
#endif
        mShaderCompiler.Release();
        pCamera = nullptr;
        mSamplerState.Reset();
        mBlendState.Reset();
        mInputLayout.Reset();
        mInputLayoutGlass.Reset();

        mRasterizer.Reset();
        mDepthStencilView.Reset();
        mDepthStencilTexture.Reset();
        mDepthStencilStateTransparent.Reset();
        mDepthStencilState.Reset();

        mRenderTargetView.Reset();
        mSwapChain.Reset();
        mDeviceContext.Reset();
        mDevice.Reset();

        mTextureManager.Clear();
        mLightHelper.Release();

        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }
}