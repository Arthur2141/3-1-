#include "StageWaterSurface.h"

// コンストラクタ
StageWaterSurface::StageWaterSurface()
{
	// ステージモデルを読み込み
	model = new Model("Data/Model/ExampleStage/Water_Surface.mdl");
}

// デストラクタ	
StageWaterSurface::~StageWaterSurface()
{
	// ステージモデルを破棄
	delete model;
}

// 更新処理
void StageWaterSurface::Update(float elapsedTime)
{
	// 今は特にやることはない
}

// 描画処理
void StageWaterSurface::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	// シェーダーにモデルを描画してもらう
	shader->Draw(dc, model);
}

// レイキャスト
bool StageWaterSurface::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return Collision::IntersectRayVsModel(start, end, model, hit);
}
