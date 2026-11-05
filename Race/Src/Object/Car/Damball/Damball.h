#pragma once
#include "../CarBase/CarBase.h"


class Damball : public CarBase{
public:

	Damball(const Input::KEYPAD_NO& keypadNo, const Vector3& pos, const int pNo, const std::function<int(void)> camNo);
	~Damball() = default;

	void Load(void) override;

private:

	void SubInit(void) override;

	// タコメーターの最大スケール
	static constexpr float MAX_SCALE = 10000.0f;

	// レブリミット
	static constexpr float MAX_RPM = 8500.0f;
	// レッドゾーン
	static constexpr int RED_ZONE = 85;
	// 最高トルク(N-m)
	static constexpr float MAX_TORQUE = 540.0f;
	// 最高トルクを出す回転数
	static constexpr float MAX_TORQUE_RPM = 6500.0f;

	// 立ち上がり幅
	static constexpr float RISE_WIDTH = 3800.0f;
	// 下降幅
	static constexpr float FALL_WIDTH = 3750.0f;

	// 最高ギアの段数
	static constexpr int GEAR_NUM = 7;
	// それぞれのギア比
	static constexpr float GEAR_RATIO[GEAR_NUM] = {

		3.909f,
		2.438f,
		1.810f,
		1.458f,
		1.185f,
		0.967f,
		0.844f
	};
	// ファイナルギア比
	static constexpr float FINAL_GEAR_RATIO = 3.05f;

	// タイヤ半径
	static constexpr float TIRE_SIZE = 0.327f;

	// 車重
	static constexpr float CAR_WEIGHT = 1389.0f;
	// CD値
	static constexpr float CD = 0.38f;

	// 前面投影面積
	static constexpr float FPA = 1.95f;
	// 転がり抵抗係数
	static constexpr float RR = 0.011f;
};