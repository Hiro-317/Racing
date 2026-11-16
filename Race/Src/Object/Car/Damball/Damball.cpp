#include <vector>

#include "Damball.h"
#include "../Meter/DigitalMeter.h"


Damball::Damball(const Input::KEYPAD_NO& keypadNo, const Vector3& pos, const int pNo, const std::function<int(void)> camNo) :
	CarBase(keypadNo, pos, pNo, camNo)
{
	// レブリミット
	param.MAX_RPM = MAX_RPM;

	// 最高トルク(N-m)
	param.MAX_TORQUE = MAX_TORQUE;
	// 最高トルクを出す回転数
	param.MAX_TORQUE_RPM = MAX_TORQUE_RPM;

	// 立ち上がり幅
	param.RISE_WIDTH = RISE_WIDTH;
	// 下降幅
	param.FALL_WIDTH = FALL_WIDTH;

	// 最高ギアの段数
	param.GEAR_NUM = GEAR_NUM;
	// それぞれのギア比
	for (int i = 0; i < GEAR_NUM; i++) {
		param.GEAR_RATIO.emplace_back(GEAR_RATIO[i]);
	}
	// ファイナルギア比
	param.FINAL_GEAR_RATIO = FINAL_GEAR_RATIO;

	// タイヤ半径
	param.TIRE_SIZE = TIRE_SIZE;

	// 車重
	param.CAR_WEIGHT = CAR_WEIGHT;
	// CD値
	param.CD = CD;

	// 前面投影面積
	param.FPA = FPA;
	// 転がり抵抗係数
	param.RR = RR;

	// この車体のアイドリング時の最低速度
	idleSpeed = (MIN_RPM * 3.6f * 2.0f * DX_PI_F * param.TIRE_SIZE) / (param.GEAR_RATIO[0] * param.FINAL_GEAR_RATIO * 60.0f);
}

void Damball::Load(void) {
	trans.LoadModel("Car/Damball");
	CarBase::Load();
}

void Damball::SubInit(void)
{
	AddChildActor(new DigitalMeter(MAX_SCALE, RED_ZONE, speed, rpm, nowGear, pNo, camNo));
}
