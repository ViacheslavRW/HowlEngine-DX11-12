#pragma once
//#include "../../PCH/pch.h"

namespace HEngine
{
	namespace TR
	{
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

		struct Vertex2D
		{
			DirectX::XMFLOAT2 position = {0.0f, 0.0f};
			DirectX::XMFLOAT4 color = { 0.1f, 0.1f, 0.1f, 1.0f };
		};

		struct Vertex3D
		{
			DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
			DirectX::XMFLOAT4 color = { 0.1f, 0.1f, 0.8f, 1.0f };
		};

		struct TransformBuffer
		{
			DirectX::XMMATRIX worldViewProjmatrix;
		};
	}
}