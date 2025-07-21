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
        Core::InitializeInputLayout(InputLayoutType::PBR, mShaderCompiler.vsBlobPBR, mInputLayoutPBR, mDevice);
        // blend state
        Core::InitializeBlendState(mDevice, mBlendState);

        // create light buffers
        mLightHelper.CreateDirectionalLightBuffer(mDevice);
        mLightHelper.CreatePointLightBuffer(mDevice);

        mLightHelper.SetPointLightPosition(0, { -1.8f, -0.2f, 0.0f });
        
        mLightHelper.SetPointLightPosition(1, { 1.0f, 0.5f, -0.3f });
        mLightHelper.SetPointLightColor(1, { 0.0f, 0.0f, 1.0f, 1.0f });

        // load textures
        mTextureManager.LoadTexture("no_texture", L"Assets/Textures/no_texture.png", mDevice.Get(), TextureFormat::PNG);

        // texture sampler
        Core::InitializeTextureSampler(mDevice, mDeviceContext, mSamplerState);

        mMeshManager.Initialize(mDevice.Get(), mDeviceContext.Get(), mInputLayoutPBR.Get(), 
            &mTextureManager, &mMeshLoader, pCamera->GetViewMatrix(), pCamera->GetProjMatrix());
        mMeshLoader.Initialize(&mTextureManager, mDevice.Get(), &mMaterialManager);
        mMeshManager.InitializeAllMeshes();
        mMeshManager.CreateAllBuffers();

        // link meshes
        mLightHelper.SetPointLightSource(0, *mMeshManager.meshes[2]);
        mMeshManager.meshes[2]->childMesh = mMeshManager.meshes[3].get();

        // load additional maps
        // normal maps
        mMeshManager.meshes[0]->subMeshes[1].material.normalSRV = mTextureManager.
            LoadAndGetSRV("CH_P_EVE_Nikke_06_UV1_N.tga", L"Assets/Textures/Nikke_06/CH_P_EVE_Nikke_06_UV1_N.tga", mDevice.Get(), TextureFormat::TGA);
        mMeshManager.meshes[0]->subMeshes[2].material.normalSRV = mTextureManager.
            LoadAndGetSRV("CH_P_EVE_Nikke_06_UV2_N.tga", L"Assets/Textures/Nikke_06/CH_P_EVE_Nikke_06_UV2_N.tga", mDevice.Get(), TextureFormat::TGA);
        // ORM maps
        mMeshManager.meshes[0]->subMeshes[0].material.ormSRV = mTextureManager.
            LoadAndGetSRV("CH_P_EVE_Nikke_06_Decal_ORM.tga", L"Assets/Textures/Nikke_06/CH_P_EVE_Nikke_06_Decal_ORM.tga", mDevice.Get(), TextureFormat::TGA);
        mMeshManager.meshes[0]->subMeshes[1].material.ormSRV = mTextureManager.
            LoadAndGetSRV("CH_P_EVE_Nikke_06_UV1_ORM.tga", L"Assets/Textures/Nikke_06/CH_P_EVE_Nikke_06_UV1_ORM.tga", mDevice.Get(), TextureFormat::TGA);
        mMeshManager.meshes[0]->subMeshes[2].material.ormSRV = mTextureManager.
            LoadAndGetSRV("CH_P_EVE_Nikke_06_UV2_ORM.tga", L"Assets/Textures/Nikke_06/CH_P_EVE_Nikke_06_UV2_ORM.tga", mDevice.Get(), TextureFormat::TGA);
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
        mShaderCompiler.SetShadersToPipeline(mDeviceContext, ShaderCompiler::ShaderPipeline::PBR);
        mMeshManager.Render(pCamera->GetViewMatrix());

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
        ImGui::SliderFloat("Range", &mLightHelper.GetPointLightParams(0).lightRange, 0.0f, 50.0f);
        ImGui::End();

        ImGui::Begin("Point Light 2");
        ImGui::SliderFloat3("Position", &mLightHelper.GetPointLightParams(1).lightPosition.x, -10.0f, 10.0f);
        ImGui::ColorEdit4("Color", &mLightHelper.GetPointLightParams(1).lightColor.x);
        ImGui::SliderFloat4("Ambient", &mLightHelper.GetPointLightParams(1).lightAmbient.x, 0.0f, 1.0f);
        ImGui::SliderFloat4("Diffuse", &mLightHelper.GetPointLightParams(1).lightDiffuse.x, 0.0f, 1.0f);
        ImGui::SliderFloat3("Attenuation", &mLightHelper.GetPointLightParams(1).lightAttenuation.x, 0.0f, 1.0f);
        ImGui::SliderFloat("Range", &mLightHelper.GetPointLightParams(1).lightRange, 0.0f, 50.0f);
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
        mInputLayoutPBR.Reset();

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

        mMeshManager.Release();
        mMeshLoader.Release();

        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }
}