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


// ゲームシーン
class SceneGame01 : public Scene
{
public:
	SceneGame01() {}
	~SceneGame01() override {}

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
	void RenderReflect();
	void RenderCubeMap();	// 追加


	// エネミーHPゲージ描画
	void RenderEnemyGauge(
		ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection
	);
	// 影用データ
	static constexpr POINT SHADOW_SIZE{ 1024,1024 };
	static constexpr POINT SHADOW_AREA{ 32,32 };
	static constexpr POINT REFLECT_SIZE{ 1024,1024 };
	static constexpr POINT CUBE_MAP_SIZE{ 512,512 };//追加
	DirectX::XMFLOAT3 earth_position{ 0, 4.0f, 0 };

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

	std::unique_ptr<RenderTarget> shadowMap = nullptr;
	std::unique_ptr<RenderTarget> reflectMap = nullptr;//追加
	std::unique_ptr<RenderTarget> cubeMap = nullptr;//追加

	std::unique_ptr<Sky> sky = nullptr;
	std::unique_ptr<SkyBox> skybox = nullptr;
	std::unique_ptr<WaterSurface> waterSurface = nullptr;
	std::unique_ptr<EarthBall> earthBall = nullptr;

	float timer = 0;

};
