#include "ProjectileSauce.h"
#include "Effect.h"

// �R���X�g���N�^
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
	// �\���T�C�Y�𒲐�
	//scale.x = scale.y = scale.z = 0.5f;
	scale.x = scale.y = scale.z = 3.0f;
}

// �f�X�g���N�^
ProjectileSauce::~ProjectileSauce()
{
	Destroy();
	delete eff;
	delete model;
}

// �X�V����
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
		// ��������
		lifeTimer -= elapsedTime;
		if (lifeTimer <= 0.0f)
		{
			// �������폜
			Destroy();
		}

		// �ړ�
		
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
	
	// �I�u�W�F�N�g�s����X�V
	UpdateTransform();

	// ���f���s��X�V
	model->UpdateTransform(transform);
}

// �`�揈��
void ProjectileSauce::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model);
}

// ����
void ProjectileSauce::Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position)
{
	this->direction = direction;
	this->position = position;
}
