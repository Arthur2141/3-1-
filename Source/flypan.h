#pragma once
#include"StaticObject.h"
#include"Character.h"
class Flypan :public StaticObject
{
public:
	Flypan();
	~Flypan()override
	{
		if (character != nullptr)
		{
			delete character;
			character = nullptr;
		}
	};
	void Update(float elapsedTime)override;
	//ƒ[ƒvŠÖ”
	bool Goal(Character* c);
private:
	Character* character = nullptr;
};