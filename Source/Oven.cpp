#include"Oven.h"
#include"Input/Input.h"
Oven::Oven()
{
	//���f�����Ȃ�������
	//model = new Model("Data/Model/");
}

void Oven::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	//�����͉��u��
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
	//�������ɂ͓���Ȃ�����angle���g���Ĕ��̑O���𔻒肷��悤�Ɍ�ŕς���
	float x, y, z;
	x = 10;
	y = 10;
	z = 10;
	//position.x+x��position.x-x�̊Ԃɂ��邩,position.y+y��position.y-y�̊Ԃɂ��邩,position.z+z��position.z-z�̊Ԃɂ��邩
	if (this->position.x + x > c->GetPosition().x && this->position.x - x<c->GetPosition().x &&
		this->position.y + y>c->GetPosition().y && this->position.y - y < c->GetPosition().y &&
		this->position.z + z>c->GetPosition().z && this->position.z - z < c->GetPosition().z)
	{
		return true;
	}
	return false;
}

