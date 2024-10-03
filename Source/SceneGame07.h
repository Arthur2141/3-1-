#pragma once

#include "Player.h"
#include "CameraController.h"
#include "Scene.h"
#include "Graphics/Sprite.h"

#include "Graphics/RenderTarget.h"
#include "Sky.h"
#include "StageWaterSurface.h"
#include "WaterSurface.h"

#include "Graphics/SkyBox.h"
#include "ParticleSystem.h"
#include "Graphics/Texture.h"


// �Q�[���V�[��
class SceneGame07 : public Scene
{
public:
	SceneGame07() {}
	~SceneGame07() override {}

	// ������
	void Initialize() override;

	// �I����
	void Finalize() override;

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
	void Render() override;
	// �V���h�[�}�b�v�`��
	void RenderShadow();
	void RenderReflect();//�ǉ�


	// �G�l�~�[HP�Q�[�W�`��
	void RenderEnemyGauge(
		ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection
	);
	// �e�p�f�[�^
	static constexpr POINT SHADOW_SIZE{ 2048,2048 };
	static constexpr POINT SHADOW_AREA{ 64,64 };
	static constexpr POINT REFLECT_SIZE{ 1024,1024 };//�ǉ�

	// ���C�g����
	DirectX::XMFLOAT4 lightDirection = { -0.6f, -1.0f, -0.2f, 0.0f };	// ���C�g�����i�������j

	//�V���h�[�p�s��
	DirectX::XMFLOAT4X4 shadowViewProjection;
	//���t���N�g�p�s��
	DirectX::XMFLOAT4X4 reflectViewProjection;//�ǉ�


private:
	Player*				player = nullptr;
	CameraController*	cameraController = nullptr;
	Sprite*				gauge = nullptr;

	std::unique_ptr<WaterSurface> waterSurface = nullptr;

	float timer = 0;

	std::unique_ptr<Model> earth;
	std::unique_ptr<RenderTarget> shadowMap = nullptr;

	std::unique_ptr<cParticleSystem> particle_bomb;
	std::unique_ptr<Texture> bomb_texture = nullptr;

	std::unique_ptr<cParticleSystem> particle_alpha;
	std::unique_ptr<cParticleSystem> particle_add;
	std::unique_ptr<cParticleSystem> particle_subtract;
	std::unique_ptr<Texture> particle_texture = nullptr;


};
