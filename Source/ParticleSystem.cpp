//#include "DxSystem.h"
#include "Misc.h"

#include <WICTextureLoader.h>

#include "ParticleSystem.h"
#include "Graphics/Graphics.h"


cParticleSystem::cParticleSystem(ID3D11ShaderResourceView* srv,int komax,int komay, bool anime,int num)
{
	Graphics& graphics = Graphics::Instance();
	ID3D11Device* device = graphics.GetDevice();

	// ���_�V�F�[�_�[
	{
		// �t�@�C�����J��
		FILE* fp = nullptr;
		fopen_s(&fp, "Shader\\PointSpriteVS.cso", "rb");
		_ASSERT_EXPR_A(fp, "CSO File not found");

		// �t�@�C���̃T�C�Y�����߂�
		fseek(fp, 0, SEEK_END);
		long csoSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		// ��������ɒ��_�V�F�[�_�[�f�[�^���i�[����̈��p�ӂ���
		std::unique_ptr<u_char[]> csoData = std::make_unique<u_char[]>(csoSize);
		fread(csoData.get(), csoSize, 1, fp);
		fclose(fp);

		// ���_�V�F�[�_�[����
		HRESULT hr = device->CreateVertexShader(csoData.get(), csoSize, nullptr, vertexShader.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// ���̓��C�A�E�g
		D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "PARAM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		hr = device->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), csoData.get(), csoSize, inputLayout.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
	// �W�I���g���V�F�[�_�[
	{
		// �t�@�C�����J��
		FILE* fp = nullptr;
		fopen_s(&fp, "Shader\\PointSpriteGS.cso", "rb");
		_ASSERT_EXPR_A(fp, "CSO File not found");

		// �t�@�C���̃T�C�Y�����߂�
		fseek(fp, 0, SEEK_END);
		long csoSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		// ��������ɒ��_�V�F�[�_�[�f�[�^���i�[����̈��p�ӂ���
		std::unique_ptr<u_char[]> csoData = std::make_unique<u_char[]>(csoSize);
		fread(csoData.get(), csoSize, 1, fp);
		fclose(fp);

		// �s�N�Z���V�F�[�_�[����
		HRESULT hr = device->CreateGeometryShader(csoData.get(), csoSize, nullptr, geometryShader.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// �s�N�Z���V�F�[�_�[
	{
		// �t�@�C�����J��
		FILE* fp = nullptr;
		fopen_s(&fp, "Shader\\PointSpritePS.cso", "rb");
		_ASSERT_EXPR_A(fp, "CSO File not found");

		// �t�@�C���̃T�C�Y�����߂�
		fseek(fp, 0, SEEK_END);
		long csoSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		// ��������ɒ��_�V�F�[�_�[�f�[�^���i�[����̈��p�ӂ���
		std::unique_ptr<u_char[]> csoData = std::make_unique<u_char[]>(csoSize);
		fread(csoData.get(), csoSize, 1, fp);
		fclose(fp);

		// �s�N�Z���V�F�[�_�[����
		HRESULT hr = device->CreatePixelShader(csoData.get(), csoSize, nullptr, pixelShader.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}


	data = new ParticleData[num];
	ZeroMemory(data, sizeof(ParticleData)*num);
	v = new VERTEX[num];
	ZeroMemory(v, sizeof(VERTEX)*num);

	this->komax = komax;
	this->komay = komay;
	//this->anime = anime;

	num_particles = num;
	for (int i = 0; i < num_particles; i++) {
		data[i].type = -1; 
	}
	//�p�[�e�B�N���쐬�Ɖ摜�ݒ�
	this->shaderResourceView = srv;


	//	���_�o�b�t�@�쐬
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		//���_�����̃o�b�t�@
		bd.ByteWidth = sizeof(VERTEX) * num_particles;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		HRESULT hr = device->CreateBuffer(&bd, NULL, vertexBuffer.GetAddressOf());
		assert(SUCCEEDED(hr));
	}

	// �萔�o�b�t�@����
	{
		D3D11_BUFFER_DESC cbd;
		ZeroMemory(&cbd, sizeof(cbd));
		cbd.Usage = D3D11_USAGE_DEFAULT;
		cbd.ByteWidth = sizeof(ConstantBufferForPerFrame);
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.CPUAccessFlags = 0;

		HRESULT hr = device->CreateBuffer(&cbd, nullptr, constantBuffer.GetAddressOf());
		assert(SUCCEEDED(hr));
	}
	// �u�����h�X�e�[�g
	{
		D3D11_BLEND_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.AlphaToCoverageEnable = false;
		desc.IndependentBlendEnable = false;
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		HRESULT hr = device->CreateBlendState(&desc, blendState[static_cast<int>(BlendStateId::Alpha)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
	{
		D3D11_BLEND_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.AlphaToCoverageEnable = false;
		desc.IndependentBlendEnable = false;
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		HRESULT hr = device->CreateBlendState(&desc, blendState[static_cast<int>(BlendStateId::Add)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
	{
		D3D11_BLEND_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.AlphaToCoverageEnable = false;
		desc.IndependentBlendEnable = false;
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		HRESULT hr = device->CreateBlendState(&desc, blendState[static_cast<int>(BlendStateId::Subtract)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
	// �[�x�X�e���V���X�e�[�g
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		HRESULT hr = device->CreateDepthStencilState(&desc, depthStencilState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
	// ���X�^���C�U�[�X�e�[�g
	{
		D3D11_RASTERIZER_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.FrontCounterClockwise = false;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0;
		desc.SlopeScaledDepthBias = 0;
		desc.DepthClipEnable = true;
		desc.ScissorEnable = false;
		desc.MultisampleEnable = true;
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_BACK;
		desc.AntialiasedLineEnable = false;

		HRESULT hr = device->CreateRasterizerState(&desc, rasterizerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
	// �T���v���X�e�[�g
	{
		D3D11_SAMPLER_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.MipLODBias = 0.0f;
		desc.MaxAnisotropy = 1;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = -FLT_MAX;
		desc.MaxLOD = FLT_MAX;
		desc.BorderColor[0] = 1.0f;
		desc.BorderColor[1] = 1.0f;
		desc.BorderColor[2] = 1.0f;
		desc.BorderColor[3] = 1.0f;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

		HRESULT hr = device->CreateSamplerState(&desc, samplerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

}
void cParticleSystem::Animation(ParticleData* data,float elapsedTime,float speed)
{
	float time = elapsedTime;
	if (data->type < 0) return;
	
	data->type += (time * speed);
	if (data->type >= komax * komay) data->type = -1;
}


void cParticleSystem::Update(float elapsedTime)
{
	for (int i = 0; i < num_particles; i++) {
		if (data[i].type < 0) continue;

		data[i].vx += data[i].ax * elapsedTime;
		data[i].vy += data[i].ay * elapsedTime;
		data[i].vz += data[i].az * elapsedTime;

		data[i].x += data[i].vx * elapsedTime;
		data[i].y += data[i].vy * elapsedTime;
		data[i].z += data[i].vz * elapsedTime;

		if (data[i].isAnime) { Animation(&data[i], elapsedTime, 30.0f); }
		else {
			data[i].timer -= elapsedTime;
			data[i].alpha = sqrtf(data[i].timer);
			if (data[i].timer < 0) 
				data[i].type = -1;

		}
	}
}

void cParticleSystem::Render(
	ID3D11DeviceContext* dc,
	const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection,
	int blend)
{


	ID3D11DeviceContext* device_context = dc;
	//�V�F�[�_�L����
	dc->VSSetShader(vertexShader.Get(), nullptr, 0);
	dc->GSSetShader(geometryShader.Get(), nullptr, 0);
	dc->PSSetShader(pixelShader.Get(), nullptr, 0);
	dc->IASetInputLayout(inputLayout.Get());


	//�萔�o�b�t�@�̍X�V
	ConstantBufferForPerFrame cb;
	cb.view = view;
	cb.projection = projection;
	cb.size = 0.1f;
	device_context->UpdateSubresource(constantBuffer.Get(), 0, nullptr, &cb, 0, 0);
	device_context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
	device_context->GSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
	device_context->PSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

	//�u�����h�X�e�[�g�ݒ�
	device_context->OMSetBlendState(blendState[blend].Get(), nullptr, 0xFFFFFFFF);
	//���X�^���C�U�\�ݒ�
	device_context->RSSetState(rasterizerState.Get());
	//�f�v�X�X�e���V���X�e�[�g�ݒ�
	device_context->OMSetDepthStencilState(depthStencilState.Get(), 1);
	//�v���~�e�B�u�E�g�|���W�[���Z�b�g
	device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	//�V�F�[�_���\�[�X�r���[�ݒ�
	device_context->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());
	//�T���v���X�e�[�g�ݒ�
	device_context->PSSetSamplers(0, 1, samplerState.GetAddressOf());


	int n = 0; //�p�[�e�B�N��������
	for (int i = 0; i < num_particles; i++)
	{
		if (data[i].type < 0) continue;

		v[n].Pos.x = data[i].x;
		v[n].Pos.y = data[i].y;
		v[n].Pos.z = data[i].z;
		v[n].Tex.x = data[i].w;
		v[n].Tex.y = data[i].h;		  
		v[n].Color.x = v[n].Color.y = v[n].Color.z = 1.0f;
		v[n].Color.w = data[i].alpha;
		
		v[n].Param.x = 0;
		v[n].Param.y = data[i].type;
		v[n].Param.z = (float)komax; //���R�}��
		v[n].Param.w = (float)komay; //�c�R�}��
		++n;
	}
	//���_�f�[�^�X�V
	device_context->UpdateSubresource(vertexBuffer.Get(), 0, nullptr, v, 0, 0);

	//�o�[�e�b�N�X�o�b�t�@�[���Z�b�g
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	device_context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	//�����_�����O(�C���f�b�N�X�t��)
	device_context->Draw(n, 0);

	//�f�v�X�X�e���V���X�e�[�g�ݒ�
	device_context->OMSetDepthStencilState(depthStencilState.Get(), 1);

	//�V�F�[�_������
	//shader->Inactivate();
	dc->VSSetShader(nullptr, nullptr, 0);
	dc->GSSetShader(nullptr, nullptr, 0);
	dc->PSSetShader(nullptr, nullptr, 0);

	dc->IASetInputLayout(nullptr);

}

void cParticleSystem::Set(int type,float timer,bool isAnime,
	DirectX::XMFLOAT3 p, DirectX::XMFLOAT3 v, DirectX::XMFLOAT3 f, DirectX::XMFLOAT2 size)
{
	for (int i = 0; i < num_particles; i++) {
		if (data[i].type >= 0) continue;
		data[i].type = (float)type;
		data[i].x = p.x;
		data[i].y = p.y;
		data[i].z = p.z;
		data[i].vx = v.x;
		data[i].vy = v.y;
		data[i].vz = v.z;
		data[i].ax = f.x;
		data[i].ay = f.y;
		data[i].az = f.z;
		data[i].w = size.x;
		data[i].h = size.y;
		data[i].alpha = 1.0f;
		data[i].timer = timer;
		data[i].isAnime = isAnime;

		break;
	}
}

void cParticleSystem::Snow(DirectX::XMFLOAT3 pos, int max)
{
	for (int i = 0; i < max; i++) {
		//�����ʒu
		DirectX::XMFLOAT3 p = { 0,0,0 };
		p.x = pos.x + (rand() % 10001 - 5000) * 0.01f;
		p.y = pos.y;
		p.z = pos.z + (rand() % 10001 - 5000) * 0.01f;
		//��������
		DirectX::XMFLOAT3 v = { 0,0,0 };
		v.y = -(rand() % 10001) * 0.0002f - 0.002f;
		//��
		DirectX::XMFLOAT3 f = { 0,0,0 };
		f.x = (rand() % 10001) * 0.00001f + 0.1f;
		f.z = (rand() % 10001 - 5000) * 0.00001f;
		//�傫��
		DirectX::XMFLOAT2 s = { .2f,.2f };

		Set(12, 5, false,p, v, f, s);

	}
}

void cParticleSystem::Spark(DirectX::XMFLOAT3 pos, int max)
{
	for (int i = 0; i < max; i++) {
		DirectX::XMFLOAT3 p;
		p.x = pos.x;
		p.y = pos.y;
		p.z = pos.z;

		DirectX::XMFLOAT3 v = { 0,0,0 };
		v.x = (rand() % 10001 - 5000) * 0.0001f;
		v.y = (rand() % 10001) * 0.0002f + 1.2f;
		v.z = (rand() % 10001 - 5000) * 0.0001f;

		DirectX::XMFLOAT3 f = { 0,-1.2f,0 };
		DirectX::XMFLOAT2 s = { 0.05f,0.05f };

		Set(2, 3, false,p, v, f, s);

	}
}
void cParticleSystem::Fire(DirectX::XMFLOAT3 pos, int max)
{
	for (int i = 0; i < max; i++) {
		DirectX::XMFLOAT3 p;
		DirectX::XMFLOAT3 v = { 0,0,0 };
		DirectX::XMFLOAT3 f = { 0,-2.0f,0 };
		DirectX::XMFLOAT2 s = { 0.8f,0.8f };

		p.x = pos.x + (rand() % 10001 - 5000) * 0.00005f;
		p.y = pos.y + (rand() % 10001) * 0.0001f + 0.2f;
		p.z = pos.z + (rand() % 10001 - 5000) * 0.00005f;

		v.x = (rand() % 10001 - 5000) * 0.0002f;
		v.y = (rand() % 10001) * 0.0004f + 0.005f;
		v.z = (rand() % 10001 - 5000) * 0.0001f;

		Set(13, 0.4f, false,p, v, f, s);

	}
}

void cParticleSystem::Smoke(DirectX::XMFLOAT3 pos, int max)
{
	for (int i = 0; i < max; i++) {
		DirectX::XMFLOAT3 p;
		DirectX::XMFLOAT3 v = { 0,0,0 };
		DirectX::XMFLOAT3 f = { 0,-2.0f,0 };
		DirectX::XMFLOAT2 s = { .4f,.4f };

		p.x = pos.x + (rand() % 10001 - 5000) * 0.0001f;
		p.y = pos.y + (rand() % 10001) * 0.0001f + 0.5f;
		p.z = pos.z + (rand() % 10001 - 5000) * 0.0001f;

		v.x = (rand() % 10001 - 5000) * 0.0001f;
		v.y = (rand() % 10001) * 0.0004f + 0.005f;
		v.z = (rand() % 10001 - 5000) * 0.0001f;
		Set(14, 0.6f, false, p, v, f, s);

	}
}




