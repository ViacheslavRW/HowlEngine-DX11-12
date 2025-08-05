#include "pch.h"
#include "Skybox.h"


namespace HEngine
{
	void Skybox::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ID3D11InputLayout* pInputLayout, XMMATRIX& _viewMatrix, XMMATRIX& _projectionMatrix)
	{
		mDevice = pDevice;
		mDeviceContext = pDeviceContext;
		mInputLayout = pInputLayout;
		cashedViewMatrix = _viewMatrix;
		cashedProjMatrix = _projectionMatrix;

		LoadSkyboxTexture();
		CreateSkyboxBuffers();
	}

	void Skybox::LoadSkyboxTexture()
	{
		ScratchImage images[faces];
		HRESULT hr;

		for (int i = 0; i < faces; ++i)
		{
			hr = LoadFromWICFile(skyboxPath[i].c_str(), WIC_FLAGS_NONE, nullptr, images[i]);
			if (FAILED(hr))
			{
				std::cout << "FAILED_TO_LOAD_TEXTURE" << std::string(skyboxPath[i].begin(), skyboxPath[i].end()) << "\n";
				return;
			}
		}

		// verify dimensions
		const TexMetadata& metadata0 = images[0].GetMetadata();
		for (int i = 1; i < faces; ++i)
		{
			const TexMetadata& metadata = images[i].GetMetadata();
			if (metadata.width != metadata0.width || metadata.height != metadata0.height || metadata.format != metadata0.format)
			{
				std::cout << "CUBEMAP_TEXTURES_ARE_NOT_INDENTICAL" << "\n";
			}
		}
		
		std::vector<D3D11_SUBRESOURCE_DATA> initData(metadata0.mipLevels * faces);
		const Image* srcImages = images[0].GetImages(); 
		for (UINT item = 0; item < metadata0.mipLevels; ++item)
		{
			for (UINT face = 0; face < faces; ++face)
			{
				UINT index = item * faces + face;
				const Image* img = images[face].GetImage(item, 0, 0); 
				initData[index].pSysMem = img->pixels;
				initData[index].SysMemPitch = static_cast<UINT>(img->rowPitch);
				initData[index].SysMemSlicePitch = static_cast<UINT>(img->slicePitch);
			}
		}

		// create texture
		D3D11_TEXTURE2D_DESC texDesc = {};
		texDesc.Width = static_cast<UINT>(metadata0.width);
		texDesc.Height = static_cast<UINT>(metadata0.height);
		texDesc.MipLevels = metadata0.mipLevels;
		texDesc.ArraySize = faces;
		texDesc.Format = metadata0.format;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		hr = mDevice->CreateTexture2D(&texDesc, initData.data(), mSkyboxTexture2D.GetAddressOf());
		if (FAILED(hr))
		{
			std::cout << "FAILED_TO_CREATE_SKYBOX_TEXTURE_2D"<< "\n";
		}

		// create SRV
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = metadata0.format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MipLevels = texDesc.MipLevels;
		srvDesc.TextureCube.MostDetailedMip = 0;

		hr = mDevice->CreateShaderResourceView(mSkyboxTexture2D.Get(), &srvDesc, mSkyboxSRV.GetAddressOf());
		if (FAILED(hr))
		{
			std::cout << "FAILED_TO_CREATE_SRV_FOR_SKYBOX" << "\n";
		}
	}

	void Skybox::CreateSkyboxBuffers()
	{
		HRESULT hr;

		// vertex buffer
		D3D11_BUFFER_DESC bDesc = {};
		bDesc.ByteWidth = static_cast<UINT>(vertices.size() * sizeof(TR::SkyboxVertex));
		bDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bDesc.CPUAccessFlags = 0;
		bDesc.MiscFlags = 0;
		bDesc.StructureByteStride = sizeof(TR::SkyboxVertex);

		D3D11_SUBRESOURCE_DATA srData = {};
		srData.pSysMem = vertices.data();
		srData.SysMemPitch = 0;
		srData.SysMemSlicePitch = 0;

		hr = mDevice->CreateBuffer(&bDesc, &srData, mVertexBuffer.GetAddressOf());
		if (FAILED(hr)) std::cout << "FAILED_TO_CREATE_VERTEX_BUFFER" << std::endl;

		// invex buffer
		D3D11_BUFFER_DESC indBuffDesc = {};
		indBuffDesc.ByteWidth = static_cast<UINT>(sizeof(UINT32) * indices.size());
		indBuffDesc.Usage = D3D11_USAGE_IMMUTABLE;
		indBuffDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indBuffDesc.CPUAccessFlags = 0;
		indBuffDesc.MiscFlags = 0;
		indBuffDesc.StructureByteStride = sizeof(UINT32);

		D3D11_SUBRESOURCE_DATA iData = {};
		iData.pSysMem = indices.data();

		hr = mDevice->CreateBuffer(&indBuffDesc, &iData, mIndexBuffer.GetAddressOf());
		if (FAILED(hr)) std::cout << "FAILED_TO_CREATE_INDEX_BUFFER" << std::endl;

		// constant buffer
		D3D11_BUFFER_DESC cbDesc = {};
		cbDesc.ByteWidth = sizeof(SkyboxCB);
		cbDesc.Usage = D3D11_USAGE_DEFAULT;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = 0;

		hr = mDevice->CreateBuffer(&cbDesc, nullptr, mConstantBuffer.GetAddressOf());
		if (FAILED(hr)) std::cout << "FAILED_TO_CREATE_CONSTANT_BUFFER" << std::endl;
	}

	void Skybox::Bind(const XMMATRIX& viewMatrix)
	{
		SkyboxCB cb = {};
		XMMATRIX view = viewMatrix;
		view.r[3].m128_f32[0] = 0.0f;
		view.r[3].m128_f32[1] = 0.0f;
		view.r[3].m128_f32[2] = 0.0f;
		cb.viewProjMatrix = XMMatrixTranspose(view * cashedProjMatrix);

		mDeviceContext->UpdateSubresource(mConstantBuffer.Get(), 0, nullptr, &cb, 0, 0);
		mDeviceContext->VSSetConstantBuffers(0, 1, mConstantBuffer.GetAddressOf());

		UINT stride = sizeof(TR::SkyboxVertex);
		UINT offset = 0;
		mDeviceContext->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &stride, &offset);
		mDeviceContext->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		mDeviceContext->IASetInputLayout(mInputLayout);

		ID3D11ShaderResourceView* srvArray[] = { mSkyboxSRV.Get() };
		mDeviceContext->PSSetShaderResources(0, 1, srvArray);
	}

	void Skybox::Draw()
	{
		mDeviceContext->DrawIndexed(indices.size(), 0, 0);
	}

	void Skybox::RenderSkybox(const XMMATRIX& viewMatrix)
	{
		Bind(viewMatrix);
		Draw();
	}
}