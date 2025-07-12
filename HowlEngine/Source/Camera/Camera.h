#pragma once
#include "../Config/EngineDLLConfig.h"
#include <DirectXMath.h>
#include "Windows.h"

namespace HEngine
{
	using namespace DirectX;

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

		inline XMMATRIX& GetProjMatrix() { return mProjMatrix; };

		inline void SetPosition(XMFLOAT3 _position) { position = _position; dirtyView = true; };
		inline void SetRotation(float _pitch, float _yaw, float _roll) { pitch = _pitch; yaw = _yaw; roll = _roll; dirtyRotation = true; dirtyView = true; };

		inline void SetRotationEnabled(bool isEnagle) { isRotationEnabled = isEnagle; };
		inline bool GetRotationEnabled() const { return isRotationEnabled; };
		// move
		void MoveForward(float deltaTime);
		void MoveBackward(float deltaTime);
		void MoveLeft(float deltaTime);
		void MoveRight(float deltaTime);
		void MoveUp(float deltaTime);
		void MoveDown(float deltaTime);
		// rotate
		void RotateByRawMouse(int dx, int dy);
		void RotateLeft(float deltaTime);
		void RotateRight(float deltaTime);
		void RotateDown(float deltaTime);
		void RotateUp(float deltaTime);
	private:
		constexpr static float nearZ = 0.1f;
		constexpr static float farZ = 100.0f;
		float pitch, yaw, roll;
	
		bool dirtyRotation = true;
		bool dirtyView = true;

		bool isRotationEnabled = true;

		constexpr static float cameraSpeed = 7.5f;
		constexpr static float cameraRotSpeed = 2.2f;
		constexpr static float cameraSensitivity = 0.004f;

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


