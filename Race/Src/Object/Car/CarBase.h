#pragma once
#include <algorithm>

#include "../ActorBase.h"
#include "../../Application/Application.h"

class CarBase : public ActorBase {
	
public:

	CarBase(void);
	~CarBase(void);

	void Load(void) override;
	void UiDraw(void) override;

	const Vector3& GetFocusPoint(void) { return focusPos; }

protected:

	void SubInit(void) override;
	void SubUpdate(void) override;
	void SubRelease(void) override;

	void AccelUpdate(void) override;

private:

	// 加速演算
	float Acceleration(float c = 1.0f);
	// エンブレ演算
	float EngineBrake(void);
	// 空気抵抗
	float AirResistance(void);
	// 転がり抵抗
	float RollingResistance(void);

	// 針の長さ
	static constexpr int NEEDLE_LENGTH = 140;
	// 数字のみ書く場所の広さ
	static constexpr int BLANK_LENGTH = 130;

	// アイドリング回転数
	static constexpr float MIN_RPM = 1000.0f;
	// レブリミット
	static constexpr float MAX_RPM = 7700.0f;
	// 空ぶかしで上がる回転数
	static constexpr float UP_RPM = 125.0f;
	// 空ぶかしで下がる回転数
	static constexpr float DOWN_RPM = 200.0f;

	// 最高トルク(N-m)
	static constexpr float MAX_TORQUE = 104.0f;
	// 最高トルクを出す回転数
	static constexpr float MAX_TORQUE_RPM = 2600.0f;
	// 最高馬力(kW)
	static constexpr float MAX_HORSEPOWER = 47.0f;
	// 最高馬力を出す回転数
	static constexpr float MAX_HORSEPOWER_RPM = 6000.0f;

	// 最高ギアの段数
	static constexpr int GEAR_NUM = 6;
	// それぞれのギア比
	static constexpr float GEAR_RATIO[GEAR_NUM]{
		3.571f,
		2.227f,
		1.529f,
		1.150f,
		0.869f,
		0.686f
	};
	// ファイナルギア比
	static constexpr float FINAL_GEAR_RATIO = 4.875f;

	// 駆動力損失率
	static constexpr float LOST_RATIO = 0.9f;

	// タイヤ半径
	static constexpr float TIRE_SIZE = 0.203f;

	// タコメーターの位置
	const Vector2 TACHOMETER_POS = Vector2(Application::SCREEN_SIZE_X_HALF, Application::SCREEN_SIZE_Y - 100);
	// タコメーターの最大スケール
	static constexpr float MAX_SCALE = 9000.0f;

	// 針の0時の角度(ラジアン)
	const float NEEDLE_MIN_RAD = Deg2Rad(205.0f);
	// 針のスケール最大時の角度(ラジアン)
	const float NEEDLE_MAX_RAD = Deg2Rad(-25.0f);
	// 針移動の偏差
	const float SLOP = (NEEDLE_MAX_RAD - NEEDLE_MIN_RAD) / MAX_SCALE;

	// トルクカーブ
	// 最高トルク回転数時に最高トルクがでて、最高馬力回転数時に最高馬力のでるような関数
	const float TORQUE_CURVE = logf((1000.0f * 60.0f * MAX_HORSEPOWER) / (2.0f * DX_PI_F * MAX_HORSEPOWER_RPM * MAX_TORQUE)) / (logf(MAX_HORSEPOWER_RPM / MAX_TORQUE_RPM) + 1 - MAX_HORSEPOWER_RPM / MAX_TORQUE_RPM);

	// 車重
	static constexpr float CAR_WEIGHT = 830.0f;
	// CD値
	static constexpr float CD = 0.28f;
	// 空気密度
	static constexpr float AIR_DENSITY = 1.20f;
	// 前面投影面積
	static constexpr float FPA = 1.74f;
	// 転がり抵抗係数
	static constexpr float RR = 0.011f;
	// エンジンブレーキ係数
	static constexpr float ENGINE_BRAKE = 0.01f;

	// 焦点距離
	const Vector3 FOCUS_POINT = Vector3::Zonly(400.0f);
	Vector3 focusPos;

	// エンジン稼働状態
	bool engineFlg;

	// 針先の位置
	Vector2 needlePos;

	// 回転数
	float rpm;

	// トルク
	float torque;
	// 実測値の最大トルク
	float torqueMax;
	// 実測値の最大トルク回転数
	float torqueMaxRpm;

	// 馬力
	float hoursepower;
	// 実測値の最大馬力
	float hoursepowerMax;
	// 実測値の最大馬力回転数
	float hoursepowerMaxRpm;

	// 速度
	float speed;
	// 最低速度
	float idleSpeed;

	// フューエルカット状態
	bool fuelCut;

	// 今のギア
	int nowGear;

	// ステア角度
	float nowSteer;
	float prevSteer;
};