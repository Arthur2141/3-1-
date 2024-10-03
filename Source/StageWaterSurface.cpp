#include "StageWaterSurface.h"

// �R���X�g���N�^
StageWaterSurface::StageWaterSurface()
{
	// �X�e�[�W���f����ǂݍ���
	model = new Model("Data/Model/ExampleStage/Water_Surface.mdl");
}

// �f�X�g���N�^	
StageWaterSurface::~StageWaterSurface()
{
	// �X�e�[�W���f����j��
	delete model;
}

// �X�V����
void StageWaterSurface::Update(float elapsedTime)
{
	// ���͓��ɂ�邱�Ƃ͂Ȃ�
}

// �`�揈��
void StageWaterSurface::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	// �V�F�[�_�[�Ƀ��f����`�悵�Ă��炤
	shader->Draw(dc, model);
}

// ���C�L���X�g
bool StageWaterSurface::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return Collision::IntersectRayVsModel(start, end, model, hit);
}
