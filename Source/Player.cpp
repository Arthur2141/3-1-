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

// �C���X�^���X�擾
Player& Player::Instance()
{
	return *instance;
}

// �R���X�g���N�^
Player::Player()
{
	// �C���X�^���X�|�C���^�ݒ�
	instance = this;

	model = new Model("Data/Model/Jammo/Jammo.mdl");

	// ���f�����傫���̂ŃX�P�[�����O
	scale.x = scale.y = scale.z = 0.01f;

	// ����
	height = 1.5f;

	// �q�b�g�G�t�F�N�g�ǂݍ���
	hitEffect = new Effect("Data/Effect/Hit.efk");
	FlareEffect = new Effect("Data/Effect/Flare.efk");

	// �ҋ@�X�e�[�g�֑J��
	TransitionIdleState();
}

// �f�X�g���N�^
Player::~Player()
{
	delete hitEffect;

	delete model;
}

// �X�V����
void Player::Update(float elapsedTime)
{
	// �X�e�[�g���̏���
	switch (state)
	{
	case State::Idle:
		UpdateIdleState(elapsedTime);
		// �I�u�W�F�N�g�s����X�V
		UpdateTransform();

		break;

	case State::Move:
		UpdateMoveState(elapsedTime);
		// �I�u�W�F�N�g�s����X�V
		UpdateTransform();

		break;

	case State::Jump:
		UpdateJumpState(elapsedTime);
		// �I�u�W�F�N�g�s����X�V
		UpdateTransform();

		break;

	case State::Land:
		UpdateLandState(elapsedTime);
		// �I�u�W�F�N�g�s����X�V
		UpdateTransform();

		break;

	case State::Attack:
		UpdateAttackState(elapsedTime);
		// �I�u�W�F�N�g�s����X�V
		UpdateTransform();

		break;

	case State::Damage:
		UpdateDamageState(elapsedTime);
		// �I�u�W�F�N�g�s����X�V
		UpdateTransform();

		break;

	case State::Death:
		UpdateDeathState(elapsedTime);
		// �I�u�W�F�N�g�s����X�V
		UpdateTransform();

		break;

	case State::Revive:
		UpdateReviveState(elapsedTime);
		// �I�u�W�F�N�g�s����X�V
		UpdateTransform();

		break;

	case State::Shoot:
		UpdateShootState(elapsedTime);
		// �I�u�W�F�N�g�s����X�V
		UpdateTransform2();

		break;
	}

	// ���͍X�V����
	UpdateVelocity(elapsedTime);

	// ���G���ԍX�V
	UpdateInvincibleTimer(elapsedTime);

	// �e�ۍX�V����
	projectileManager.Update(elapsedTime);

	// �v���C���[�ƓG�Ƃ̏Փˏ���
	CollisionPlayerVsEnemies();

	// �e�ۂƓG�Ƃ̏Փˏ���
	CollisionProjectilesVsEnemies();

	//// �I�u�W�F�N�g�s����X�V
	//UpdateTransform();

	// ���f���A�j���[�V�����X�V����
	model->UpdateAnimation(elapsedTime);

	// ���f���s��X�V
	model->UpdateTransform(transform);
}

// �`�揈��
void Player::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model);

	// �e�ە`�揈��
	projectileManager.Render(dc, shader);
}

// �f�o�b�O�pGUI�`��
void Player::DrawDebugGUI()
{
	if (ImGui::CollapsingHeader("Player", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// �f�o�b�O�v���~�e�B�u�\��
		ImGui::Checkbox("VisibleDebugPrimitive", &visibleDebugPrimitive);
		// �X�e�[�g
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

		// �ʒu
		ImGui::InputFloat3("Position", &position.x);
		// ��]
		DirectX::XMFLOAT3 a;
		a.x = DirectX::XMConvertToDegrees(angle.x);
		a.y = DirectX::XMConvertToDegrees(angle.y);
		a.z = DirectX::XMConvertToDegrees(angle.z);
		ImGui::InputFloat3("Angle", &a.x);
		angle.x = DirectX::XMConvertToRadians(a.x);
		angle.y = DirectX::XMConvertToRadians(a.y);
		angle.z = DirectX::XMConvertToRadians(a.z);
		// �X�P�[��
		ImGui::InputFloat3("Scale", &scale.x);
		// �ړ����x
		ImGui::InputFloat("MoveSpeed", &moveSpeed);
		// ���񑬓x
		float turnDeg = DirectX::XMConvertToDegrees(turnSpeed);
		ImGui::InputFloat("TurnSpeed", &turnDeg);
		turnSpeed = DirectX::XMConvertToRadians(turnDeg);
		// ���a
		ImGui::InputFloat("Radius", &radius);
		// ����
		ImGui::InputFloat("Height", &height);
		// ���C
		ImGui::InputFloat("Friction", &friction);
		// �����x
		ImGui::InputFloat("Acceleration", &acceleration);
		// �󒆐���W��
		ImGui::InputFloat("AirControl", &airControl);
	}
}

// �f�o�b�O�v���~�e�B�u�`��
void Player::DrawDebugPrimitive()
{
	if (!visibleDebugPrimitive) return;

	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	// �Փ˔���p�̃f�o�b�O�~����`��
	debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));

	// �e�ۃf�o�b�O�v���~�e�B�u�`��
	projectileManager.DrawDebugPrimitive();

	// �U���Փ˗p�̍���m�[�h�̃f�o�b�O����`��
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

// ���n�������ɌĂ΂��
void Player::OnLanding()
{
	jumpCount = 0;

	// �_���[�W�A���S�X�e�[�g���͒��n�������ɃX�e�[�g�J�ڂ��Ȃ��悤�ɂ���
	if (state != State::Damage && state != State::Death)
	{
		// �������̑��͂����ȏ�Ȃ璅�n
		if (velocity.y < gravity * 5.0f)
		{
			// ���n�X�e�[�g�֑J��
			TransitionLandState();
		}
	}
}

// �_���[�W���󂯂����ɌĂ΂��
void Player::OnDamaged()
{
	// �_���[�W�X�e�[�g�֑J��
	TransitionDamageState();
}

// ���S�������ɌĂ΂��
void Player::OnDead()
{
	// ���S�X�e�[�g�֑J��
	TransitionDeathState();
}
// ���񏈗�
void Player::TurnXYZ(float elapsedTime, float vx, float vy, float vz, float speed)
{
	speed *= elapsedTime;

	// �[���x�N�g���̏ꍇ�͏�������K�v�Ȃ�
	float length = sqrtf(vx * vx + vz * vz);
	if (length < 0.001f) return;

	// �P�ʃx�N�g����
	vx /= length;
	vz /= length;

	// ���g�̉�]�l����O���������߂�
	float frontX = sinf(angle.y);
	float frontZ = cosf(angle.y);

	// ��]�p�����߂邽�߁A�Q�̒P�ʃx�N�g���̓��ς��v�Z����
	float dot = (frontX * vx) + (frontZ * vz);

	// ���ϒl��-1.0�`1.0�ŕ\������Ă���A�Q�̒P�ʃx�N�g���̊p�x��
	// �������ق�1.0�ɋ߂Â��Ƃ��������𗘗p���ĉ�]���x�𒲐�����
	float rot = 1.0f - dot;
	if (rot > speed) rot = speed;

	// ���E������s�����߂ɂQ�̒P�ʃx�N�g���̊O�ς��v�Z����
	float cross = (frontX * vz) - (frontZ * vx);

	// 2D�̊O�ϒl�����̏ꍇ�����̏ꍇ�ɂ���č��E���肪�s����
	// ���E������s�����Ƃɂ���č��E��]��I������
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



// �X�e�B�b�N���͒l����ړ��x�N�g�����擾
DirectX::XMFLOAT3 Player::GetMoveVec() const
{
	// ���͏����擾
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();

	// �J���������ƃX�e�B�b�N�̓��͒l�ɂ���Đi�s�������v�Z����
	Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
	const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

	// �ړ��x�N�g����XZ���ʂɐ����ȃx�N�g���Ȃ�悤�ɂ���

	// �J�����E�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
	if (cameraRightLength > 0.0f)
	{
		// �P�ʃx�N�g����
		cameraRightX /= cameraRightLength;
		cameraRightZ /= cameraRightLength;
	}

	// �J�����O�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
	float cameraFrontX = cameraFront.x;
	float cameraFrontZ = cameraFront.z;
	float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
	if (cameraFrontLength > 0.0f)
	{
		// �P�ʃx�N�g����
		cameraFrontX /= cameraFrontLength;
		cameraFrontZ /= cameraFrontLength;
	}

	// �X�e�B�b�N�̐������͒l���J�����E�����ɔ��f���A
	// �X�e�B�b�N�̐������͒l���J�����O�����ɔ��f���A
	// �i�s�x�N�g�����v�Z����
	DirectX::XMFLOAT3 vec;
	vec.x = (cameraRightX * ax) + (cameraFrontX * ay);
	vec.z = (cameraRightZ * ax) + (cameraFrontZ * ay);
	// Y�������ɂ͈ړ����Ȃ�
	vec.y = 0.0f;

	return vec;
}

// �o�C�I�n�U�[�h�ړ�
DirectX::XMFLOAT3 Player::GetShootVec() const
{
	// ���͏����擾

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



// �ړ����͏���
bool Player::InputMove(float elapsedTime)
{
	// �i�s�x�N�g���擾
	DirectX::XMFLOAT3 moveVec = GetMoveVec();
	if (moveVec.z == 0)
	{
		moveVec.x = 0;
	}
	// �ړ�����
	Move(0, moveVec.z, moveSpeed);
	Moveturn(moveVec, elapsedTime);
	// ���񏈗�
		Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed / 5);
	// �i�s�x�N�g�����[���x�N�g���łȂ��ꍇ�͓��͂��ꂽ
	return moveVec.x != 0.0f || moveVec.y != 0.0f || moveVec.z != 0.0f;
}

// �W�����v���͏���
bool Player::InputJump()
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() & GamePad::BTN_A)
	{
		// �W�����v�񐔐���
		if (jumpCount < jumpLimit)
		{
			// �W�����v
			jumpCount++;
			Jump(jumpSpeed);

			// �W�����v���͂���
			return true;

		}
	}

	return false;
}

// �e�ۓ��͏���
bool Player::InputProjectile()
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	// ���i�e�۔���
	if (gamePad.GetButtonDown() & GamePad::BTN_X)
	{
		// �O����
		DirectX::XMFLOAT3 dir;
		dir.x = sinf(angle.y);
		dir.y = 0.0f;
		dir.z = cosf(angle.y);
		// ���ˈʒu�i�v���C���[�̍�������j
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + height * 0.5f;
		pos.z = position.z;
		// ����
		ProjectileStraite* projectile = new ProjectileStraite(&projectileManager);
		projectile->Launch(dir, pos);

		// ���˓��͂���
		return true;
	}

	// �ǔ��e�۔���
	if (gamePad.GetButtonDown() & GamePad::BTN_Y)
	{
		// �O����
		DirectX::XMFLOAT3 dir;
		dir.x = sinf(angle.y);
		dir.y = 0.0f;
		dir.z = cosf(angle.y);

		// ���ˈʒu�i�v���C���[�̍�������j
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + height * 0.5f;
		pos.z = position.z;

		// �^�[�Q�b�g�i�f�t�H���g�ł̓v���C���[�̑O���j
		DirectX::XMFLOAT3 target;
		target.x = pos.x + dir.x * 1000.0f;
		target.y = pos.y + dir.y * 1000.0f;
		target.z = pos.z + dir.z * 1000.0f;

		// ��ԋ߂��̓G���^�[�Q�b�g�ɂ���
		float dist = FLT_MAX;
		EnemyManager& enemyManager = EnemyManager::Instance();
		int enemyCount = enemyManager.GetEnemyCount();
		for (int i = 0; i < enemyCount; ++i)
		{
			// �G�Ƃ̋�������
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

		// ����
		ProjectileHoming* projectile = new ProjectileHoming(&projectileManager);
		projectile->Launch(dir, pos, target);

		// ���˓��͂���
		return true;
	}

	return false;
}

// �U�����͏���
bool Player::InputAttack()
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	if (gamePad.GetButtonDown() & GamePad::BTN_B)
	{
		return true;
	}

	return false;
}

// �W�����v���͏���
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
// �ړ����͏���
bool Player::InputShootMove(float elapsedTime)
{
	float shootMoveSpeed;
	GamePad& gamePad = Input::Instance().GetGamePad();
	// �O�i���x
	if (gamePad.GetButton() & GamePad::BTN_A) 
		shootMoveSpeed = 2.0f;
	else 
		shootMoveSpeed = 0.0f;


	DirectX::XMFLOAT3 moveVec = DirectX::XMFLOAT3(0, 0, 0);
	// �ˌ��x�N�g���擾
	moveVec = GetShootVec();

	// �ړ�����
	Move(moveVec.x, moveVec.z, shootMoveSpeed);

	// XZ���񏈗�
	//Turn(elapsedTime, vec.x, vec.z, turnSpeed);


	// XZY���񏈗�
	TurnXYZ(elapsedTime, moveVec.x, moveVec.y , moveVec.z, turnSpeed);

	// �i�s�x�N�g�����[���x�N�g���łȂ��ꍇ�͓��͂��ꂽ
	return moveVec.x != 0.0f || moveVec.y != 0.0f || moveVec.z != 0.0f;
}



// �v���C���[�ƃG�l�~�[�Ƃ̏Փˏ���
void Player::CollisionPlayerVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	// �S�Ă̓G�Ƒ�������ŏՓˏ���
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		// �Փˏ���
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
			// �G�̐^��t�߂ɓ����������𔻒�
			DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
			DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
			DirectX::XMVECTOR V = DirectX::XMVectorSubtract(P, E);
			DirectX::XMVECTOR N = DirectX::XMVector3Normalize(V);
			DirectX::XMFLOAT3 normal;
			DirectX::XMStoreFloat3(&normal, N);

			// �ォ�瓥��Â����ꍇ�͏��W�����v����
			if (normal.y > 0.8f)
			{
				// ���W�����v����
				Jump(jumpSpeed * 0.5f);

				// �_���[�W��^����
				enemy->ApplyDamage(1, 0.5f);
			}
			else
			{
				// �����o����̈ʒu�ݒ�
				enemy->SetPosition(outPosition);
			}
		}
	}
}

// �e�ۂƓG�̏Փˏ���
void Player::CollisionProjectilesVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	// �S�Ă̒e�ۂƑS�Ă̓G�𑍓�����ŏՓˏ���
	int projectileCount = projectileManager.GetProjectileCount();
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < projectileCount; ++i)
	{
		Projectile* projectile = projectileManager.GetProjectile(i);

		for (int j = 0; j < enemyCount; ++j)
		{
			Enemy* enemy = enemyManager.GetEnemy(j);

			// �Փˏ���
			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectSphereVsCylinder(
				projectile->GetPosition(),
				projectile->GetRadius(),
				enemy->GetPosition(),
				enemy->GetRadius(),
				enemy->GetHeight(),
				outPosition))
			{
				// �_���[�W��^����
				if (enemy->ApplyDamage(1, 0.5f))
				{
					// ������΂�
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

					// �q�b�g�G�t�F�N�g�Đ�
					{
						DirectX::XMFLOAT3 e = enemy->GetPosition();
						e.y += enemy->GetHeight() * 0.5f;
						hitEffect->Play(e);
					}

					// �e�۔j��
					projectile->Destroy();
				}
			}
		}
	}
}

// �m�[�h�ƓG�̏Փˏ���
void Player::CollisionNodeVsEnemies(const char* nodeName, float nodeRadius)
{
	// �m�[�h�擾
	Model::Node* node = model->FindNode(nodeName);

	// �m�[�h�ʒu�擾
	DirectX::XMFLOAT3 nodePosition;
	nodePosition.x = node->worldTransform._41;
	nodePosition.y = node->worldTransform._42;
	nodePosition.z = node->worldTransform._43;

	// �w��̃m�[�h�ƑS�Ă̓G�𑍓�����ŏՓˏ���
	EnemyManager& enemyManager = EnemyManager::Instance();
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		// �Փˏ���
		DirectX::XMFLOAT3 outPosition;
		if (Collision::IntersectSphereVsCylinder(
			nodePosition,
			nodeRadius,
			enemy->GetPosition(),
			enemy->GetRadius(),
			enemy->GetHeight(),
			outPosition))
		{
			// �_���[�W��^����
			if (enemy->ApplyDamage(1, 0.5f))
			{
				// ������΂�
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

				// �q�b�g�G�t�F�N�g�Đ�
				{
					DirectX::XMFLOAT3 e = enemy->GetPosition();
					e.y += enemy->GetHeight() * 0.5f;
					hitEffect->Play(e);
				}
			}
		}
	}
}

// �ҋ@�X�e�[�g�֑J��
void Player::TransitionIdleState()
{
	state = State::Idle;

	// �ҋ@�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Idle, true);
}

// �ҋ@�X�e�[�g�X�V����
void Player::UpdateIdleState(float elapsedTime)
{
	// �ړ����͏���
	if (InputMove(elapsedTime))
	{
		// �ړ��X�e�[�g�֑J��
		TransitionMoveState();
	}

	// �W�����v���͏���
	if (InputJump())
	{
		// �W�����v�X�e�[�g�֑J��
		TransitionJumpState();
	}

	// �e�ۓ��͏���
	InputProjectile();

	// �U�����͏���
	if (InputAttack())
	{
		// �U���X�e�[�g�֑J��
		TransitionAttackState();
	}
	// ���ˌ����͏���
	if (InputShoot())
	{
		// ���ˌ��X�e�[�g�֑J��
		TransitionShootState();
	}


}

// �ړ��X�e�[�g�֑J��
void Player::TransitionMoveState()
{
	state = State::Move;

	// ����A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Running, true);
}

// �ړ��X�e�[�g�X�V����
void Player::UpdateMoveState(float elapsedTime)
{
	// �ړ����͏���
	if (!InputMove(elapsedTime))
	{
		// �ҋ@�X�e�[�g�֑J��
		TransitionIdleState();
	}

	// �W�����v���͏���
	if (InputJump())
	{
		// �W�����v�X�e�[�g�֑J��
		TransitionJumpState();
	}

	// �e�ۓ��͏���
	InputProjectile();

	// �U�����͏���
	if (InputAttack())
	{
		// �U���X�e�[�g�֑J��
		TransitionAttackState();
	}

	// ���ˌ����͏���
	if (InputShoot())
	{
		// ���ˌ��X�e�[�g�֑J��
		TransitionShootState();
	}

}

void Player::UpdateShootState(float elapsedTime)
{
	//// �ړ����͏���
	//InputMove(elapsedTime);

	// �ˌ����͏���
	if (InputShootMove(elapsedTime))
	{
		// �ҋ@�X�e�[�g�֑J��
		//TransitionIdleState();
	}


	// �_���[�W�A�j���[�V�������I�����
	if (!model->IsPlayAnimation())
	{	//�@P�L�[�őҋ@�X�e�[�g�֑J��
		if (::GetAsyncKeyState('P') & 1) 
		{
			TransitionIdleState();
		}
	}

}



// �W�����v�X�e�[�g�֑J��
void Player::TransitionJumpState()
{
	state = State::Jump;

	// �W�����v�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Jump, false);
}

// �W�����v�X�e�[�g�X�V����
void Player::UpdateJumpState(float elapsedTime)
{
	// �ړ����͏���
	InputMove(elapsedTime);

	// �W�����v���͏���
	if (InputJump())
	{
		// �W�����v�t���b�v�A�j���[�V�����Đ�
		model->PlayAnimation(Anim_Jump_Flip, false);
	}
	// �W�����v�A�j���[�V�����I����
	if (!model->IsPlayAnimation())
	{
		// �����A�j���[�V�����Đ�
		model->PlayAnimation(Anim_Falling, true);
	}

	// �e�ۓ��͏���
	InputProjectile();
}

// ���n�X�e�[�g�֑J��
void Player::TransitionLandState()
{
	state = State::Land;

	// ���n�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Landing, false);
}

// ���n�X�e�[�g�X�V����
void Player::UpdateLandState(float elapsedTime)
{
	// ���n�A�j���[�V�����I����
	if (!model->IsPlayAnimation())
	{
		// �ҋ@�X�e�[�g�֑J��
		TransitionIdleState();
	}
}

// �U���X�e�[�g�֑J��
void Player::TransitionAttackState()
{
	state = State::Attack;

	// �U���A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Attack, false);
}

// �U���X�e�[�g�֑J��
void Player::TransitionShootState()
{
	state = State::Shoot;

	// �U���A�j���[�V�����Đ�
	model->PlayAnimation(Anim_GetHit2, false);
}


// �U���X�e�[�g�X�V����
void Player::UpdateAttackState(float elapsedTime)
{
	// �U���A�j���[�V�����I����
	if (!model->IsPlayAnimation())
	{
		// �ҋ@�X�e�[�g�֑J��
		TransitionIdleState();
	}

	// �C�ӂ̃A�j���[�V�����Đ���Ԃł̂ݏՓ˔��菈��������
	float animationTime = model->GetCurrentAnimationSeconds();
	attackCollisionFlag = animationTime >= 0.3f && animationTime <= 0.4f;
	if (attackCollisionFlag)
	{
		// ����m�[�h�ƃG�l�~�[�̏Փˏ���
		CollisionNodeVsEnemies("mixamorig:LeftHand", leftHandRadius);
	}
}

// �_���[�W�X�e�[�g�֑J��
void Player::TransitionDamageState()
{
	state = State::Damage;

	// �_���[�W�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_GetHit1, false);
}

// �_���[�W�X�e�[�g�X�V����
void Player::UpdateDamageState(float elapsedTime)
{
	// �_���[�W�A�j���[�V�������I�������ҋ@�X�e�[�g�֑J��
	if (!model->IsPlayAnimation())
	{
		TransitionIdleState();
	}
}

// ���S�X�e�[�g�֑J��
void Player::TransitionDeathState()
{
	state = State::Death;

	// ���S�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Death, false);
}

// ���S�X�e�[�g�X�V����
void Player::UpdateDeathState(float elapsedTime)
{
	if (!model->IsPlayAnimation())
	{
		// �{�^�����������畜���X�e�[�g�֑J��
		GamePad& gamePad = Input::Instance().GetGamePad();
		if (gamePad.GetButtonDown() & GamePad::BTN_A)
		{
			TransitionReviveState();
		}
	}
}

// �����X�e�[�g�֑J��
void Player::TransitionReviveState()
{
	state = State::Revive;

	// �̗͉�
	health = maxHealth;

	// �����A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Revive, false);
}

// �����X�e�[�g�X�V����
void Player::UpdateReviveState(float elapsedTime)
{
	// �����A�j���[�V�����I����ɑҋ@�X�e�[�g�֑J��
	if (!model->IsPlayAnimation())
	{
		TransitionIdleState();
	}
}

