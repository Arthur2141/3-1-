#include"Oven.h"
#include"Input/Input.h"
Oven::Oven()
{
	//モデルがないから後で
	//model = new Model("Data/Model/");
}

void Oven::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	//条件は仮置き
	if (gamePad.GetButtonDown() & GamePad::BTN_UP&&WarpRange(character))
	{
		Warp(character, oven);
	}
	UpdateTransform();
}

void Oven::Warp(Character* c, Oven* o)
{
	c->SetPosition(o->GetPosition());
}

void Oven::SetWarpPoint(Character* c, Oven* o)
{
	oven = o;
	character = c;
}

bool Oven::WarpRange(Character*c)
{
	//多分中には入れないからangleを使って扉の前かを判定するように後で変える
	float x, y, z;
	x = 10;
	y = 10;
	z = 10;
	//position.x+xとposition.x-xの間にあるか,position.y+yとposition.y-yの間にあるか,position.z+zとposition.z-zの間にあるか
	if (this->position.x + x > c->GetPosition().x && this->position.x - x<c->GetPosition().x &&
		this->position.y + y>c->GetPosition().y && this->position.y - y < c->GetPosition().y &&
		this->position.z + z>c->GetPosition().z && this->position.z - z < c->GetPosition().z)
	{
		return true;
	}
	return false;
}

