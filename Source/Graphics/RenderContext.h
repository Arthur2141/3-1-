#pragma once

#include <DirectXMath.h>
#include <wrl.h>

// レンダーコンテキスト
struct RenderContext
{
	DirectX::XMFLOAT4X4		view;
	DirectX::XMFLOAT4X4		projection;
	DirectX::XMFLOAT4		lightDirection;

	DirectX::XMFLOAT4		viewPosition;

	ID3D11ShaderResourceView* shaderResourceView = nullptr;
	DirectX::XMFLOAT4X4 shadowViewProjection;
	float timer = 0.0f;
	ID3D11ShaderResourceView* envShaderResourceView = nullptr;

	ID3D11ShaderResourceView* reflectShaderResourceView = nullptr;
	DirectX::XMFLOAT4X4 reflectViewProjection;

	float water_surface_height;

	DirectX::XMFLOAT3 ripplesPosition;
	float ripplesSize;

	DirectX::XMFLOAT4X4 cubeMapViewMatrix[6];
	DirectX::XMFLOAT4X4 cubeMapProjectionMatrix;
	ID3D11ShaderResourceView* cubeMapShaderResourceView = nullptr;


};
