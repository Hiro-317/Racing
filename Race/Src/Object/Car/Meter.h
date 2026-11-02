#pragma once
#include <functional>

#include "../../Common/Vector2.h"
#include "../../Application/Application.h"

#include "../ActorBase/ActorBase.h"


class Meter : public ActorBase {

public:
	
	Meter(const float& MAX_SCALE, const int& RED_ZONE, const float& speed, const float& rpm, const int& gear, const int& pNo, const std::function<int(void)>& camNo);
	~Meter() = default;

	void Load() override;

private:

	// タコメーターの最大スケール
	const float& MAX_SCALE;
	// タコメーターのレッドゾーン
	const int& RED_ZONE;

	// 現在の速度
	const float& speed;
	// 現在の回転数
	const float& rpm;
	// 現在のギア数
	const int& gear;
	
	// 描画カメラ
	const std::function<int(void)>& camNo;
	// 何Pか
	const int& pNo;
	
	void SubInit(void) override;
	void SubUpdate(void) override;
	void SubDraw(void) override;
	void SubRelease(void) override;

	//メーターのサイズ
	const int METER_SIZE = App::DEFAULT_SIZE_X_HALF;

	// 針の長さ
	static constexpr int NEEDLE_LENGTH = 100;
	// 数字のみ書く場所の広さ
	static constexpr int BLANK_LENGTH = 90;

	// タコメーターの位置
	const Vector2 TACHOMETER_POS = Vector2(App::GetIns().GetScreenSizeXHalf(), App::GetIns().GetScreenSizeY() - 100);

	// 針の0時の角度(ラジアン)
	const float NEEDLE_MIN_RAD = Deg2Rad(205.0f);
	// 針のスケール最大時の角度(ラジアン)
	const float NEEDLE_MAX_RAD = Deg2Rad(-25.0f);
	// 針移動の偏差
	const float SLOP;

	// 作ったメーターの保存
	int imageBack;
	int imageFront;

	// 針先の位置
	Vector2 needlePos;

	int time;
};