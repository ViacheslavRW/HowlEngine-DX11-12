#include "pch.h"
#include "Camera.h"

namespace HEngine
{
	Camera::Camera()
		: position(0.0f, 0.0f, -4.5f), pitch(0.0f), yaw(0.0f), roll(0.0f) {}

	XMMATRIX& Camera::GetViewMatrix()
	{
		XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
		XMVECTOR forward = XMVector3TransformCoord(defaultForward, rotationMatrix);
		XMVECTOR up = XMVector3TransformCoord(defaultUp, rotationMatrix);

		XMVECTOR camPos = XMLoadFloat3(&position);
		mViewMatrix = XMMatrixLookAtLH(camPos, camPos + forward, up);
		return mViewMatrix;
	}

	void Camera::SetProjMatrix(UINT width, UINT height)
	{
		mProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), static_cast<float>(width) / static_cast<float>(height), nearZ, farZ);
	}
}