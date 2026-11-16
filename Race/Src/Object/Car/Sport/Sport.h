#pragma once
#include "../CarBase/CarBase.h"


class Sport : public CarBase{
public:

	Sport(const Input::KEYPAD_NO& keypadNo, const Vector3& pos, const int pNo, const std::function<int(void)> camNo);
	~Sport() = default;

	void Load(void) override;

private:
	
	void SubInit(void) override;

	// タコメーターの最大スケール
	static constexpr float TACO_MAX_SCALE = 9000.0f;

	// レブリミット
	static constexpr float MAX_RPM = 7500.0f;
	// レッドゾーン
	static constexpr int RED_ZONE = 75;
	// 最高トルク(N-m)
	static constexpr float MAX_TORQUE = 600.0f;
	// 最高トルクを出す回転数
	static constexpr float MAX_TORQUE_RPM = 6000.0f;

	// 立ち上がり幅
	static constexpr float RISE_WIDTH = 5250.0f;
	// 下降幅
	static constexpr float FALL_WIDTH = 2000.0f;

	// 最高ギアの段数
	static constexpr int GEAR_NUM = 9;
	// それぞれのギア比
	static constexpr float GEAR_RATIO[GEAR_NUM] = {

		3.838f,
		2.433f,
		1.777f,
		1.427f,
		1.211f,
		1.038f,
		0.88f,
		0.747f,
		0.633f,
	};
	// ファイナルギア比
	static constexpr float FINAL_GEAR_RATIO = 3.583f;

	// タイヤ半径
	static constexpr float TIRE_SIZE = 0.31f;

	// 車重
	static constexpr float CAR_WEIGHT = 1790.0f;
	// CD値
	static constexpr float CD = 0.35f;

	// 前面投影面積
	static constexpr float FPA = 2.46f;
	// 転がり抵抗係数
	static constexpr float RR = 0.011f;
};