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

	static constexpr int NEEDLE_LENGTH = 200;
	static constexpr float MIN_RPM = 1000.0f;
	static constexpr float MAX_RPM = 7700.0f;
	static constexpr float REV_RPM = 250.0f;
	static constexpr float UP_RPM = 80.0f;
	static constexpr float DOWN_RPM = 100.0f;

	static constexpr float MAX_TORQUE = 104.0f;
	static constexpr float MAX_TORQUE_RPM = 2600.0f;
	static constexpr float MAX_HORSEPOWER = 47.0f;
	static constexpr float MAX_HORSEPOWER_RPM = 6000.0f;

	static constexpr int DEFAULT_ENGINE_FREQUENCY = 48000;

	const Vector2 TACHOMETER_POS = Vector2(Application::SCREEN_SIZE_X_HALF, Application::SCREEN_SIZE_Y - 200);
	static constexpr float MAX_SCALE = 9000.0f;

	const float NEEDLE_MIN_RAD = Deg2Rad(205.0f);
	const float NEEDLE_MAX_RAD = Deg2Rad(-25.0f);

	const float SLOP = (NEEDLE_MAX_RAD - NEEDLE_MIN_RAD) / MAX_SCALE;

	const float TORQUE_CURVE = logf((9950.0f * MAX_HORSEPOWER) / (MAX_HORSEPOWER_RPM * MAX_TORQUE)) / (logf(MAX_HORSEPOWER_RPM / MAX_TORQUE_RPM) + 1 - MAX_HORSEPOWER_RPM / MAX_TORQUE_RPM);

	float rpm;

	bool engineFlg;

	Vector2 needlePos;

	float torque;
	float torqueMax;
	float torqueMaxRpm;
	float hoursepower;
	float hoursepowerMax;
	float hoursepowerMaxRpm;
};