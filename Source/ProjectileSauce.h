#pragma once

#include "Graphics/Model.h"
#include "Projectile.h"

enum Sauce
{
	Mustard,
	Tomato,
	Ketchup
};

enum SauceState
{
	Flying,
	Stuck
};

// ���i�e��
class ProjectileSauce : public Projectile
{
public:
	ProjectileSauce(ProjectileManager* manager);
	~ProjectileSauce() override;

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	// ����
	void Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position);

private:
	Model* model = nullptr;
	float		speed = 10.0f;
	float		lifeTimer = 3.0f;
	Sauce sauceType;
	SauceState s_state=SauceState::Flying;
};
