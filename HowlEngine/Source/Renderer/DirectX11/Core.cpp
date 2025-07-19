#include "pch.h"
#include "Core.h"

namespace HEngine
{
	 void Core::InitializeDeviceAndSwapChain(HWND hwnd, const UINT width, const UINT height, ComPtr<ID3D11Device>& pDevice, ComPtr<IDXGISwapChain>& pSwapChain, ComPtr<ID3D11DeviceContext>& pDeviceContext)
	 {
        DXGI_MODE_DESC mDesc = {};
        mDesc.Width = width;
        mDesc.Height = height;
        mDesc.RefreshRate = { 0, 0 };
        mDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        mDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        mDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

        DXGI_SWAP_CHAIN_DESC scDesc = {};
        scDesc.BufferDesc = mDesc;
        scDesc.SampleDesc = { 1, 0 };
        scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        scDesc.BufferCount = MAX_SWAPCHAIN_BUFFERS;
        scDesc.OutputWindow = hwnd;
        scDesc.Windowed = TRUE;
        scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        // selecting the best adapter
        ComPtr<IDXGIFactory6> factory;
        HRESULT coreHRes = CreateDXGIFactory(IID_PPV_ARGS(&factory));
        if (FAILED(coreHRes)) std::cout << "FAILED_TO_CREATE_FACTORY" << std::endl;

        ComPtr<IDXGIAdapter> adapter;

        coreHRes = factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter));
        if (coreHRes == DXGI_ERROR_NOT_FOUND) std::cout << "ADAPTER_NOT_FOUND" << std::endl;

        DXGI_ADAPTER_DESC desc;
        adapter->GetDesc(&desc);
        std::wcout << L"Selected GPU: " << desc.Description << std::endl;

        UINT deviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
        deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
        coreHRes = D3D11CreateDeviceAndSwapChain(
            adapter.Get(),
            D3D_DRIVER_TYPE_UNKNOWN,
            nullptr,
            deviceFlags,
            featureLevels,
            ARRAYSIZE(featureLevels),
            D3D11_SDK_VERSION,
            &scDesc,
            pSwapChain.GetAddressOf(),
            pDevice.GetAddressOf(),
            nullptr,
            pDeviceContext.GetAddressOf()
        );
        if (FAILED(coreHRes)) std::cout << "FAILED_TO_CREATE_DEVICE_AND_SWAPCHAIN" << std::endl;
	 }

     void Core::InitializeRenderTargetView(ComPtr<ID3D11RenderTargetView>& pRTV, ComPtr<ID3D11Device>& pDevice, ComPtr<IDXGISwapChain>& pSwapChain)
     {
         ComPtr<ID3D11Texture2D> backBuffer = nullptr;
         HRESULT coreHRes = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer);
         if (FAILED(coreHRes)) std::cout << "FAILED_TO_CREATE_BACK_BUFFER" << std::endl;

         coreHRes = pDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, pRTV.GetAddressOf());
         if (FAILED(coreHRes)) std::cout << "FAILED_TO_CREATE_RTV" << std::endl;
     }

     void Core::SetViewPort(const UINT width, const UINT height, ComPtr<ID3D11DeviceContext>& pDeviceContext, D3D11_VIEWPORT& pViewPort)
     {
         pViewPort.TopLeftX = 0.0f;
         pViewPort.TopLeftY = 0.0f;
         pViewPort.Width = static_cast<float>(width);
         pViewPort.Height = static_cast<float>(height);
         pViewPort.MinDepth = 0.0f;
         pViewPort.MaxDepth = 1.0f;
         pDeviceContext->RSSetViewports(1, &pViewPort);
     }

     void Core::InitializeDepthStencilView(ComPtr<ID3D11DepthStencilState>& pDepthStencilState, ComPtr<ID3D11Texture2D>& pDepthStencilTexture, ComPtr<ID3D11DepthStencilView>& pDepthStencilView, ComPtr<ID3D11Device> pDevice, const UINT width, const UINT height)
     {
         // depth buffer
         D3D11_DEPTH_STENCIL_DESC dsDesc = {};
         dsDesc.DepthEnable = true;
         dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
         dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
         dsDesc.StencilEnable = false;

         HRESULT coreHRes = pDevice->CreateDepthStencilState(&dsDesc, pDepthStencilState.GetAddressOf());
         if (FAILED(coreHRes)) std::cout << "FAILED_TO_DEPTH_BUFFER" << std::endl;

         // depth stencil texture
         D3D11_TEXTURE2D_DESC tDesc = {};
         tDesc.Width = width;
         tDesc.Height = height;
         tDesc.MipLevels = 1;
         tDesc.ArraySize = 1;
         tDesc.Format = DXGI_FORMAT_D32_FLOAT;
         tDesc.SampleDesc.Count = 1;
         tDesc.SampleDesc.Quality = 0;
         tDesc.Usage = D3D11_USAGE_DEFAULT;
         tDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

         coreHRes = pDevice->CreateTexture2D(&tDesc, nullptr, pDepthStencilTexture.GetAddressOf());
         if (FAILED(coreHRes)) std::cout << "FAILED_TO_DEPTH_STENCIL_TEXTURE" << std::endl;

         // depth stencil view
         D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
         dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
         dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
         dsvDesc.Texture2D.MipSlice = 0;

         coreHRes = pDevice->CreateDepthStencilView(pDepthStencilTexture.Get(), &dsvDesc, pDepthStencilView.GetAddressOf());
         if (FAILED(coreHRes)) std::cout << "FAILED_TO_DEPTH_STENCIL_VIEW" << std::endl;
     }

     void Core::InitializeDepthStencilViewTransparent(ComPtr<ID3D11DepthStencilState>& pDepthStencilState, ComPtr<ID3D11Device> pDevice)
     {
         D3D11_DEPTH_STENCIL_DESC dsDesc = {};
         dsDesc.DepthEnable = true;
         dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
         dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
         dsDesc.StencilEnable = false;

         HRESULT coreHRes = pDevice->CreateDepthStencilState(&dsDesc, pDepthStencilState.GetAddressOf());
         if (FAILED(coreHRes)) std::cout << "FAILED_TO_CREATE_TRANSPARENT_DEPTH_BUFFER" << std::endl;
     }

     void Core::InitializeRasterizer(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pDeviceContext, ComPtr<ID3D11RasterizerState>& pRasterizer)
     {
         D3D11_RASTERIZER_DESC rasterDesc;
         ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));

         rasterDesc.FillMode = D3D11_FILL_SOLID; // D3D11_FILL_WIREFRAME may be used
         rasterDesc.CullMode = D3D11_CULL_NONE; // try cull back if current state is incorrect
         rasterDesc.FrontCounterClockwise = false;
         rasterDesc.DepthBias = 0;
         rasterDesc.SlopeScaledDepthBias = 0.0f;
         rasterDesc.DepthBiasClamp = 0.0f;
         rasterDesc.ScissorEnable = false;
         rasterDesc.MultisampleEnable = false;
         rasterDesc.AntialiasedLineEnable = false;
         HRESULT coreHRes = pDevice->CreateRasterizerState(&rasterDesc, &pRasterizer);
         if (FAILED(coreHRes)) std::cout << "FAILED_TO_CREATE_RASTERIZER" << std::endl;
         pDeviceContext->RSSetState(pRasterizer.Get());
     }

     void Core::InitializeInputLayout(InputLayoutType type, ComPtr<ID3DBlob>& pBlob, ComPtr<ID3D11InputLayout>& pInputLayout, ComPtr<ID3D11Device>& pDevice)
     {
         switch (type)
         {
         case InputLayoutType::Universal:
             {
                 D3D11_INPUT_ELEMENT_DESC layout[] =
                 {
                     { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
                           D3D11_INPUT_PER_VERTEX_DATA, 0 },
                     { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
                             D3D11_INPUT_PER_VERTEX_DATA, 0 },
                     { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24,
                     D3D11_INPUT_PER_VERTEX_DATA, 0 },
                 };
                 HRESULT coreHRes = pDevice->CreateInputLayout(layout, (UINT)std::size(layout), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pInputLayout);
                 if (FAILED(coreHRes)) std::cout << "FAILED_TO_CREATE_INPUT_LAYOUT" << std::endl;
             } break;
         case InputLayoutType::Glass:
             {
                 D3D11_INPUT_ELEMENT_DESC layout[] =
                 {
                     { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
                           D3D11_INPUT_PER_VERTEX_DATA, 0 },
                     { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
                             D3D11_INPUT_PER_VERTEX_DATA, 0 },
                     { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24,
                     D3D11_INPUT_PER_VERTEX_DATA, 0 },
                 };
                 HRESULT coreHRes = pDevice->CreateInputLayout(layout, (UINT)std::size(layout), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pInputLayout);
                 if (FAILED(coreHRes)) std::cout << "FAILED_TO_CREATE_INPUT_LAYOUT" << std::endl;
             } break;
         }
     }

     void Core::InitializeBlendState(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11BlendState>& pBlendState)
     {
         D3D11_BLEND_DESC blendDesc = {};
         blendDesc.RenderTarget[0].BlendEnable = true;
         blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
         blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
         blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

         blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
         blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
         blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

         blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

         HRESULT coreHRes = pDevice->CreateBlendState(&blendDesc, pBlendState.GetAddressOf());
         if (FAILED(coreHRes)) std::cout << "FAILED_TO_CREATE_BLEND_STATE" << std::endl;
     }

     void Core::InitializeTextureSampler(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pDeviceContext, ComPtr<ID3D11SamplerState>& pSampleState)
     {
         D3D11_SAMPLER_DESC sampDesc = {};
         sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
         sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
         sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
         sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

         HRESULT coreHRes = pDevice->CreateSamplerState(&sampDesc, pSampleState.GetAddressOf());
         if (FAILED(coreHRes)) std::cout << "FAILED_TO_CREATE_SAMPLER_STATE" << std::endl;
         pDeviceContext->PSSetSamplers(0, 1, pSampleState.GetAddressOf());
     }
}