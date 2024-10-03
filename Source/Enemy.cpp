#include "Enemy.h"
#include "EnemyManager.h"
#include "Graphics/Graphics.h"

// デバッグプリミティブ描画
void Enemy::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	// 衝突判定用のデバッグ円柱を描画
	debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
}

// デバッグ用GUI描画
void Enemy::DrawDebugGUI()
{
	// 位置
	ImGui::InputFloat3("Position", &position.x);
	// 半径
	ImGui::InputFloat("Radius", &radius);
	// 高さ
	ImGui::InputFloat("Height", &height);
	// 健康
	ImGui::InputInt("Health", &health);
}

// 破棄
void Enemy::Destroy()
{
	EnemyManager::Instance().Remove(this);
}
