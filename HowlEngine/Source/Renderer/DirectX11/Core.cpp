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
}