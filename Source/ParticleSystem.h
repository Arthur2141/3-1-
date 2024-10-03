#pragma once
#include <d3d11.h>
#include <vector>
#include <DirectXMath.h>
#include <wrl.h>
#include <memory>

//#include "Texture.h"
//#include "Shader.h"

enum class BlendStateId
{
	Alpha,
	Add,
	Subtract,
	Max,
};

class cParticleSystem{
private:
	struct VERTEX
	{
		DirectX::XMFLOAT3 Pos;	//�ʒu
		DirectX::XMFLOAT3 Normal;//�@��
		DirectX::XMFLOAT2 Tex;	//UV���W
		DirectX::XMFLOAT4 Color;	//���_�F		
		DirectX::XMFLOAT4 Param;	//�ėp�p�����[�^
	};
	struct ParticleData
	{
		float x, y, z;
		float w, h;
		float aw, ah;
		float vx, vy, vz;
		float ax, ay, az;
		float alpha;
		float timer;
		float type;
		bool isAnime;
	};
	ParticleData* data; //�p�[�e�B�N���f�[�^

	VERTEX* v;			//���_�f�[�^

	int num_particles = 0;
	int komax, komay;
	//bool anime;

	// ���_�f�[�^
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer = nullptr;
	// �萔�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>	geometryShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout;
	Microsoft::WRL::ComPtr<ID3D11BlendState>			blendState[3];
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>		rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depthStencilState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>			samplerState;


	//�e�N�X�`�����p
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shaderResourceView;

	// �萔�o�b�t�@�̃f�[�^��`
	struct ConstantBufferForPerFrame {
		DirectX::XMFLOAT4X4	view;			// �r���[�ϊ��s��
		DirectX::XMFLOAT4X4	projection;		// �����ϊ��s��
		float		size;
		float		dummy0;
		float		dummy1;
		float		dummy2;
	};
	int textureWidth = 0;
	int textureHeight = 0;

public:

	//cParticleSystem(int num = 1000);
	cParticleSystem(ID3D11ShaderResourceView* srv,int komax = 4,int komay = 4,bool anime = false,int num = 1000);

	~cParticleSystem() { delete[] data; delete[] v; }
	void Update(float elapsedTime);

	void Animation(ParticleData* data,float elapsedTime,float speed = 0.1f);

	void Render(ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4 & v, const DirectX::XMFLOAT4X4& p,
		int blend = static_cast<int>(BlendStateId::Alpha));

	void Set(
		int type,
		float timer,
		bool isAnime,
		DirectX::XMFLOAT3 p,
		DirectX::XMFLOAT3 v = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3 f = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT2 size = DirectX::XMFLOAT2(1.0f, 1.0f)
	);

	void Snow(DirectX::XMFLOAT3 pos, int max);

	void Spark(DirectX::XMFLOAT3 pos, int max);
	void Fire(DirectX::XMFLOAT3 pos, int max);
	void Smoke(DirectX::XMFLOAT3 pos, int max);
};