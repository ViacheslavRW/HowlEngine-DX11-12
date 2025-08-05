#pragma once
#include <DirectXTex.h>
#include "../Renderer/RenderTypes/RenderDataTypes.h"

namespace HEngine
{
	using namespace Microsoft::WRL;
	using namespace DirectX;

	struct SkyboxCB
	{
		XMMATRIX viewProjMatrix;
	};

	struct Skybox
	{
	public:
		void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ID3D11InputLayout* pInputLayout,
			XMMATRIX& _viewMatrix, XMMATRIX& _projectionMatrix);
		void LoadSkyboxTexture();
		void CreateSkyboxBuffers();
		void Bind(const XMMATRIX& viewMatrix);
		void Draw();
		void RenderSkybox(const XMMATRIX& viewMatrix);

	private:
		static constexpr USHORT faces = 6;
		static constexpr float cubeSize = 1000.0f;

		const std::wstring skyboxPath[faces] =
		{
			L"Assets/Skybox/BlueCloud/bluecloud_lf.jpg",
			L"Assets/Skybox/BlueCloud/bluecloud_rt.jpg",
			L"Assets/Skybox/BlueCloud/bluecloud_up.jpg",
			L"Assets/Skybox/BlueCloud/bluecloud_dn.jpg",
			L"Assets/Skybox/BlueCloud/bluecloud_ft.jpg",
			L"Assets/Skybox/BlueCloud/bluecloud_bk.jpg"
		};
		
		const std::vector<TR::SkyboxVertex> vertices =
		{
			{XMFLOAT3(-cubeSize, -cubeSize, -cubeSize)}, {XMFLOAT3(cubeSize, -cubeSize, -cubeSize)},
			{XMFLOAT3(cubeSize, cubeSize, -cubeSize)}, {XMFLOAT3(-cubeSize, cubeSize, -cubeSize)},
			{XMFLOAT3(-cubeSize, -cubeSize, cubeSize)}, {XMFLOAT3(cubeSize, -cubeSize, cubeSize)},
			{XMFLOAT3(cubeSize, cubeSize, cubeSize)}, {XMFLOAT3(-cubeSize, cubeSize, cubeSize)}
		};


		const std::vector<UINT> indices =
		{
			0, 1, 2, 0, 2, 3, // Front
			5, 4, 7, 5, 7, 6, // Back
			1, 5, 6, 1, 6, 2, // Right
			4, 0, 3, 4, 3, 7, // Left
			3, 2, 6, 3, 6, 7, // Top
			4, 5, 1, 4, 1, 0  // Bottom
		};

	private:
		ComPtr<ID3D11ShaderResourceView> mSkyboxSRV;
		ComPtr<ID3D11Texture2D> mSkyboxTexture2D;

		ComPtr<ID3D11Buffer> mVertexBuffer = nullptr;
		ComPtr<ID3D11Buffer> mIndexBuffer = nullptr;
		ComPtr<ID3D11Buffer> mConstantBuffer = nullptr;

	private:
		XMMATRIX cashedViewMatrix;
		XMMATRIX cashedProjMatrix;

		ID3D11Device* mDevice;
		ID3D11DeviceContext* mDeviceContext;
		ID3D11InputLayout* mInputLayout;
	};
}


