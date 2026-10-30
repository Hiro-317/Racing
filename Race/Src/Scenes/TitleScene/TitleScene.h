#pragma once

#include"../SceneBase.h"

#include "../../Manager/Input/InputManager.h"


class TitleScene : public SceneBase
{
public:
	TitleScene();
	~TitleScene()override = default;

private:

	// 読み込み
	void SubPostLoad(void)override;
	// 初期化（メイン処理の後）
	void SubPostInit(void)override;
	// 更新（後）
	void SubPostUpdate(void)override;

	void SubUiDraw(void)override;

	void SubPostRelease(void)override;

	bool UseSkyBox(void)const override { return false; }

	// カメラ状態変更フラグ
	bool focusFlg;

	// 時間計測
	float time;

	// マルチで時間をどのくらいの間隔で同期するか（秒数）
	const char MULTI_TIME_SEND_COUNT = 5;

	// 人数
	int num;

	bool flg;
	bool Flg;

	Input::KEYPAD_NO tmpNo[4];
	int tmpNum;
};