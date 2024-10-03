#include "Sky.h"

// コンストラクタ
Sky::Sky()
{
	// ステージモデルを読み込み
	model = new Model("Data/Model/Sky/Sky.mdl");

}

// デストラクタ	
Sky::~Sky()
{
	// ステージモデルを破棄
	delete model;
}

// 更新処理
void Sky::Update(float elapsedTime)
{
	// 行列の更新
	UpdateTransform();
}

void Sky::UpdateTransform()
{
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);
}


// 描画処理
void Sky::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	model->UpdateTransform(transform);
	// シェーダーにモデルを描画してもらう
	shader->Draw(dc, model);
}

