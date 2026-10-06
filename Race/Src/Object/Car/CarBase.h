#pragma once
#include <algorithm>

#include "../ActorBase.h"
#include "../../Application/Application.h"

class CarBase : public ActorBase {
	
public:

	CarBase(void);
	~CarBase(void);

	void Load(void) override;

protected:

	void SubUpdate(void) override;
	void SubDraw(void) override;
	void SubRelease(void) override;

	void AccelUpdate(void) override;

private:

	static constexpr int NEEDLE_LENGTH = 140;
	static constexpr int BLANK_LENGTH = 130;

	static constexpr float MIN_RPM = 1000.0f;
	static constexpr float MAX_RPM = 7700.0f;
	static constexpr float REV_RPM = 250.0f;
	static constexpr float UP_RPM = 125.0f;
	static constexpr float DOWN_RPM = 200.0f;

	static constexpr float MAX_TORQUE = 104.0f;
	static constexpr float MAX_TORQUE_RPM = 2600.0f;
	static constexpr float MAX_HORSEPOWER = 47.0f;
	static constexpr float MAX_HORSEPOWER_RPM = 6000.0f;

	static constexpr int GEAR_NUM = 6;

	static constexpr float GEAR_RATIO[GEAR_NUM]{
		3.571f,
		2.227f,
		1.529f,
		1.150f,
		0.869f,
		0.686f
	};

	static constexpr float FINAL_GEAR_RATIO = 4.875f;
	static constexpr float LOST_RATIO = 0.9f;

	static constexpr float TIRE_SIZE_ROUND = 0.203f;

	static constexpr int DEFAULT_ENGINE_FREQUENCY = 48000;

	const Vector2 TACHOMETER_POS = Vector2(Application::SCREEN_SIZE_X_HALF, Application::SCREEN_SIZE_Y - 200);
	static constexpr float MAX_SCALE = 9000.0f;

	const float NEEDLE_MIN_RAD = Deg2Rad(205.0f);
	const float NEEDLE_MAX_RAD = Deg2Rad(-25.0f);

	const float SLOP = (NEEDLE_MAX_RAD - NEEDLE_MIN_RAD) / MAX_SCALE;

	const float TORQUE_CURVE = logf((9950.0f * MAX_HORSEPOWER) / (MAX_HORSEPOWER_RPM * MAX_TORQUE)) / (logf(MAX_HORSEPOWER_RPM / MAX_TORQUE_RPM) + 1 - MAX_HORSEPOWER_RPM / MAX_TORQUE_RPM);

	static constexpr float CAR_WEIGHT = 830.0f;
	static constexpr float CD = 0.28f;
	static constexpr float AIR_DENSITY = 1.20f;
	static constexpr float FPA = 1.74f;
	static constexpr float RR = 0.011f;
	static constexpr float ENGINE_BRAKE = 0.01f;

	float rpm;
	float prevRpm;

	bool engineFlg;

	Vector2 needlePos;

	float torque;
	float torqueMax;
	float torqueMaxRpm;
	float hoursepower;
	float hoursepowerMax;
	float hoursepowerMaxRpm;
	float speed;
	float idleSpeed;

	bool fuelCut;

	int nowGear;
};