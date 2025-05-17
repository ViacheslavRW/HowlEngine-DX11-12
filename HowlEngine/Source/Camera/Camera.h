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

		XMMATRIX& GetViewMatrix();
		inline XMMATRIX& GetProjMatrix() { return mProjMatrix; };
		void SetProjMatrix(UINT width, UINT height);

		inline void SetPosition(XMFLOAT3 _position) { position = _position; };
		inline void SetRotation(float _pitch, float _yaw, float _roll) { pitch = _pitch; yaw = _yaw; roll = _roll; };
		// move
		inline void MoveForward(float deltaTime) { position.z += cameraSpeed * deltaTime; };
		inline void MoveBackward(float deltaTime) { position.z -= cameraSpeed * deltaTime; };
		inline void MoveLeft(float deltaTime) { position.x -= cameraSpeed * deltaTime; };
		inline void MoveRight(float deltaTime) { position.x += cameraSpeed * deltaTime; };
		inline void MoveUp(float deltaTime) { position.y += cameraSpeed * deltaTime; };
		inline void MoveDown(float deltaTime) { position.y -= cameraSpeed * deltaTime; };

	private:
		constexpr static float nearZ = 0.1f;
		constexpr static float farZ = 100.0f;
		float pitch, yaw, roll;
	
		constexpr static float cameraSpeed = 3.9f;

		XMFLOAT3 position;

		const XMVECTOR defaultForward = XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f );
		const XMVECTOR defaultUp = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );

		XMMATRIX mViewMatrix = {};
		XMMATRIX mProjMatrix = {};
	};
}


