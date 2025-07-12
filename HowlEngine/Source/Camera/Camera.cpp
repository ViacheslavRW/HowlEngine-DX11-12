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

	void Camera::UpdateCameraVectors()
	{
		XMVECTOR frontVec = XMVectorSet(0.f, 0.f, 1.f, 0.f);

		XMMATRIX rotMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
		frontVec = XMVector3TransformCoord(frontVec, rotMatrix);

		XMStoreFloat3(&forward, XMVector3Normalize(frontVec));

		XMVECTOR forwardVec = XMLoadFloat3(&forward);
		XMVECTOR rightVec = XMVector3Normalize(XMVector3Cross(worldUp, forwardVec));
		XMVECTOR upVec = XMVector3Normalize(XMVector3Cross(forwardVec, rightVec));

		XMStoreFloat3(&right, rightVec);
		XMStoreFloat3(&up, upVec);
	}

	XMMATRIX& Camera::GetViewMatrix()
	{
		if (dirtyView)
		{
			if (dirtyRotation)
			{
				UpdateCameraVectors();
				dirtyRotation = false;
			}

			XMVECTOR pos = XMLoadFloat3(&position);
			XMVECTOR target = XMVectorAdd(pos, XMLoadFloat3(&forward));
			XMVECTOR upVec = XMLoadFloat3(&up);

			mViewMatrix = XMMatrixLookAtLH(pos, target, upVec);
			dirtyView = false;
		}

		return mViewMatrix;
	}

	void Camera::SetProjMatrix(UINT width, UINT height)
	{
		mProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), static_cast<float>(width) / static_cast<float>(height), nearZ, farZ);
	}

	void Camera::MoveForward(float deltaTime)
	{
		XMVECTOR f = XMLoadFloat3(&forward);
		XMVECTOR p = XMLoadFloat3(&position);
		p = XMVectorAdd(p, f * cameraSpeed * deltaTime);
		XMStoreFloat3(&position, p);
		dirtyView = true;
	}

	void Camera::MoveBackward(float deltaTime)
	{
		XMVECTOR f = XMLoadFloat3(&forward);
		XMVECTOR p = XMLoadFloat3(&position);
		p = XMVectorSubtract(p, f * cameraSpeed * deltaTime);
		XMStoreFloat3(&position, p);
		dirtyView = true;
	}

	void Camera::MoveLeft(float deltaTime)
	{
		XMVECTOR f = XMLoadFloat3(&right);
		XMVECTOR p = XMLoadFloat3(&position);
		p = XMVectorSubtract(p, f * cameraSpeed * deltaTime);
		XMStoreFloat3(&position, p);
		dirtyView = true;
	}

	void Camera::MoveRight(float deltaTime)
	{
		XMVECTOR f = XMLoadFloat3(&right);
		XMVECTOR p = XMLoadFloat3(&position);
		p = XMVectorAdd(p, f * cameraSpeed * deltaTime);
		XMStoreFloat3(&position, p);
		dirtyView = true;
	}

	void Camera::MoveUp(float deltaTime)
	{
		XMVECTOR worldUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		XMVECTOR p = XMLoadFloat3(&position);
		p = XMVectorAdd(p, worldUp * cameraSpeed * deltaTime);
		XMStoreFloat3(&position, p);
		dirtyView = true;
	}

	void Camera::MoveDown(float deltaTime)
	{
		XMVECTOR worldUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		XMVECTOR p = XMLoadFloat3(&position);
		p = XMVectorSubtract(p, worldUp * cameraSpeed * deltaTime);
		XMStoreFloat3(&position, p);
		dirtyView = true;
	}

	void Camera::RotateByRawMouse(int dx, int dy)
	{
		if (!isRotationEnabled) return;
		yaw += dx * cameraSensitivity;
		pitch += dy * cameraSensitivity;
		pitch = Clamp(pitch, -XM_PIDIV2 + 0.01f, XM_PIDIV2 - 0.01f);

		dirtyView = true;
		dirtyRotation = true;
	}

	void Camera::RotateLeft(float deltaTime)
	{
		if (!isRotationEnabled) return;
		yaw -= cameraRotSpeed * deltaTime;
		dirtyView = true;
		dirtyRotation = true;
	}

	void Camera::RotateRight(float deltaTime)
	{
		if (!isRotationEnabled) return;
		yaw += cameraRotSpeed * deltaTime;
		dirtyView = true;
		dirtyRotation = true;
	}

	void Camera::RotateDown(float deltaTime)
	{
		if (!isRotationEnabled) return;
		pitch += cameraRotSpeed * deltaTime;
		pitch = Clamp(pitch, -XM_PIDIV2 + 0.01f, XM_PIDIV2 - 0.01f);
		dirtyView = true;
		dirtyRotation = true;
	}

	void Camera::RotateUp(float deltaTime)
	{
		if (!isRotationEnabled) return;
		pitch -= cameraRotSpeed * deltaTime;
		pitch = Clamp(pitch, -XM_PIDIV2 + 0.01f, XM_PIDIV2 - 0.01f);
		dirtyView = true;
		dirtyRotation = true;
	}
}