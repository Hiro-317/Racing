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

#include "../../Object/Car/CarFactory/CarFactory.h"

#include "../../Object/Point/Point.h"
#include "../../Object/Point/Goal.h"


GameScene::GameScene(const int& num, const Input::KEYPAD_NO* no, const CAR_TYPE* type) :
	SceneBase(),
	focusFlg(false),
	time(0.0f),
	num(num),
	no(no),
	type(type)
{
#ifdef _DEBUG

	App::GetIns().SetDrawDebug(true);
#endif // _DEBUG
}

	void GameScene::SubPostLoad(void)
{
	//Snd::GetIns().ChangeScene("Game");
	AddActor(new Stage());

	cars = CarFactory(type, num, no, [&]() { return SceneBase::GetCamNo(); });

	for (int i = 0; i < (int)cars.size(); i++) {
		AddActor(cars.at(i));
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

void GameScene::SubPostRelease(void)
{
	for (int i = 0; i < (int)cars.size(); i++) {
		cars.at(i) = nullptr;
	}
	goalInsRef = nullptr;
}


void GameScene::CreateCamera(void)
{
	for (auto car : cars) {
		if (car == nullptr) return;
		camera.emplace_back(new FollowVecCamera(&car->GetTrans().pos, &car->GetMoveVec()));
	}
}
