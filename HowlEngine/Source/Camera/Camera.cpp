#include "pch.h"
#include "Camera.h"

namespace HEngine
{
	void Camera::Initialize()
	{
		pitch = 0.0f;
		yaw = 0.0f;
		roll = 0.0f;
		position = { 0.0f, 0.0f, -4.5f };
	}

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
		mProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), 800.0f / 600.0f, nearZ, farZ);
	}
}