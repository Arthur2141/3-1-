#include <imgui.h>
#include "Player.h"
#include "Input/Input.h"
#include "Camera.h"
#include "Graphics/Graphics.h"
#include "EnemyManager.h"
#include "Collision.h"
#include "ProjectileStraite.h"
#include "ProjectileHoming.h"

static Player* instance = nullptr;

// インスタンス取得
Player& Player::Instance()
{
	return *instance;
}

// コンストラクタ
Player::Player()
{
	// インスタンスポインタ設定
	instance = this;

	model = new Model("Data/Model/Jammo/Jammo.mdl");

	// モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.01f;

	// 高さ
	height = 1.5f;

	// ヒットエフェクト読み込み
	hitEffect = new Effect("Data/Effect/Hit.efk");
	FlareEffect = new Effect("Data/Effect/Flare.efk");

	// 待機ステートへ遷移
	TransitionIdleState();
}

// デストラクタ
Player::~Player()
{
	delete hitEffect;

	delete model;
}

// 更新処理
void Player::Update(float elapsedTime)
{
	// ステート毎の処理
	switch (state)
	{
	case State::Idle:
		UpdateIdleState(elapsedTime);
		// オブジェクト行列を更新
		UpdateTransform();

		break;

	case State::Move:
		UpdateMoveState(elapsedTime);
		// オブジェクト行列を更新
		UpdateTransform();

		break;

	case State::Jump:
		UpdateJumpState(elapsedTime);
		// オブジェクト行列を更新
		UpdateTransform();

		break;

	case State::Land:
		UpdateLandState(elapsedTime);
		// オブジェクト行列を更新
		UpdateTransform();

		break;

	case State::Attack:
		UpdateAttackState(elapsedTime);
		// オブジェクト行列を更新
		UpdateTransform();

		break;

	case State::Damage:
		UpdateDamageState(elapsedTime);
		// オブジェクト行列を更新
		UpdateTransform();

		break;

	case State::Death:
		UpdateDeathState(elapsedTime);
		// オブジェクト行列を更新
		UpdateTransform();

		break;

	case State::Revive:
		UpdateReviveState(elapsedTime);
		// オブジェクト行列を更新
		UpdateTransform();

		break;

	case State::Shoot:
		UpdateShootState(elapsedTime);
		// オブジェクト行列を更新
		UpdateTransform2();

		break;
	}

	// 速力更新処理
	UpdateVelocity(elapsedTime);

	// 無敵時間更新
	UpdateInvincibleTimer(elapsedTime);

	// 弾丸更新処理
	projectileManager.Update(elapsedTime);

	// プレイヤーと敵との衝突処理
	CollisionPlayerVsEnemies();

	// 弾丸と敵との衝突処理
	CollisionProjectilesVsEnemies();

	//// オブジェクト行列を更新
	//UpdateTransform();

	// モデルアニメーション更新処理
	model->UpdateAnimation(elapsedTime);

	// モデル行列更新
	model->UpdateTransform(transform);
}

// 描画処理
void Player::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model);

	// 弾丸描画処理
	projectileManager.Render(dc, shader);
}

// デバッグ用GUI描画
void Player::DrawDebugGUI()
{
	if (ImGui::CollapsingHeader("Player", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// デバッグプリミティブ表示
		ImGui::Checkbox("VisibleDebugPrimitive", &visibleDebugPrimitive);
		// ステート
		switch (state)
		{
		case State::Idle:	ImGui::LabelText("State", "Idle");	break;
		case State::Move:	ImGui::LabelText("State", "Move");	break;
		case State::Jump:	ImGui::LabelText("State", "Jump");	break;
		case State::Land:	ImGui::LabelText("State", "Land");	break;
		case State::Attack:	ImGui::LabelText("State", "Attack");	break;
		case State::Damage:	ImGui::LabelText("State", "Damage");	break;
		case State::Death:	ImGui::LabelText("State", "Death");	break;
		case State::Revive:	ImGui::LabelText("State", "Revive");	break;
		}

		// 位置
		ImGui::InputFloat3("Position", &position.x);
		// 回転
		DirectX::XMFLOAT3 a;
		a.x = DirectX::XMConvertToDegrees(angle.x);
		a.y = DirectX::XMConvertToDegrees(angle.y);
		a.z = DirectX::XMConvertToDegrees(angle.z);
		ImGui::InputFloat3("Angle", &a.x);
		angle.x = DirectX::XMConvertToRadians(a.x);
		angle.y = DirectX::XMConvertToRadians(a.y);
		angle.z = DirectX::XMConvertToRadians(a.z);
		// スケール
		ImGui::InputFloat3("Scale", &scale.x);
		// 移動速度
		ImGui::InputFloat("MoveSpeed", &moveSpeed);
		// 旋回速度
		float turnDeg = DirectX::XMConvertToDegrees(turnSpeed);
		ImGui::InputFloat("TurnSpeed", &turnDeg);
		turnSpeed = DirectX::XMConvertToRadians(turnDeg);
		// 半径
		ImGui::InputFloat("Radius", &radius);
		// 高さ
		ImGui::InputFloat("Height", &height);
		// 摩擦
		ImGui::InputFloat("Friction", &friction);
		// 加速度
		ImGui::InputFloat("Acceleration", &acceleration);
		// 空中制御係数
		ImGui::InputFloat("AirControl", &airControl);
	}
}

// デバッグプリミティブ描画
void Player::DrawDebugPrimitive()
{
	if (!visibleDebugPrimitive) return;

	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	// 衝突判定用のデバッグ円柱を描画
	debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));

	// 弾丸デバッグプリミティブ描画
	projectileManager.DrawDebugPrimitive();

	// 攻撃衝突用の左手ノードのデバッグ球を描画
	if (attackCollisionFlag)
	{
		Model::Node* leftHandBone = model->FindNode("mixamorig:LeftHand");
		debugRenderer->DrawSphere(DirectX::XMFLOAT3(
			leftHandBone->worldTransform._41,
			leftHandBone->worldTransform._42,
			leftHandBone->worldTransform._43),
			leftHandRadius,
			DirectX::XMFLOAT4(1, 0, 0, 1)
		);
	}
}

// 着地した時に呼ばれる
void Player::OnLanding()
{
	jumpCount = 0;

	// ダメージ、死亡ステート時は着地した時にステート遷移しないようにする
	if (state != State::Damage && state != State::Death)
	{
		// 下方向の速力が一定以上なら着地
		if (velocity.y < gravity * 5.0f)
		{
			// 着地ステートへ遷移
			TransitionLandState();
		}
	}
}

// ダメージを受けた時に呼ばれる
void Player::OnDamaged()
{
	// ダメージステートへ遷移
	TransitionDamageState();
}

// 死亡した時に呼ばれる
void Player::OnDead()
{
	// 死亡ステートへ遷移
	TransitionDeathState();
}
// 旋回処理
void Player::TurnXYZ(float elapsedTime, float vx, float vy, float vz, float speed)
{
	speed *= elapsedTime;

	// ゼロベクトルの場合は処理する必要なし
	float length = sqrtf(vx * vx + vz * vz);
	if (length < 0.001f) return;

	// 単位ベクトル化
	vx /= length;
	vz /= length;

	// 自身の回転値から前方向を求める
	float frontX = sinf(angle.y);
	float frontZ = cosf(angle.y);

	// 回転角を求めるため、２つの単位ベクトルの内積を計算する
	float dot = (frontX * vx) + (frontZ * vz);

	// 内積値は-1.0～1.0で表現されており、２つの単位ベクトルの角度が
	// 小さいほど1.0に近づくという性質を利用して回転速度を調整する
	float rot = 1.0f - dot;
	if (rot > speed) rot = speed;

	// 左右判定を行うために２つの単位ベクトルの外積を計算する
	float cross = (frontX * vz) - (frontZ * vx);

	// 2Dの外積値が正の場合か負の場合によって左右判定が行える
	// 左右判定を行うことによって左右回転を選択する
	if (cross < 0.0f)
	{
		angle.y += rot;
	}
	else
	{
		angle.y -= rot;
	}

	angle.x = vy;

}



// スティック入力値から移動ベクトルを取得
DirectX::XMFLOAT3 Player::GetMoveVec() const
{
	// 入力情報を取得
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();

	// カメラ方向とスティックの入力値によって進行方向を計算する
	Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
	const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

	// 移動ベクトルはXZ平面に水平なベクトルなるようにする

	// カメラ右方向ベクトルをXZ単位ベクトルに変換
	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
	if (cameraRightLength > 0.0f)
	{
		// 単位ベクトル化
		cameraRightX /= cameraRightLength;
		cameraRightZ /= cameraRightLength;
	}

	// カメラ前方向ベクトルをXZ単位ベクトルに変換
	float cameraFrontX = cameraFront.x;
	float cameraFrontZ = cameraFront.z;
	float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
	if (cameraFrontLength > 0.0f)
	{
		// 単位ベクトル化
		cameraFrontX /= cameraFrontLength;
		cameraFrontZ /= cameraFrontLength;
	}

	// スティックの水平入力値をカメラ右方向に反映し、
	// スティックの垂直入力値をカメラ前方向に反映し、
	// 進行ベクトルを計算する
	DirectX::XMFLOAT3 vec;
	vec.x = (cameraRightX * ax) + (cameraFrontX * ay);
	vec.z = (cameraRightZ * ax) + (cameraFrontZ * ay);
	// Y軸方向には移動しない
	vec.y = 0.0f;

	return vec;
}

// バイオハザード移動
DirectX::XMFLOAT3 Player::GetShootVec() const
{
	// 入力情報を取得

	GamePad& gamePad = Input::Instance().GetGamePad();
	//float ax = gamePad.GetAxisLX();
	//float ay = gamePad.GetAxisLY();

	DirectX::XMFLOAT3 vec{};
	static DirectX::XMFLOAT3 angle = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	angle = Character::GetAngle();
	if (gamePad.GetButton() & GamePad::BTN_RIGHT) {
		angle.y -= 0.4f;
	}

	if (gamePad.GetButton() & GamePad::BTN_LEFT) {
		angle.y += 0.4f;
	}

	if (gamePad.GetButton() & GamePad::BTN_UP) {
		angle.x -= 0.2f;
	}
	if (gamePad.GetButton() & GamePad::BTN_DOWN) {
		angle.x += 0.2f;
	}

	vec.x = sinf(angle.y);
	vec.z = cosf(angle.y);
	vec.y = sinf(angle.x);

	return vec;
}

void Player::Moveturn(DirectX::XMFLOAT3 vec, float elapsedtime)
{
	angle.x += 3*vec.z*elapsedtime;
}



// 移動入力処理
bool Player::InputMove(float elapsedTime)
{
	// 進行ベクトル取得
	DirectX::XMFLOAT3 moveVec = GetMoveVec();
	if (moveVec.z == 0)
	{
		moveVec.x = 0;
	}
	// 移動処理
	Move(0, moveVec.z, moveSpeed);
	Moveturn(moveVec, elapsedTime);
	// 旋回処理
		Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed / 5);
	// 進行ベクトルがゼロベクトルでない場合は入力された
	return moveVec.x != 0.0f || moveVec.y != 0.0f || moveVec.z != 0.0f;
}

// ジャンプ入力処理
bool Player::InputJump()
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() & GamePad::BTN_A)
	{
		// ジャンプ回数制限
		if (jumpCount < jumpLimit)
		{
			// ジャンプ
			jumpCount++;
			Jump(jumpSpeed);

			// ジャンプ入力した
			return true;

		}
	}

	return false;
}

// 弾丸入力処理
bool Player::InputProjectile()
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	// 直進弾丸発射
	if (gamePad.GetButtonDown() & GamePad::BTN_X)
	{
		// 前方向
		DirectX::XMFLOAT3 dir;
		dir.x = sinf(angle.y);
		dir.y = 0.0f;
		dir.z = cosf(angle.y);
		// 発射位置（プレイヤーの腰あたり）
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + height * 0.5f;
		pos.z = position.z;
		// 発射
		ProjectileStraite* projectile = new ProjectileStraite(&projectileManager);
		projectile->Launch(dir, pos);

		// 発射入力した
		return true;
	}

	// 追尾弾丸発射
	if (gamePad.GetButtonDown() & GamePad::BTN_Y)
	{
		// 前方向
		DirectX::XMFLOAT3 dir;
		dir.x = sinf(angle.y);
		dir.y = 0.0f;
		dir.z = cosf(angle.y);

		// 発射位置（プレイヤーの腰あたり）
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + height * 0.5f;
		pos.z = position.z;

		// ターゲット（デフォルトではプレイヤーの前方）
		DirectX::XMFLOAT3 target;
		target.x = pos.x + dir.x * 1000.0f;
		target.y = pos.y + dir.y * 1000.0f;
		target.z = pos.z + dir.z * 1000.0f;

		// 一番近くの敵をターゲットにする
		float dist = FLT_MAX;
		EnemyManager& enemyManager = EnemyManager::Instance();
		int enemyCount = enemyManager.GetEnemyCount();
		for (int i = 0; i < enemyCount; ++i)
		{
			// 敵との距離判定
			Enemy* enemy = EnemyManager::Instance().GetEnemy(i);
			DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
			DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
			DirectX::XMVECTOR V = DirectX::XMVectorSubtract(E, P);
			DirectX::XMVECTOR D = DirectX::XMVector3LengthSq(V);
			float d;
			DirectX::XMStoreFloat(&d, D);
			if (d < dist)
			{
				dist = d;
				target = enemy->GetPosition();
				target.y += enemy->GetHeight() * 0.5f;
			}
		}

		// 発射
		ProjectileHoming* projectile = new ProjectileHoming(&projectileManager);
		projectile->Launch(dir, pos, target);

		// 発射入力した
		return true;
	}

	return false;
}

// 攻撃入力処理
bool Player::InputAttack()
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	if (gamePad.GetButtonDown() & GamePad::BTN_B)
	{
		return true;
	}

	return false;
}

// ジャンプ入力処理
bool Player::InputShoot()
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (::GetAsyncKeyState('P') & 1)
	//if (gamePad.GetButtonDown() & GamePad::BTN_UP)
	{
			return true;
	}
	return false;
}
// 移動入力処理
bool Player::InputShootMove(float elapsedTime)
{
	float shootMoveSpeed;
	GamePad& gamePad = Input::Instance().GetGamePad();
	// 前進速度
	if (gamePad.GetButton() & GamePad::BTN_A) 
		shootMoveSpeed = 2.0f;
	else 
		shootMoveSpeed = 0.0f;


	DirectX::XMFLOAT3 moveVec = DirectX::XMFLOAT3(0, 0, 0);
	// 射撃ベクトル取得
	moveVec = GetShootVec();

	// 移動処理
	Move(moveVec.x, moveVec.z, shootMoveSpeed);

	// XZ旋回処理
	//Turn(elapsedTime, vec.x, vec.z, turnSpeed);


	// XZY旋回処理
	TurnXYZ(elapsedTime, moveVec.x, moveVec.y , moveVec.z, turnSpeed);

	// 進行ベクトルがゼロベクトルでない場合は入力された
	return moveVec.x != 0.0f || moveVec.y != 0.0f || moveVec.z != 0.0f;
}



// プレイヤーとエネミーとの衝突処理
void Player::CollisionPlayerVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	// 全ての敵と総当たりで衝突処理
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		// 衝突処理
		DirectX::XMFLOAT3 outPosition;
		if (Collision::IntersectCylinderVsCylinder(
			position,
			radius,
			height,
			enemy->GetPosition(),
			enemy->GetRadius(),
			enemy->GetHeight(),
			outPosition))
		{
			// 敵の真上付近に当たったかを判定
			DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
			DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
			DirectX::XMVECTOR V = DirectX::XMVectorSubtract(P, E);
			DirectX::XMVECTOR N = DirectX::XMVector3Normalize(V);
			DirectX::XMFLOAT3 normal;
			DirectX::XMStoreFloat3(&normal, N);

			// 上から踏んづけた場合は小ジャンプする
			if (normal.y > 0.8f)
			{
				// 小ジャンプする
				Jump(jumpSpeed * 0.5f);

				// ダメージを与える
				enemy->ApplyDamage(1, 0.5f);
			}
			else
			{
				// 押し出し後の位置設定
				enemy->SetPosition(outPosition);
			}
		}
	}
}

// 弾丸と敵の衝突処理
void Player::CollisionProjectilesVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	// 全ての弾丸と全ての敵を総当たりで衝突処理
	int projectileCount = projectileManager.GetProjectileCount();
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < projectileCount; ++i)
	{
		Projectile* projectile = projectileManager.GetProjectile(i);

		for (int j = 0; j < enemyCount; ++j)
		{
			Enemy* enemy = enemyManager.GetEnemy(j);

			// 衝突処理
			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectSphereVsCylinder(
				projectile->GetPosition(),
				projectile->GetRadius(),
				enemy->GetPosition(),
				enemy->GetRadius(),
				enemy->GetHeight(),
				outPosition))
			{
				// ダメージを与える
				if (enemy->ApplyDamage(1, 0.5f))
				{
					// 吹き飛ばす
					{
						const float power = 10.0f;
						const DirectX::XMFLOAT3& e = enemy->GetPosition();
						const DirectX::XMFLOAT3& p = projectile->GetPosition();
						float vx = e.x - p.x;
						float vz = e.z - p.z;
						float lengthXZ = sqrtf(vx * vx + vz * vz);
						vx /= lengthXZ;
						vz /= lengthXZ;

						DirectX::XMFLOAT3 impulse;
						impulse.x = vx * power;
						impulse.y = power * 0.5f;
						impulse.z = vz * power;

						enemy->AddImpulse(impulse);
					}

					// ヒットエフェクト再生
					{
						DirectX::XMFLOAT3 e = enemy->GetPosition();
						e.y += enemy->GetHeight() * 0.5f;
						hitEffect->Play(e);
					}

					// 弾丸破棄
					projectile->Destroy();
				}
			}
		}
	}
}

// ノードと敵の衝突処理
void Player::CollisionNodeVsEnemies(const char* nodeName, float nodeRadius)
{
	// ノード取得
	Model::Node* node = model->FindNode(nodeName);

	// ノード位置取得
	DirectX::XMFLOAT3 nodePosition;
	nodePosition.x = node->worldTransform._41;
	nodePosition.y = node->worldTransform._42;
	nodePosition.z = node->worldTransform._43;

	// 指定のノードと全ての敵を総当たりで衝突処理
	EnemyManager& enemyManager = EnemyManager::Instance();
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		// 衝突処理
		DirectX::XMFLOAT3 outPosition;
		if (Collision::IntersectSphereVsCylinder(
			nodePosition,
			nodeRadius,
			enemy->GetPosition(),
			enemy->GetRadius(),
			enemy->GetHeight(),
			outPosition))
		{
			// ダメージを与える
			if (enemy->ApplyDamage(1, 0.5f))
			{
				// 吹っ飛ばす
				{
					const float power = 10.0f;
					const DirectX::XMFLOAT3& e = enemy->GetPosition();
					float vx = e.x - nodePosition.x;
					float vz = e.z - nodePosition.z;
					float lengthXZ = sqrtf(vx * vx + vz * vz);
					vx /= lengthXZ;
					vz /= lengthXZ;

					DirectX::XMFLOAT3 impulse;
					impulse.x = vx * power;
					impulse.y = power * 0.5f;
					impulse.z = vz * power;

					enemy->AddImpulse(impulse);
				}

				// ヒットエフェクト再生
				{
					DirectX::XMFLOAT3 e = enemy->GetPosition();
					e.y += enemy->GetHeight() * 0.5f;
					hitEffect->Play(e);
				}
			}
		}
	}
}

// 待機ステートへ遷移
void Player::TransitionIdleState()
{
	state = State::Idle;

	// 待機アニメーション再生
	model->PlayAnimation(Anim_Idle, true);
}

// 待機ステート更新処理
void Player::UpdateIdleState(float elapsedTime)
{
	// 移動入力処理
	if (InputMove(elapsedTime))
	{
		// 移動ステートへ遷移
		TransitionMoveState();
	}

	// ジャンプ入力処理
	if (InputJump())
	{
		// ジャンプステートへ遷移
		TransitionJumpState();
	}

	// 弾丸入力処理
	InputProjectile();

	// 攻撃入力処理
	if (InputAttack())
	{
		// 攻撃ステートへ遷移
		TransitionAttackState();
	}
	// ★射撃入力処理
	if (InputShoot())
	{
		// ★射撃ステートへ遷移
		TransitionShootState();
	}


}

// 移動ステートへ遷移
void Player::TransitionMoveState()
{
	state = State::Move;

	// 走りアニメーション再生
	model->PlayAnimation(Anim_Running, true);
}

// 移動ステート更新処理
void Player::UpdateMoveState(float elapsedTime)
{
	// 移動入力処理
	if (!InputMove(elapsedTime))
	{
		// 待機ステートへ遷移
		TransitionIdleState();
	}

	// ジャンプ入力処理
	if (InputJump())
	{
		// ジャンプステートへ遷移
		TransitionJumpState();
	}

	// 弾丸入力処理
	InputProjectile();

	// 攻撃入力処理
	if (InputAttack())
	{
		// 攻撃ステートへ遷移
		TransitionAttackState();
	}

	// ★射撃入力処理
	if (InputShoot())
	{
		// ★射撃ステートへ遷移
		TransitionShootState();
	}

}

void Player::UpdateShootState(float elapsedTime)
{
	//// 移動入力処理
	//InputMove(elapsedTime);

	// 射撃入力処理
	if (InputShootMove(elapsedTime))
	{
		// 待機ステートへ遷移
		//TransitionIdleState();
	}


	// ダメージアニメーションが終わって
	if (!model->IsPlayAnimation())
	{	//　Pキーで待機ステートへ遷移
		if (::GetAsyncKeyState('P') & 1) 
		{
			TransitionIdleState();
		}
	}

}



// ジャンプステートへ遷移
void Player::TransitionJumpState()
{
	state = State::Jump;

	// ジャンプアニメーション再生
	model->PlayAnimation(Anim_Jump, false);
}

// ジャンプステート更新処理
void Player::UpdateJumpState(float elapsedTime)
{
	// 移動入力処理
	InputMove(elapsedTime);

	// ジャンプ入力処理
	if (InputJump())
	{
		// ジャンプフリップアニメーション再生
		model->PlayAnimation(Anim_Jump_Flip, false);
	}
	// ジャンプアニメーション終了後
	if (!model->IsPlayAnimation())
	{
		// 落下アニメーション再生
		model->PlayAnimation(Anim_Falling, true);
	}

	// 弾丸入力処理
	InputProjectile();
}

// 着地ステートへ遷移
void Player::TransitionLandState()
{
	state = State::Land;

	// 着地アニメーション再生
	model->PlayAnimation(Anim_Landing, false);
}

// 着地ステート更新処理
void Player::UpdateLandState(float elapsedTime)
{
	// 着地アニメーション終了後
	if (!model->IsPlayAnimation())
	{
		// 待機ステートへ遷移
		TransitionIdleState();
	}
}

// 攻撃ステートへ遷移
void Player::TransitionAttackState()
{
	state = State::Attack;

	// 攻撃アニメーション再生
	model->PlayAnimation(Anim_Attack, false);
}

// 攻撃ステートへ遷移
void Player::TransitionShootState()
{
	state = State::Shoot;

	// 攻撃アニメーション再生
	model->PlayAnimation(Anim_GetHit2, false);
}


// 攻撃ステート更新処理
void Player::UpdateAttackState(float elapsedTime)
{
	// 攻撃アニメーション終了後
	if (!model->IsPlayAnimation())
	{
		// 待機ステートへ遷移
		TransitionIdleState();
	}

	// 任意のアニメーション再生区間でのみ衝突判定処理をする
	float animationTime = model->GetCurrentAnimationSeconds();
	attackCollisionFlag = animationTime >= 0.3f && animationTime <= 0.4f;
	if (attackCollisionFlag)
	{
		// 左手ノードとエネミーの衝突処理
		CollisionNodeVsEnemies("mixamorig:LeftHand", leftHandRadius);
	}
}

// ダメージステートへ遷移
void Player::TransitionDamageState()
{
	state = State::Damage;

	// ダメージアニメーション再生
	model->PlayAnimation(Anim_GetHit1, false);
}

// ダメージステート更新処理
void Player::UpdateDamageState(float elapsedTime)
{
	// ダメージアニメーションが終わったら待機ステートへ遷移
	if (!model->IsPlayAnimation())
	{
		TransitionIdleState();
	}
}

// 死亡ステートへ遷移
void Player::TransitionDeathState()
{
	state = State::Death;

	// 死亡アニメーション再生
	model->PlayAnimation(Anim_Death, false);
}

// 死亡ステート更新処理
void Player::UpdateDeathState(float elapsedTime)
{
	if (!model->IsPlayAnimation())
	{
		// ボタンを押したら復活ステートへ遷移
		GamePad& gamePad = Input::Instance().GetGamePad();
		if (gamePad.GetButtonDown() & GamePad::BTN_A)
		{
			TransitionReviveState();
		}
	}
}

// 復活ステートへ遷移
void Player::TransitionReviveState()
{
	state = State::Revive;

	// 体力回復
	health = maxHealth;

	// 復活アニメーション再生
	model->PlayAnimation(Anim_Revive, false);
}

// 復活ステート更新処理
void Player::UpdateReviveState(float elapsedTime)
{
	// 復活アニメーション終了後に待機ステートへ遷移
	if (!model->IsPlayAnimation())
	{
		TransitionIdleState();
	}
}

