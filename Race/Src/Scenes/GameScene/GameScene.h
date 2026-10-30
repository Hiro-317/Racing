#pragma once

#include"../SceneBase.h"

#include "../../Object/Car/CarBase.h"
#include "../../Manager/Input/InputManager.h"


class GameScene : public SceneBase
{
public:

	GameScene(const int& num, const Input::KEYPAD_NO* no);
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

	void SubUiDraw(void)override;

	void SubPostRelease(void)override;

	void CreateCamera(void)override;

	bool UseSkyBox(void)const override { return true; }

	// カメラ状態変更フラグ
	bool focusFlg;

	// 時間計測
	float time;

	// マルチで時間をどのくらいの間隔で同期するか（秒数）
	const char MULTI_TIME_SEND_COUNT = 5;

	CarBase* carInsRef[4];

	const int& num;
	const Input::KEYPAD_NO no[4];
};