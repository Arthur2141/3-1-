#include "Enemy.h"
#include "EnemyManager.h"
#include "Graphics/Graphics.h"

// fobOv~eBu`æ
void Enemy::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	// ÕË»èpÌfobO~ð`æ
	debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
}

// fobOpGUI`æ
void Enemy::DrawDebugGUI()
{
	// Êu
	ImGui::InputFloat3("Position", &position.x);
	// ¼a
	ImGui::InputFloat("Radius", &radius);
	// ³
	ImGui::InputFloat("Height", &height);
	// N
	ImGui::InputInt("Health", &health);
}

// jü
void Enemy::Destroy()
{
	EnemyManager::Instance().Remove(this);
}
