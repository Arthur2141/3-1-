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
#include "ParticleSystem.h"
#include "Graphics/Texture.h"


// ゲームシーン
class SceneGame07 : public Scene
{
public:
	SceneGame07() {}
	~SceneGame07() override {}

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
	static constexpr POINT SHADOW_SIZE{ 2048,2048 };
	static constexpr POINT SHADOW_AREA{ 64,64 };
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

	std::unique_ptr<Model> earth;
	std::unique_ptr<RenderTarget> shadowMap = nullptr;

	std::unique_ptr<cParticleSystem> particle_bomb;
	std::unique_ptr<Texture> bomb_texture = nullptr;

	std::unique_ptr<cParticleSystem> particle_alpha;
	std::unique_ptr<cParticleSystem> particle_add;
	std::unique_ptr<cParticleSystem> particle_subtract;
	std::unique_ptr<Texture> particle_texture = nullptr;


};
