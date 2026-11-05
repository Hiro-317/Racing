#pragma once
#include "../CarBase/CarBase.h"


class Rally : public CarBase{
public:

	Rally(const Input::KEYPAD_NO& keypadNo, const Vector3& pos, const int pNo, const std::function<int(void)> camNo);
	~Rally() = default;

	void Load(void) override;

private:
	
	void SubInit(void) override;

	// タコメーターの最大スケール
	static constexpr float TACO_MAX_SCALE = 10000.0f;

	// スピードメーターの最大スケール
	static constexpr float SPEED_MAX_SCALE = 280.0f;

	// レブリミット
	static constexpr float MAX_RPM = 7900.0f;
	// レッドゾーン
	static constexpr int RED_ZONE = 79;
	// 最高トルク(N-m)
	static constexpr float MAX_TORQUE = 362.8f;
	// 最高トルクを出す回転数
	static constexpr float MAX_TORQUE_RPM = 3200.0f;

	// 立ち上がり幅
	static constexpr float RISE_WIDTH = 2200.0f;
	// 下降幅
	static constexpr float FALL_WIDTH = 4650.0f;

	// 最高ギアの段数
	static constexpr int GEAR_NUM = 5;
	// それぞれのギア比
	static constexpr float GEAR_RATIO[GEAR_NUM] = {

		3.083f,
		2.062f,
		1.545f,
		1.151f,
		0.825f
	};
	// ファイナルギア比
	static constexpr float FINAL_GEAR_RATIO = 4.444f;

	// タイヤ半径
	static constexpr float TIRE_SIZE = 0.31f;

	// 車重
	static constexpr float CAR_WEIGHT = 1270.0f;
	// CD値
	static constexpr float CD = 0.35f;

	// 前面投影面積
	static constexpr float FPA = 2.46f;
	// 転がり抵抗係数
	static constexpr float RR = 0.011f;
};