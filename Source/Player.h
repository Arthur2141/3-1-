#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Character.h"
#include "ProjectileManager.h"
#include "Effect.h"

// プレイヤー
class Player : public Character
{
public:
	Player();
	~Player() override;

	// インスタンス取得
	static Player& Instance();

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	// デバッグ用GUI描画
	void DrawDebugGUI();

	// デバッグプリミティブ描画
	void DrawDebugPrimitive();

protected:
	// 着地した時に呼ばれる
	void OnLanding() override;

	// ダメージを受けた時に呼ばれる
	void OnDamaged() override;

	// 死亡した時に呼ばれる
	void OnDead() override;


private:
	// スティック入力値から移動ベクトルを取得
	DirectX::XMFLOAT3 GetMoveVec() const;

	// スティック入力値から射撃ベクトルを取得
	DirectX::XMFLOAT3 GetShootVec() const;

	// 移動入力処理
	bool InputMove(float elapsedTime);
	// 射撃移動入力処理
	bool InputShoot();

	// ジャンプ入力処理
	bool InputJump();

	// 弾丸入力処理
	bool InputProjectile();

	// 攻撃入力処理
	bool InputAttack();

	// 射撃入力処理
	bool InputShootMove(float elapsedTime);

	// 3D回転
	void TurnXYZ(float elapsedTime, float vx, float vy, float vz, float speed);



	// プレイヤーとエネミーとの衝突処理
	void CollisionPlayerVsEnemies();

	// 弾丸と敵の衝突処理
	void CollisionProjectilesVsEnemies();

	// ノードとエネミーの衝突処理
	void CollisionNodeVsEnemies(const char* nodeName, float nodeRadius);

	// 待機ステートへ遷移
	void TransitionIdleState();

	// 待機ステート更新処理
	void UpdateIdleState(float elapsedTime);

	// 移動ステートへ遷移
	void TransitionMoveState();

	// 移動ステート更新処理
	void UpdateMoveState(float elapsedTime);

	// ジャンプステートへ遷移
	void TransitionJumpState();

	// ジャンプステート更新処理
	void UpdateJumpState(float elapsedTime);

	// 着地ステートへ遷移
	void TransitionLandState();

	// 着地ステート更新処理
	void UpdateLandState(float elapsedTime);

	// 攻撃ステートへ遷移
	void TransitionAttackState();

	// 射撃ステートへ遷移
	void TransitionShootState();
	// 射撃ステート更新処理
	void UpdateShootState(float elapsedTime);



	// 攻撃ステート更新処理
	void UpdateAttackState(float elapsedTime);

	// ダメージステートへ遷移
	void TransitionDamageState();

	// ダメージステート更新処理
	void UpdateDamageState(float elapsedTime);

	// 死亡ステートへ遷移
	void TransitionDeathState();

	// 死亡ステート更新処理
	void UpdateDeathState(float elapsedTime);

	// 復活ステートへ遷移
	void TransitionReviveState();

	// 復活ステート更新処理
	void UpdateReviveState(float elapsedTime);




private:
	// アニメーション
	enum Animation
	{
		Anim_Attack,
		Anim_Death,
		Anim_Falling,
		Anim_GetHit1,
		Anim_GetHit2,
		Anim_Idle,
		Anim_Jump,
		Anim_Jump_Flip,
		Anim_Landing,
		Anim_Revive,
		Anim_Running,
		Anim_Walking
	};

	// ステート
	enum class State
	{
		Idle,
		Move,
		Jump,
		Land,
		Attack,
		Damage,
		Death,
		Revive,
		// 射撃状態
		Shoot,
	};

private:
	Model*				model = nullptr;
	float				moveSpeed = 5.0f;
	float				turnSpeed = DirectX::XMConvertToRadians(720);
	float				jumpSpeed = 20.0f;
	int					jumpCount = 0;
	int					jumpLimit = 2;
	ProjectileManager	projectileManager;
	Effect*				hitEffect = nullptr;
	Effect*				FlareEffect = nullptr;
	State				state = State::Idle;
	float				leftHandRadius = 0.4f;
	bool				attackCollisionFlag = false; 
	bool				visibleDebugPrimitive = true;
};
