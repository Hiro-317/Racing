#include "GameScene.h"

#include <cmath>

#include "../../Common/Utility.h"

#include "../../Application/Application.h"

//#include "../../Manager/Net/NetWorkManager.h"
#include "../../Manager/Camera/FollowVec/FollowVecCamera.h"
#include "../../Manager/Sound/SoundManager.h"
#include "../../Manager/Font/FontManager.h"

#include "../../Manager/Scene/SceneManager.h"

#include "../../Object/Stage/Stage.h"
#include "../../Object/Point/Point.h"


GameScene::GameScene(const int& num, const Input::KEYPAD_NO* no) :
	SceneBase(),
	focusFlg(false),
	time(0.0f),
	num(num),
	no{no[0], no[1], no[2], no[3]}
{
	App::GetIns().SetDrawDebug(true);
}

	void GameScene::SubPostLoad(void)
{
	DevideScreen(num);

	//Snd::GetIns().ChangeScene("Game");
	AddActor(new Stage());

	for (int i = 0; i < num; i++) {
		carInsRef[i] = new CarBase(no[i], Vector3());
		AddActor(carInsRef[i]);
	}

	AddActor(new Point());
}

void GameScene::SubPostInit(void)
{
	time = 0;
}

void GameScene::SubPreUpdate(void)
{
}

void GameScene::SubPostUpdate(void)
{
#pragma region ŽžŠÔŒv‘ª

	// ŽžŠÔXV
	time += App::GetPassTime();
#pragma endregion
}

void GameScene::SubUiDraw(void)
{
}

void GameScene::SubPostRelease(void)
{
	for (int i = 0; i < 3; i++) {
		carInsRef[i] = nullptr;
	}
}


void GameScene::CreateCamera(void)
{
	for (auto car : carInsRef) {
		if (car == nullptr) return;
		camera.emplace_back(new FollowVecCamera(&car->GetTrans().pos, &car->GetMoveVec()));
	}
}
