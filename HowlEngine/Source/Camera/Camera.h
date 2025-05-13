#pragma once
#include <DirectXMath.h>

namespace HEngine
{
	using namespace DirectX;

	class Camera
	{
	public:
		Camera();
		Camera(const Camera&) = delete;
		Camera* operator=(const Camera&) = delete;

		XMMATRIX& GetViewMatrix();
		inline XMMATRIX& GetProjMatrix() { return mProjMatrix; };
		void SetProjMatrix(UINT width, UINT height);

		inline void SetPosition(XMFLOAT3 _position) { position = _position; };
		inline void SetRotation(float _pitch, float _yaw, float _roll) { pitch = _pitch; yaw = _yaw; roll = _roll; };
	private:
		constexpr static float nearZ = 0.1f;
		constexpr static float farZ = 100.0f;
		float pitch, yaw, roll;

		XMFLOAT3 position;

		const XMVECTOR defaultForward = XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f );
		const XMVECTOR defaultUp = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );

		XMMATRIX mViewMatrix = {};
		XMMATRIX mProjMatrix = {};
	};
}


