#pragma once
#include"StaticObject.h"
#include"Character.h"
//Managerを作る場合ovenを移設する
//TODO (TODOとスペース)を入れるとタスク一覧に表示されるのでやってほしいことには書くこと
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
	//ワープ関数
	void Warp(Character* c, Oven* o);
	//ワープ座標とワープ対象取得関数
	void SetWarpPoint(Character* c, Oven* o);
	DirectX::XMFLOAT3 GetPosition() { return position; }
	//ワープ範囲に入ってるかを確認する関数
	bool WarpRange(Character*c);
private:
	Oven* oven=nullptr;
	Character* character = nullptr;
};