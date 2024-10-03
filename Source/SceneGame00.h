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


// ゲームシーン
class SceneGame00 : public Scene
{
public:
	SceneGame00() {}
	~SceneGame00() override {}

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render() override;
	// シャドーマップ描画
	void RenderShadow();
	void RenderReflect();//追加


	// エネミーHPゲージ描画
	void RenderEnemyGauge(
		ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection
	);
	// 影用データ
	static constexpr POINT SHADOW_SIZE{ 1024,1024 };
	static constexpr POINT SHADOW_AREA{ 32,32 };
	static constexpr POINT REFLECT_SIZE{ 1024,1024 };//追加

	// ライト方向
	DirectX::XMFLOAT4 lightDirection = { -0.6f, -1.0f, -0.2f, 0.0f };	// ライト方向（下方向）

	//シャドー用行列
	DirectX::XMFLOAT4X4 shadowViewProjection;
	//リフレクト用行列
	DirectX::XMFLOAT4X4 reflectViewProjection;//追加


private:
	Player*				player = nullptr;
	CameraController*	cameraController = nullptr;
	Sprite*				gauge = nullptr;

	std::unique_ptr<WaterSurface> waterSurface = nullptr;

	float timer = 0;

};
