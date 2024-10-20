#include"flypan.h"

Flypan::Flypan()
{
	model = new Model("Data/Model/Flypan/flypans.mdl");
}

void Flypan::Update(float elapsedTime)
{
	UpdateTransform();
}

bool Flypan::Goal(Character* c)
{
	float x, y, z;
	x = 7;
	y = 5;
	z = 7;
	//position.x+xとposition.x-xの間にあるか,position.y+yとposition.y-yの間にあるか,position.z+zとposition.z-zの間にあるか
	if ((this->position.x + x)*scale.x > c->GetPosition().x*c->GetScale().x && (this->position.x - x) * scale.x<c->GetPosition().x * c->GetScale().x &&
		(this->position.y+2) * scale.y>c->GetPosition().y * c->GetScale().y && (this->position.y - y) * scale.y < c->GetPosition().y * c->GetScale().y &&
		(this->position.z + z) * scale.z>c->GetPosition().z * c->GetScale().z && (this->position.z - z) * scale.z < c->GetPosition().z * c->GetScale().z)
	{
		return true;
	}
	return false;
}
