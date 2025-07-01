#pragma once
//#include "../../PCH/pch.h"

namespace HEngine
{
	namespace TR
	{
		// --- DX12 ---
		struct Vertex
		{
			DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
			DirectX::XMFLOAT4 color = { 0.1f, 0.1f, 0.8f, 1.0f };
			DirectX::XMFLOAT3 normal = { 0.0f, 0.0f, 0.0f };
		};

		struct MaterialCelShader
		{
			DirectX::XMFLOAT4 diffuseAlbedo = { 0.0f, 0.0f, 0.0f, 1.0f };
		};

		struct PassData
		{
			DirectX::XMMATRIX viewProjMatrix = DirectX::XMMatrixIdentity();
		};

		// --- Vertices ---
		struct Vertex3C
		{
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT4 color;
		};

		struct Vertex3T
		{
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT3 normal;
			DirectX::XMFLOAT2 textureCoord;
		};

		struct Vertex3M
		{
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT3 normal;
		};

		// --- Material ---

		struct Material
		{
			DirectX::XMFLOAT3 albedo = { 0.8f, 0.0f, 0.0f };
		};
	}
}