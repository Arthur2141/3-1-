#pragma once

#include "Player.h"
#include "CameraController.h"
#include "Scene.h"
#include "Graphics/Sprite.h"

#include "Graphics/RenderTarget.h"
#include "Sky.h"
#include "StageWaterSurface.h"
#include "WaterSurface.h"
#include "EarthBall.h"

#include "Graphics/SkyBox.h"


// �Q�[���V�[��
class SceneGame01 : public Scene
{
public:
	SceneGame01() {}
	~SceneGame01() override {}

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
	void RenderReflect();
	void RenderCubeMap();	// �ǉ�


	// �G�l�~�[HP�Q�[�W�`��
	void RenderEnemyGauge(
		ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection
	);
	// �e�p�f�[�^
	static constexpr POINT SHADOW_SIZE{ 1024,1024 };
	static constexpr POINT SHADOW_AREA{ 32,32 };
	static constexpr POINT REFLECT_SIZE{ 1024,1024 };
	static constexpr POINT CUBE_MAP_SIZE{ 512,512 };//�ǉ�
	DirectX::XMFLOAT3 earth_position{ 0, 4.0f, 0 };

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

	std::unique_ptr<RenderTarget> shadowMap = nullptr;
	std::unique_ptr<RenderTarget> reflectMap = nullptr;//�ǉ�
	std::unique_ptr<RenderTarget> cubeMap = nullptr;//�ǉ�

	std::unique_ptr<Sky> sky = nullptr;
	std::unique_ptr<SkyBox> skybox = nullptr;
	std::unique_ptr<WaterSurface> waterSurface = nullptr;
	std::unique_ptr<EarthBall> earthBall = nullptr;

	float timer = 0;

};
