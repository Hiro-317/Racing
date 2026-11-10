#pragma once

#include"../SceneBase.h"

#include "../../Manager/Input/InputManager.h"
#include "../../Object/Car/CarFactory/CarTypeDefine.h"


class CarBase;
class Goal;

class GameScene : public SceneBase
{
public:

	GameScene(const int& num, const Input::KEYPAD_NO* no, const CAR_TYPE* type);
	~GameScene()override = default;

private:

	// 読み込み
	void SubPostLoad(void)override;
	// 初期化（メイン処理の後）
	void SubPostInit(void)override;
	// 更新（前）
	void SubPreUpdate(void)override;
	// 更新（後）
	void SubPostUpdate(void)override;

	void SubPostRelease(void)override;

	void CreateCamera(void)override;

	bool UseSkyBox(void)const override { return true; }

	int DevideNum(void)const override { return num; }

	// カメラ状態変更フラグ
	bool focusFlg;

	// 時間計測
	float time;

	// マルチで時間をどのくらいの間隔で同期するか（秒数）
	const char MULTI_TIME_SEND_COUNT = 5;

	Goal* goalInsRef;
	std::vector<CarBase*> cars;

	const int& num;
	const Input::KEYPAD_NO* no;
	const CAR_TYPE* type;
};