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
        HRESULT fHr = CreateDXGIFactory(IID_PPV_ARGS(&factory));
        if (FAILED(fHr)) std::cout << "FAILED_TO_CREATE_FACTORY" << std::endl;

        ComPtr<IDXGIAdapter> adapter;

        HRESULT eRes = factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter));
        if (eRes == DXGI_ERROR_NOT_FOUND) std::cout << "ADAPTER_NOT_FOUND" << std::endl;

        DXGI_ADAPTER_DESC desc;
        adapter->GetDesc(&desc);
        std::wcout << L"Selected GPU: " << desc.Description << std::endl;

        UINT deviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
        deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
        HRESULT res = D3D11CreateDeviceAndSwapChain(
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
        if (FAILED(res)) std::cout << "FAILED_TO_CREATE_DEVICE_AND_SWAPCHAIN" << std::endl;
	 }

     void Core::InitializeRenderTargetView(ComPtr<ID3D11RenderTargetView>& pRTV, ComPtr<ID3D11Device>& pDevice, ComPtr<IDXGISwapChain>& pSwapChain)
     {
         ComPtr<ID3D11Texture2D> backBuffer = nullptr;
         HRESULT bRes = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer);
         if (FAILED(bRes)) std::cout << "FAILED_TO_CREATE_BACK_BUFFER" << std::endl;

         HRESULT rtvRes = pDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, pRTV.GetAddressOf());
         if (FAILED(rtvRes)) std::cout << "FAILED_TO_CREATE_RTV" << std::endl;
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

         HRESULT hr1 = pDevice->CreateDepthStencilState(&dsDesc, pDepthStencilState.GetAddressOf());
         if (FAILED(hr1)) std::cout << "FAILED_TO_DEPTH_BUFFER" << std::endl;

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

         HRESULT hr2 = pDevice->CreateTexture2D(&tDesc, nullptr, pDepthStencilTexture.GetAddressOf());
         if (FAILED(hr2)) std::cout << "FAILED_TO_DEPTH_STENCIL_TEXTURE" << std::endl;

         // depth stencil view
         D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
         dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
         dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
         dsvDesc.Texture2D.MipSlice = 0;

         HRESULT hr3 = pDevice->CreateDepthStencilView(pDepthStencilTexture.Get(), &dsvDesc, pDepthStencilView.GetAddressOf());
         if (FAILED(hr3)) std::cout << "FAILED_TO_DEPTH_STENCIL_VIEW" << std::endl;
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
         HRESULT hr = pDevice->CreateRasterizerState(&rasterDesc, &pRasterizer);
         if (FAILED(hr)) std::cout << "FAILED_TO_CREATE_RASTERIZER" << std::endl;
         pDeviceContext->RSSetState(pRasterizer.Get());
     }
}