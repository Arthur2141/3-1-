#include "ProjectileSauce.h"
#include "Effect.h"

// コンストラクタ
ProjectileSauce::ProjectileSauce(ProjectileManager* manager)
	: Projectile(manager)
{
	//model = new Model("Data/Model/SpikeBall/SpikeBall.mdl");
	switch (sauceType)
	{
	case Mustard:
		break;
	case Tomato:
		break;
	case Ketchup:
		break;
	default:
		break;
	}
	model = new Model("Data/Model/Sword/Sword.mdl");
	eff = new Effect("Data/Effect/Hit.efk");
	targetpos = nullptr;
	// 表示サイズを調整
	//scale.x = scale.y = scale.z = 0.5f;
	scale.x = scale.y = scale.z = 3.0f;
}

// デストラクタ
ProjectileSauce::~ProjectileSauce()
{
	Destroy();
	delete eff;
	delete model;
}

// 更新処理
void ProjectileSauce::Update(float elapsedTime)
{
	float speed = this->speed * elapsedTime;
	if (state > 0)
	{
		s_state = Stuck;
	}
	switch (s_state)
	{
	case Flying:
		// 寿命処理
		lifeTimer -= elapsedTime;
		if (lifeTimer <= 0.0f)
		{
			// 自分を削除
			Destroy();
		}

		// 移動
		
		position.x += direction.x * speed;
		position.y += direction.y * speed;
		position.z += direction.z * speed;
		break;
	case Stuck:
		position.x = targetpos->x+offset.x;
		position.y = targetpos->y-offset.y;
		position.z = targetpos->z+offset.z;
		break;
	default:
		break;
	}
	
	// オブジェクト行列を更新
	UpdateTransform();

	// モデル行列更新
	model->UpdateTransform(transform);
}

// 描画処理
void ProjectileSauce::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model);
}

// 発射
void ProjectileSauce::Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position)
{
	this->direction = direction;
	this->position = position;
}
