#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Character.h"
#include "ProjectileManager.h"
#include "Effect.h"

// �v���C���[
class Player : public Character
{
public:
	Player();
	~Player() override;

	// �C���X�^���X�擾
	static Player& Instance();

	// �X�V����
	void Update(float elapsedTime);

	// �`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	// �f�o�b�O�pGUI�`��
	void DrawDebugGUI();

	// �f�o�b�O�v���~�e�B�u�`��
	void DrawDebugPrimitive();

protected:
	// ���n�������ɌĂ΂��
	void OnLanding() override;

	// �_���[�W���󂯂����ɌĂ΂��
	void OnDamaged() override;

	// ���S�������ɌĂ΂��
	void OnDead() override;


private:
	// �X�e�B�b�N���͒l����ړ��x�N�g�����擾
	DirectX::XMFLOAT3 GetMoveVec() const;

	// �X�e�B�b�N���͒l����ˌ��x�N�g�����擾
	DirectX::XMFLOAT3 GetShootVec() const;

	// �ړ����͏���
	bool InputMove(float elapsedTime);
	// �ˌ��ړ����͏���
	bool InputShoot();

	// �W�����v���͏���
	bool InputJump();

	// �e�ۓ��͏���
	bool InputProjectile();

	// �U�����͏���
	bool InputAttack();

	// �ˌ����͏���
	bool InputShootMove(float elapsedTime);

	// 3D��]
	void TurnXYZ(float elapsedTime, float vx, float vy, float vz, float speed);



	// �v���C���[�ƃG�l�~�[�Ƃ̏Փˏ���
	void CollisionPlayerVsEnemies();

	// �e�ۂƓG�̏Փˏ���
	void CollisionProjectilesVsEnemies();

	// �m�[�h�ƃG�l�~�[�̏Փˏ���
	void CollisionNodeVsEnemies(const char* nodeName, float nodeRadius);

	// �ҋ@�X�e�[�g�֑J��
	void TransitionIdleState();

	// �ҋ@�X�e�[�g�X�V����
	void UpdateIdleState(float elapsedTime);

	// �ړ��X�e�[�g�֑J��
	void TransitionMoveState();

	// �ړ��X�e�[�g�X�V����
	void UpdateMoveState(float elapsedTime);

	// �W�����v�X�e�[�g�֑J��
	void TransitionJumpState();

	// �W�����v�X�e�[�g�X�V����
	void UpdateJumpState(float elapsedTime);

	// ���n�X�e�[�g�֑J��
	void TransitionLandState();

	// ���n�X�e�[�g�X�V����
	void UpdateLandState(float elapsedTime);

	// �U���X�e�[�g�֑J��
	void TransitionAttackState();

	// �ˌ��X�e�[�g�֑J��
	void TransitionShootState();
	// �ˌ��X�e�[�g�X�V����
	void UpdateShootState(float elapsedTime);



	// �U���X�e�[�g�X�V����
	void UpdateAttackState(float elapsedTime);

	// �_���[�W�X�e�[�g�֑J��
	void TransitionDamageState();

	// �_���[�W�X�e�[�g�X�V����
	void UpdateDamageState(float elapsedTime);

	// ���S�X�e�[�g�֑J��
	void TransitionDeathState();

	// ���S�X�e�[�g�X�V����
	void UpdateDeathState(float elapsedTime);

	// �����X�e�[�g�֑J��
	void TransitionReviveState();

	// �����X�e�[�g�X�V����
	void UpdateReviveState(float elapsedTime);




private:
	// �A�j���[�V����
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

	// �X�e�[�g
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
		// �ˌ����
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
