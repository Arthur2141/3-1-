#include"flypan.h"
#include "Player.h"
#include <DirectXMath.h>
#include"Collision.h"


Flypan::Flypan()
{
	model = new Model("Data/Model/Flypan/flypans.mdl");
}

void Flypan::Update(float elapsedTime)
{
	UpdateTransform();
}

bool Flypan::Goal(Player* c)
{
	float x, y, z;
	x = 7;
	y = 5;
	z = 7;
	//position.x+x‚Æposition.x-x‚ÌŠÔ‚É‚ ‚é‚©,position.y+y‚Æposition.y-y‚ÌŠÔ‚É‚ ‚é‚©,position.z+z‚Æposition.z-z‚ÌŠÔ‚É‚ ‚é‚©
	/*if ((this->position.x + x)*scale.x > c->GetPosition().x*c->GetScale().x && (this->position.x - x) * scale.x<c->GetPosition().x * c->GetScale().x &&
		(this->position.y+2) * scale.y>c->GetPosition().y * c->GetScale().y && (this->position.y - y) * scale.y < c->GetPosition().y * c->GetScale().y &&
		(this->position.z + z) * scale.z>c->GetPosition().z * c->GetScale().z && (this->position.z - z) * scale.z < c->GetPosition().z * c->GetScale().z)
	{
		return true;
	}*/
	DirectX::XMFLOAT3 s, e;
	HitResult result;
	s = c->GetPosition();
	//s.y += 0.5f;
	DirectX::XMStoreFloat3(&e, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&c->GetPosition()), DirectX::XMLoadFloat3(&c->GetVelocity())));

	if (Collision::IntersectRayVsModel(s, e, this->model, result))
	{
		return true;
	}
	return false;
}
