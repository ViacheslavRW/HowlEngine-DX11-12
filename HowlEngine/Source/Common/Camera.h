#pragma once
#include "wrl.h"
#include "d3d11.h"
#include "Windows.h"
#include "../Config/EngineDLLConfig.h"
#include <DirectXMath.h>

namespace HEngine
{
	using namespace DirectX;
	using namespace Microsoft::WRL;

	struct CameraBuffer
	{
		XMFLOAT3 cameraPosition;
		float pad;
	};

	class HE_API Camera
	{
	public:
		Camera() = default;
		Camera(const Camera&) = delete;
		Camera* operator=(const Camera&) = delete;

		void Initialize();

		void UpdateCameraVectors();
		XMMATRIX& GetViewMatrix();
		void SetProjMatrix(UINT width, UINT height);
	public:
		void CreateCameraBuffer(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pDeviceContext);
		void UpdateCameraBuffer();
		void Release();
	public:
		inline XMMATRIX& GetProjMatrix() { return mProjMatrix; };
		inline XMFLOAT3 GetPosition() const { return position; };

		inline void SetPosition(XMFLOAT3 _position) { position = _position; dirtyView = true; };
		inline void SetRotation(float _pitch, float _yaw, float _roll) { pitch = _pitch; yaw = _yaw; roll = _roll; dirtyRotation = true; dirtyView = true; };

		inline void SetRotationEnabled(bool isEnagle) { isRotationEnabled = isEnagle; };
		inline bool GetRotationEnabled() const { return isRotationEnabled; };
		// move
		void MoveForward(const float& deltaTime);
		void MoveBackward(const float& deltaTime);
		void MoveLeft(const float& deltaTime);
		void MoveRight(const float& deltaTime);
		void MoveUp(const float& deltaTime);
		void MoveDown(const float& deltaTime);
		// rotate
		void RotateByRawMouse(int dx, int dy);
		void RotateLeft(const float& deltaTime);
		void RotateRight(const float& deltaTime);
		void RotateDown(const float& deltaTime);
		void RotateUp(const float& deltaTime);
	private:
		constexpr static float nearZ = 0.1f;
		constexpr static float farZ = 1000.0f;
		float pitch, yaw, roll;
	
		bool dirtyRotation = true;
		bool dirtyView = true;

		bool isRotationEnabled = true;

		constexpr static float cameraSpeed = 2.5f;
		constexpr static float cameraRotSpeed = 2.2f;
		constexpr static float cameraSensitivity = 0.004f;

		ComPtr<ID3D11Device> mDevice;
		ComPtr<ID3D11DeviceContext> mDeviceContext;
		ComPtr<ID3D11Buffer> mCameraBuffer = nullptr;

		XMFLOAT3 position;

		XMFLOAT3 forward = { 0.0f, 0.0f, 1.0f };
		XMFLOAT3 right = { 1.0f, 0.0f, 0.0f };
		XMFLOAT3 up = { 0.0f, 1.0f, 0.0f };
		XMVECTOR worldUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		XMMATRIX mViewMatrix = {};
		XMMATRIX mProjMatrix = {};

		template <typename T>
		inline const T& Clamp(const T& v, const T& lo, const T& hi)
		{
			return (v < lo) ? lo : (hi < v) ? hi : v;
		}
	};
}


