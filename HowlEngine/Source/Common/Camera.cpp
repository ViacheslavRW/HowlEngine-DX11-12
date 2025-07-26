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
			UpdateCameraBuffer();
			dirtyView = false;
		}

		return mViewMatrix;
	}

	void Camera::SetProjMatrix(UINT width, UINT height)
	{
		mProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), static_cast<float>(width) / static_cast<float>(height), nearZ, farZ);
	}

	void Camera::CreateCameraBuffer(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pDeviceContext)
	{
		mDevice = pDevice;
		mDeviceContext = pDeviceContext;

		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = sizeof(CameraBuffer);
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		HRESULT hr = pDevice->CreateBuffer(&bufferDesc, nullptr, mCameraBuffer.GetAddressOf());
		if (FAILED(hr)) std::cout << "FAILED_TO_CREATE_CAMERA_BUFFER" << std::endl;
	}

	void Camera::UpdateCameraBuffer()
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = mDeviceContext->Map(mCameraBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (SUCCEEDED(hr))
		{
			auto* cameraData = reinterpret_cast<CameraBuffer*>(mappedResource.pData);
			cameraData->cameraPosition = position;
			cameraData->pad = 0.0f;

			mDeviceContext->Unmap(mCameraBuffer.Get(), 0);
			mDeviceContext->PSSetConstantBuffers(3, 0, mCameraBuffer.GetAddressOf());
		}
	}

	void Camera::Release()
	{
		mDevice.Reset();
		mDeviceContext.Reset();
		mCameraBuffer.Reset();
	}

	void Camera::MoveForward(const float& deltaTime)
	{
		XMVECTOR f = XMLoadFloat3(&forward);
		XMVECTOR p = XMLoadFloat3(&position);
		p = XMVectorAdd(p, f * cameraSpeed * deltaTime);
		XMStoreFloat3(&position, p);
		dirtyView = true;
	}

	void Camera::MoveBackward(const float& deltaTime)
	{
		XMVECTOR f = XMLoadFloat3(&forward);
		XMVECTOR p = XMLoadFloat3(&position);
		p = XMVectorSubtract(p, f * cameraSpeed * deltaTime);
		XMStoreFloat3(&position, p);
		dirtyView = true;
	}

	void Camera::MoveLeft(const float& deltaTime)
	{
		XMVECTOR f = XMLoadFloat3(&right);
		XMVECTOR p = XMLoadFloat3(&position);
		p = XMVectorSubtract(p, f * cameraSpeed * deltaTime);
		XMStoreFloat3(&position, p);
		dirtyView = true;
	}

	void Camera::MoveRight(const float& deltaTime)
	{
		XMVECTOR f = XMLoadFloat3(&right);
		XMVECTOR p = XMLoadFloat3(&position);
		p = XMVectorAdd(p, f * cameraSpeed * deltaTime);
		XMStoreFloat3(&position, p);
		dirtyView = true;
	}

	void Camera::MoveUp(const float& deltaTime)
	{
		XMVECTOR worldUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		XMVECTOR p = XMLoadFloat3(&position);
		p = XMVectorAdd(p, worldUp * cameraSpeed * deltaTime);
		XMStoreFloat3(&position, p);
		dirtyView = true;
	}

	void Camera::MoveDown(const float& deltaTime)
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

	void Camera::RotateLeft(const float& deltaTime)
	{
		if (!isRotationEnabled) return;
		yaw -= cameraRotSpeed * deltaTime;
		dirtyView = true;
		dirtyRotation = true;
	}

	void Camera::RotateRight(const float& deltaTime)
	{
		if (!isRotationEnabled) return;
		yaw += cameraRotSpeed * deltaTime;
		dirtyView = true;
		dirtyRotation = true;
	}

	void Camera::RotateDown(const float& deltaTime)
	{
		if (!isRotationEnabled) return;
		pitch += cameraRotSpeed * deltaTime;
		pitch = Clamp(pitch, -XM_PIDIV2 + 0.01f, XM_PIDIV2 - 0.01f);
		dirtyView = true;
		dirtyRotation = true;
	}

	void Camera::RotateUp(const float& deltaTime)
	{
		if (!isRotationEnabled) return;
		pitch -= cameraRotSpeed * deltaTime;
		pitch = Clamp(pitch, -XM_PIDIV2 + 0.01f, XM_PIDIV2 - 0.01f);
		dirtyView = true;
		dirtyRotation = true;
	}
}