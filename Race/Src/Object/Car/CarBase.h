#pragma once

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

	void AccelUpdate(void) override;

private:

	static constexpr int NEEDLE_LENGTH = 200;
	static constexpr float MIN_RPM = 1000.0f;
	static constexpr float MAX_RPM = 7700.0f;
	static constexpr float REV_RPM = 250.0f;
	static constexpr float IDLING_RPM = 400.0f;
	static constexpr float UP_RPM = 80.0f;
	static constexpr float DOWN_RPM = 100.0f;

	const Vector2 TACHOMETER_POS = Vector2(Application::SCREEN_SIZE_X_HALF, Application::SCREEN_SIZE_Y - 200);
	const float NEEDLE_MIN_RAD = Deg2Rad(225.0f);
	const float NEEDLE_MAX_RAD = Deg2Rad(-45.0f);

	float rpm;

	bool engineFlg;

	Vector2 needlePos;
};