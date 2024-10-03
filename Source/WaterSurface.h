#pragma once

#include "Graphics/Model.h"
#include "StaticObject.h"

// メインステージ
class WaterSurface : public StaticObject
{
public:
	WaterSurface();
	~WaterSurface() override {}

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	//void Render(ID3D11DeviceContext* dc, Shader* shader) override {}

	//// 行列更新処理
	//void UpdateTransform();

	//void SetPosition(const DirectX::XMFLOAT3& p) { position = p; }
	//void SetScale(const DirectX::XMFLOAT3& s) { scale = s; }
	//void SetAngle(const DirectX::XMFLOAT3& a) { angle = a; }

private:
	//Model* model = nullptr;
	//DirectX::XMFLOAT3		position = { 0, 0, 0 };
	//DirectX::XMFLOAT3		angle = { 0, 0, 0 };
	//DirectX::XMFLOAT3		scale = { 1, 1, 1 };
	//DirectX::XMFLOAT4X4		transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
};

