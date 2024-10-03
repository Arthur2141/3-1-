#include "Enemy.h"
#include "EnemyManager.h"
#include "Graphics/Graphics.h"

// �f�o�b�O�v���~�e�B�u�`��
void Enemy::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	// �Փ˔���p�̃f�o�b�O�~����`��
	debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
}

// �f�o�b�O�pGUI�`��
void Enemy::DrawDebugGUI()
{
	// �ʒu
	ImGui::InputFloat3("Position", &position.x);
	// ���a
	ImGui::InputFloat("Radius", &radius);
	// ����
	ImGui::InputFloat("Height", &height);
	// ���N
	ImGui::InputInt("Health", &health);
}

// �j��
void Enemy::Destroy()
{
	EnemyManager::Instance().Remove(this);
}
