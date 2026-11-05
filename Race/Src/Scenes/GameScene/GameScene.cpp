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

#include "../../Object/Car/Damball/Damball.h"
#include "../../Object/Car/Rally/Rally.h"
#include "../../Object/Point/Point.h"


GameScene::GameScene(const int& num, const Input::KEYPAD_NO* no) :
	SceneBase(),
	focusFlg(false),
	time(0.0f),
	num(num),
	no{ no[0], no[1], no[2], no[3] }
{
#ifdef _DEBUG

	App::GetIns().SetDrawDebug(true);
#endif // _DEBUG
}

	void GameScene::SubPostLoad(void)
{
	//Snd::GetIns().ChangeScene("Game");
	AddActor(new Stage());

	for (int i = 0; i < num; i++) {
		if (i % 2 == 0) {
			carInsRef[i] = new Damball(no[i], Vector3(200.0f * (i % 2), 0.0f, -200.0f * i), i, [&]() { return SceneBase::GetCamNo(); });
		}
		else {
			carInsRef[i] = new Rally(no[i], Vector3(200.0f * (i % 2), 0.0f, -200.0f * i), i, [&]() { return SceneBase::GetCamNo(); });
		}
		AddActor(carInsRef[i]);
	}

	goalInsRef = new Goal(1);
	AddActor(goalInsRef);
	AddActor(new Point(goalInsRef));
}

void GameScene::SubPostInit(void)
{
	time = 0;
}

void GameScene::SubPreUpdate(void)
{
	if (Input::GetIns().GetPriorityKey(COMMAND::PAUSE).keyTrgDown) {
		SceneManager::GetIns().ChangeSceneFade(SCENE_ID::Title);
	}
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
