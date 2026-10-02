#include "CarBase.h"

#include "../../Manager/Input/InputManager.h"

CarBase::CarBase(void) : 
	ActorBase(), engineFlg(false), rpm(), needlePos(TACHOMETER_POS.x + NEEDLE_LENGTH * cosf(NEEDLE_MIN_RAD), TACHOMETER_POS.y + NEEDLE_LENGTH * -sinf(NEEDLE_MIN_RAD))
{
}

CarBase::~CarBase(void)
{
}

void CarBase::Load(void)
{
}

void CarBase::SubUpdate(void)
{
	if (!engineFlg) {
		if (Input::GetIns().GetPriorityKey(Input::COMMAND::USE).keyTrgDown) {

			engineFlg = true;
		}
	}
	else {
		if (Input::GetIns().GetPriorityKey(Input::COMMAND::USE).keyTrgDown) {

			engineFlg = false;
			rpm = 0.0f;
		}
		if (Input::GetIns().GetPriorityKey(Input::COMMAND::DODGE).keyNew) {
			rpm += UP_RPM;
			if (rpm > MAX_RPM) {
				rpm -= REV_RPM;
			}
		}
		else {
			if (rpm > MIN_RPM) {

				rpm -= DOWN_RPM;
			}
			else if (rpm != MIN_RPM) {

				rpm = MIN_RPM;
			}
		}
		torque = MAX_TORQUE * powf(rpm / MAX_TORQUE_RPM, TORQUE_CURVE) * expf(TORQUE_CURVE * (1 - rpm / MAX_TORQUE_RPM));
		if (torqueMax < torque) { torqueMax = torque; torqueMaxRpm = rpm; }
		hoursepower = (torque * rpm) / 9550.0f;
		if (hoursepowerMax < hoursepower) { hoursepowerMax = hoursepower; hoursepowerMaxRpm = rpm; }
		float rad = SLOP * rpm + NEEDLE_MIN_RAD;
		needlePos = Vector2(TACHOMETER_POS.x + NEEDLE_LENGTH * cosf(rad), TACHOMETER_POS.y + NEEDLE_LENGTH * -sinf(rad));
	}
}

void CarBase::SubDraw(void)
{
	DrawLineAA(TACHOMETER_POS.x, TACHOMETER_POS.y, needlePos.x, needlePos.y, 0xff0000, 5.0f);
	DrawCircleAA(TACHOMETER_POS.x, TACHOMETER_POS.y, NEEDLE_LENGTH / 2, 32, 0x000000);

	DrawFormatString(100, 20, 0xffffff, "%.2f", rpm);
	DrawFormatString(200, 20, 0xffffff, "%.2f", torque);
	DrawFormatString(200, 40, 0xffffff, "%.2f", torqueMax);
	DrawFormatString(200, 60, 0xffffff, "%.2f", torqueMaxRpm);
	DrawFormatString(300, 20, 0xffffff, "%.2f", hoursepower);
	DrawFormatString(300, 40, 0xffffff, "%.2f", hoursepowerMax);
	DrawFormatString(300, 60, 0xffffff, "%.2f", hoursepowerMaxRpm);
}

void CarBase::SubRelease(void)
{
}

void CarBase::AccelUpdate(void)
{
}
