#include "WaterSurface.h"




// �R���X�g���N�^
WaterSurface::WaterSurface()
{
	// �X�e�[�W���f����ǂݍ���
	model = new Model("Data/Model/ExampleStage/water_surface1.mdl");

}


// �X�V����
void WaterSurface::Update(float elapsedTime)
{
	// �s��̍X�V
	UpdateTransform();
}

//void StaticObject::UpdateTransform()
//{
//	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
//	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
//	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
//	DirectX::XMMATRIX W = S * R * T;
//	DirectX::XMStoreFloat4x4(&transform, W);
//}


//// �`�揈��
//void StaticObject::Render(ID3D11DeviceContext* dc, Shader* shader)
//{
//	model->UpdateTransform(transform);
//	// �V�F�[�_�[�Ƀ��f����`�悵�Ă��炤
//	shader->Draw(dc, model);
//}
//

