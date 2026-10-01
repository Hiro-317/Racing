#include <DxLib.h>
#include "Mouse.h"
#include "../../../Application/Application.h"


void Mouse::Init(void)
{
	anyoneTrgDown = anyoneTrg = anyone = false;

	// マウスを中央に
	SetMousePoint(Application::SCREEN_SIZE_X_HALF, Application::SCREEN_SIZE_Y_HALF);
}

void Mouse::Update(void)
{
	// 初期化
	anyoneTrgDown = anyoneTrg = anyone = false;
	mouseInput = GetMouseInput();

	// キー検知
	for (auto& p : infos)
	{
		p.second.keyOld = p.second.keyNew;
		p.second.keyNew = CheckMouse(p.second.key);
		InputBase::Update(p);
	}

	// マウス検知
	if (mouseLock) {
		int mouseX = 0;
		int mouseY = 0;
		GetMousePoint(&mouseX, &mouseY);
		mousePos.x = static_cast<float>(mouseX);
		mousePos.y = static_cast<float>(mouseY);

		if (!anyone && mousePos.x != Application::SCREEN_SIZE_X_HALF && mousePos.y != Application::SCREEN_SIZE_Y_HALF) {

			anyone = true;
		}

		// マウスを中央に
		SetMousePoint(Application::SCREEN_SIZE_X_HALF, Application::SCREEN_SIZE_Y_HALF);
	}
}

void Mouse::Release(void)
{
}

bool Mouse::CheckMouse(int key)
{
	if (mouseInput & key) {

		return true;
	}
	return false;
}

Mouse::Mouse(void) : mousePos()
{
	mouseInput = -1;
}
