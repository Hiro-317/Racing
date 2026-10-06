#include "CarBase.h"

#include "../../Manager/Input/InputManager.h"

CarBase::CarBase(void) : 
	ActorBase(), engineFlg(false), rpm(), needlePos(TACHOMETER_POS.x + NEEDLE_LENGTH * cosf(NEEDLE_MIN_RAD), TACHOMETER_POS.y + NEEDLE_LENGTH * -sinf(NEEDLE_MIN_RAD)), nowGear(0)
{
	idleSpeed = (MIN_RPM * 3.6f * 2.0f * DX_PI_F * TIRE_SIZE_ROUND) / (GEAR_RATIO[0] * FINAL_GEAR_RATIO * 60.0f);
}

CarBase::~CarBase(void)
{
}

void CarBase::Load(void)
{
}

void CarBase::SubUpdate(void)
{
	prevRpm = rpm;
	if (!engineFlg) {
		if (rpm > MIN_RPM) {

			rpm -= DOWN_RPM * GEAR_RATIO[nowGear];
		}
		else {

			rpm = 0.0f;
		}
		if (Input::GetIns().GetPriorityKey(Input::COMMAND::USE).keyTrgDown && Input::GetIns().GetPriorityKey(Input::COMMAND::RUN).keyNew) {

			engineFlg = true;
			rpm = MIN_RPM;
		}
	}
	else {
		if (nowGear < 5) {
			if (rpm * (GEAR_RATIO[nowGear + 1] / GEAR_RATIO[nowGear]) >= MIN_RPM) {
				if (Input::GetIns().GetPriorityKey(Input::COMMAND::DOWN).keyTrgDown) {
					rpm = rpm * (GEAR_RATIO[nowGear + 1] / GEAR_RATIO[nowGear]);
					nowGear++;
				}
			}
		}
		if (nowGear > 0) {
			if (rpm * (GEAR_RATIO[nowGear - 1] / GEAR_RATIO[nowGear]) <= MAX_RPM) {
				if (Input::GetIns().GetPriorityKey(Input::COMMAND::UP).keyTrgDown || rpm <= MIN_RPM) {
					rpm = rpm * (GEAR_RATIO[nowGear - 1] / GEAR_RATIO[nowGear]);
					nowGear--;
				}
			}
		}
		if (Input::GetIns().GetPriorityKey(Input::COMMAND::RUN).keyNew) {
			if (Input::GetIns().GetPriorityKey(Input::COMMAND::DODGE).keyNew) {
				if (!fuelCut) {
					rpm += UP_RPM;
					if (rpm > MAX_TORQUE_RPM) {
						fuelCut = true;
					}
				}
				else {
					rpm -= DOWN_RPM;
					fuelCut = false;
				}
			}
			else {
				if (speed > 0.0f) {
					speed -= 0.5f;
					float ep = (rpm * GEAR_RATIO[nowGear] * FINAL_GEAR_RATIO * ENGINE_BRAKE) / (TIRE_SIZE_ROUND);
					float ar = AIR_DENSITY * CD * FPA * (speed / 3.6f) * (speed / 3.6f) * 0.5f;
					float rr = RR * 9.81f * CAR_WEIGHT;
					float acc = (-ep - ar - rr) / CAR_WEIGHT;
					speed += acc * 3.6f / Application::FRAME_PAR_SECOND;
					rpm = (speed * GEAR_RATIO[nowGear] * FINAL_GEAR_RATIO * 60.0f) / (3.6f * 2.0f * DX_PI_F * TIRE_SIZE_ROUND);
				}
				else{
					speed = 0.0f;
					if (rpm > MIN_RPM) {
						rpm -= DOWN_RPM;
					}
					if (rpm < MIN_RPM) {
						rpm = MIN_RPM;
					}
				}
			}
		}
		else if (idleSpeed > speed) {
			float clatch = 1.0f - (idleSpeed - speed) / idleSpeed + 0.25f;
			if (clatch > 1.0f) {
				clatch = 1.0f;
			}
			torque = MAX_TORQUE * powf(rpm / MAX_TORQUE_RPM, TORQUE_CURVE) * expf(TORQUE_CURVE * (1 - rpm / MAX_TORQUE_RPM));
			float ep = (torque * GEAR_RATIO[nowGear] * FINAL_GEAR_RATIO * LOST_RATIO * clatch) / TIRE_SIZE_ROUND;
			float ar = AIR_DENSITY * CD * FPA * (speed / 3.6f) * (speed / 3.6f) * 0.5f;
			float rr = RR * 9.81f * CAR_WEIGHT;
			float acc = (ep - ar - rr) / CAR_WEIGHT;
			speed += acc * 3.6f / Application::FRAME_PAR_SECOND;
			float spRpm = (speed * GEAR_RATIO[nowGear] * FINAL_GEAR_RATIO * 60.0f) / (3.6f * 2.0f * DX_PI_F * TIRE_SIZE_ROUND);

			rpm = rpm * (1.0f - clatch) + spRpm * clatch;
		}
		else {
			if (Input::GetIns().GetPriorityKey(Input::COMMAND::DODGE).keyNew && !fuelCut) {
				torque = MAX_TORQUE * powf(rpm / MAX_TORQUE_RPM, TORQUE_CURVE) * expf(TORQUE_CURVE * (1 - rpm / MAX_TORQUE_RPM));
				float ep = (torque * GEAR_RATIO[nowGear] * FINAL_GEAR_RATIO * LOST_RATIO) / TIRE_SIZE_ROUND;
				float ar = AIR_DENSITY * CD * FPA * (speed / 3.6f) * (speed / 3.6f) * 0.5f;
				float rr = RR * 9.81f * CAR_WEIGHT;
				float acc = (ep - ar - rr) / CAR_WEIGHT;
				speed += acc * 3.6f / Application::FRAME_PAR_SECOND;
				rpm = (speed * GEAR_RATIO[nowGear] * FINAL_GEAR_RATIO * 60.0f) / (3.6f * 2.0f * DX_PI_F * TIRE_SIZE_ROUND);
				if (rpm > MAX_RPM) {
					fuelCut = true;
				}
			}
			else {
				float ep = (rpm * GEAR_RATIO[nowGear] * FINAL_GEAR_RATIO * ENGINE_BRAKE) / (TIRE_SIZE_ROUND);
				float ar = AIR_DENSITY * CD * FPA * (speed / 3.6f) * (speed / 3.6f) * 0.5f;
				float rr = RR * 9.81f * CAR_WEIGHT;
				float acc = (-ep - ar - rr) / CAR_WEIGHT;
				speed += acc * 3.6f / Application::FRAME_PAR_SECOND;
				rpm = (speed * GEAR_RATIO[nowGear] * FINAL_GEAR_RATIO * 60.0f) / (3.6f * 2.0f * DX_PI_F * TIRE_SIZE_ROUND);
				if (fuelCut) {
					fuelCut = false;
				}
			}
			if (speed < idleSpeed) {
				speed = idleSpeed;
				rpm = MIN_RPM;
			}
		}
	}
	if (torqueMax < torque) { torqueMax = torque; torqueMaxRpm = rpm; }
	hoursepower = (torque * rpm) / 9550.0f;
	if (hoursepowerMax < hoursepower) { hoursepowerMax = hoursepower; hoursepowerMaxRpm = rpm; }
	float rad = SLOP * rpm + NEEDLE_MIN_RAD;
	needlePos = Vector2(TACHOMETER_POS.x + NEEDLE_LENGTH * cosf(rad), TACHOMETER_POS.y + NEEDLE_LENGTH * -sinf(rad));
}

void CarBase::SubDraw(void)
{
	for (int i = 0; i <= (int)MAX_SCALE / 100; i++) {
		float rad = SLOP * (i * 100) + NEEDLE_MIN_RAD;
		Vector2 brank = Vector2(TACHOMETER_POS.x + BLANK_LENGTH * cosf(rad), TACHOMETER_POS.y + BLANK_LENGTH * -sinf(rad));

		if (i % 10 == 0) {
			Vector2 line = Vector2(TACHOMETER_POS.x + (NEEDLE_LENGTH + 5.0f) * cosf(rad), TACHOMETER_POS.y + (NEEDLE_LENGTH + 5.0f) * -sinf(rad));
			Vector2 number = Vector2(TACHOMETER_POS.x + (BLANK_LENGTH - 15.0f) * cosf(rad), TACHOMETER_POS.y + (BLANK_LENGTH - 15.0f) * -sinf(rad));
			DrawFormatString(number.x - 5.0f, number.y - 5.0f, 0xffffff, "%d", i / 10);
			if (i < 70) {
				DrawLineAA(line.x, line.y, brank.x, brank.y, 0xffffff, 4.0f);
			}
			else {
				DrawLineAA(line.x, line.y, brank.x, brank.y, 0xff0000, 4.0f);
			}
		}
		else if (i % 5 == 0) {
			Vector2 line = Vector2(TACHOMETER_POS.x + NEEDLE_LENGTH * cosf(rad), TACHOMETER_POS.y + NEEDLE_LENGTH * -sinf(rad));
			if (i < 70) {
				DrawLineAA(line.x, line.y, brank.x, brank.y, 0xffffff, 2.0f);
			}
			else {
				DrawLineAA(line.x, line.y, brank.x, brank.y, 0xff0000, 2.0f);
			}
		}
		else {
			Vector2 line = Vector2(TACHOMETER_POS.x + (NEEDLE_LENGTH - 5.0f) * cosf(rad), TACHOMETER_POS.y + (NEEDLE_LENGTH - 5.0f) * -sinf(rad));
			if (i < 70) {
				DrawLineAA(line.x, line.y, brank.x, brank.y, 0xffffff);
			}
			else {
				DrawLineAA(line.x, line.y, brank.x, brank.y, 0xff0000);
			}
		}
	}
	DrawLineAA(TACHOMETER_POS.x, TACHOMETER_POS.y, needlePos.x, needlePos.y, 0xff0000, 5.0f);
	DrawCircleAA(TACHOMETER_POS.x, TACHOMETER_POS.y, NEEDLE_LENGTH / 12 * 7 + 3.0f, 32, 0xff0000);
	DrawCircleAA(TACHOMETER_POS.x, TACHOMETER_POS.y, NEEDLE_LENGTH / 12 * 7, 32, 0x000000);
	SetFontSize(40);
	int s = static_cast<int>(speed) % 10;
	DrawFormatString(TACHOMETER_POS.x + 12, TACHOMETER_POS.y - 20, 0xffffff, "%d", s);
	if ((s = static_cast<int>(speed) / 10) >= 1) {
		DrawFormatString(TACHOMETER_POS.x - 12, TACHOMETER_POS.y - 20, 0xffffff, "%d", s % 10);
	}
	if ((s = static_cast<int>(speed) / 100) >= 1) {
		DrawFormatString(TACHOMETER_POS.x - 36, TACHOMETER_POS.y - 20, 0xffffff, "%d", s);
	}

	SetFontSize(16);
	DrawFormatString(100, 20, 0xffffff, "%.2f", rpm);
	DrawFormatString(200, 20, 0xffffff, "%.2f", torque);
	DrawFormatString(200, 40, 0xffffff, "%.2f", torqueMax);
	DrawFormatString(200, 60, 0xffffff, "%.2f", torqueMaxRpm);
	DrawFormatString(300, 20, 0xffffff, "%.2f", hoursepower);
	DrawFormatString(300, 40, 0xffffff, "%.2f", hoursepowerMax);
	DrawFormatString(300, 60, 0xffffff, "%.2f", hoursepowerMaxRpm);
	DrawFormatString(400, 20, 0xffffff, "%d", nowGear + 1);
}

void CarBase::SubRelease(void)
{
}

void CarBase::AccelUpdate(void)
{
}
