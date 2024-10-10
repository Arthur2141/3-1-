#pragma once
#include"StaticObject.h"
#include"Character.h"
//Manager�����ꍇoven���ڐ݂���
//TODO (TODO�ƃX�y�[�X)������ƃ^�X�N�ꗗ�ɕ\�������̂ł���Ăق������Ƃɂ͏�������
class Oven:public StaticObject 
{
public:
	Oven();
	~Oven()override 
	{
		if (oven != nullptr)
		{
			delete oven;
			oven = nullptr;
	}		
		if (character != nullptr)
		{
			delete character;
			character = nullptr;
	}
	};
	void Update(float elapsedTime)override;
	//���[�v�֐�
	void Warp(Character* c, Oven* o);
	//���[�v���W�ƃ��[�v�Ώێ擾�֐�
	void SetWarpPoint(Character* c, Oven* o);
	DirectX::XMFLOAT3 GetPosition() { return position; }
	//���[�v�͈͂ɓ����Ă邩���m�F����֐�
	bool WarpRange(Character*c);
private:
	Oven* oven=nullptr;
	Character* character = nullptr;
};