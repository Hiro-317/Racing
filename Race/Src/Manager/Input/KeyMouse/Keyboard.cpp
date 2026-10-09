#include <DxLib.h>
#include "Keyboard.h"

void Keyboard::Init(void)
{
	anyoneTrgDown = anyoneTrg = anyone = false;
	Add(KEY_INPUT_W);
	Add(KEY_INPUT_A);
	Add(KEY_INPUT_S);
	Add(KEY_INPUT_D);
}

void Keyboard::Update(void)
{
	// 初期化
	anyoneTrgDown = anyoneTrg = anyone = false; 

	// キーボード検知
	for (auto& p : infos)
	{
		p.second.keyOld = p.second.keyNew;
		p.second.keyNew = CheckHitKey(p.second.key);
		InputBase::Update(p);
	}
}

void Keyboard::Release(void)
{
}

Keyboard::Keyboard(void)
{
}
