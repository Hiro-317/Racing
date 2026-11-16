#pragma once
#include "../CarBase/CarBase.h"


class Kanjo : public CarBase{
public:

	Kanjo(const Input::KEYPAD_NO& keypadNo, const Vector3& pos, const int pNo, const std::function<int(void)> camNo);
	~Kanjo() = default;

	void Load(void) override;

private:
	
	void SubInit(void) override;

	// タコメーターの最大スケール
	static constexpr float TACO_MAX_SCALE = 8000.0f;

	// スピードメーターの最大スケール
	static constexpr float SPEED_MAX_SCALE = 320.0f;
	// スピードメーターの数値刻み(*10)
	static constexpr int EVERY_SPEED = 4;

	// レブリミット
	static constexpr float MAX_RPM = 7000.0f;
	// レッドゾーン
	static constexpr int RED_ZONE = 70;
	// 最高トルク(N-m)
	static constexpr float MAX_TORQUE = 420.0f;
	// 最高トルクを出す回転数
	static constexpr float MAX_TORQUE_RPM = 4000.0f;

	// 立ち上がり幅
	static constexpr float RISE_WIDTH = 3000.0f;
	// 下降幅
	static constexpr float FALL_WIDTH = 4000.0f;

	// 最高ギアの段数
	static constexpr int GEAR_NUM = 6;
	// それぞれのギア比
	static constexpr float GEAR_RATIO[GEAR_NUM] = {

		3.625f,
		2.115f,
		1.529f,
		1.125f,
		0.911f,
		0.734f,
	};
	// ファイナルギア比
	static constexpr float FINAL_GEAR_RATIO = 3.842f;

	// タイヤ半径
	static constexpr float TIRE_SIZE = 0.31f;

	// 車重
	static constexpr float CAR_WEIGHT = 1430.0f;
	// CD値
	static constexpr float CD = 0.35f;

	// 前面投影面積
	static constexpr float FPA = 2.46f;
	// 転がり抵抗係数
	static constexpr float RR = 0.011f;
};