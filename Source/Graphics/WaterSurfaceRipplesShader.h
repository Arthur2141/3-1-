#pragma once

#include <memory>
#include <wrl.h>
#include "Graphics/Shader.h"

class WaterSurfaceRipplesShader : public Shader
{
public:
	WaterSurfaceRipplesShader(ID3D11Device* device);
	~WaterSurfaceRipplesShader() override {}

	void Begin(ID3D11DeviceContext* dc, const RenderContext& rc) override;
	void Draw(ID3D11DeviceContext* dc, const Model* model) override;
	void End(ID3D11DeviceContext* dc) override;

private:
	static const int MaxBones = 128;

	struct CbScene
	{
		DirectX::XMFLOAT4X4	viewProjection;
		DirectX::XMFLOAT4	lightDirection;
		//�J�����ʒu
		DirectX::XMFLOAT4	viewPosition;
		//�V���h�[�}�b�v�s��
		DirectX::XMFLOAT4X4	shadowViewProjection;
		// x:�^�C�}�[
		DirectX::XMFLOAT4 param;
		//���˃}�b�v�s��
		DirectX::XMFLOAT4X4	reflectViewProjection;
		// �g��ʒu(x,y,z),�T�C�Y(w)
		DirectX::XMFLOAT4 param2;



	};

	struct CbMesh
	{
		DirectX::XMFLOAT4X4	boneTransforms[MaxBones];
	};

	struct CbSubset
	{
		DirectX::XMFLOAT4	materialColor;
	};


	Microsoft::WRL::ComPtr<ID3D11Buffer>			sceneConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			meshConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			subsetConstantBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout;

	Microsoft::WRL::ComPtr<ID3D11BlendState>		blendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState>		samplerState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>		shadowSamplerState;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	ripplesResourceView;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>		ripplesSamplerState;

	int textureWidth = 0;
	int textureHeight = 0;


};
