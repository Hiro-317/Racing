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
			rpm = MIN_RPM;
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
			else {

				rpm += IDLING_RPM;
			}
		}
		float rad = Deg2Rad(225.0f - (rpm * rpm) / 300000.0f);
		needlePos = Vector2(TACHOMETER_POS.x + NEEDLE_LENGTH * cosf(rad), TACHOMETER_POS.y + NEEDLE_LENGTH * -sinf(rad));
	}
}

void CarBase::SubDraw(void)
{
	DrawLineAA(TACHOMETER_POS.x, TACHOMETER_POS.y, needlePos.x, needlePos.y, 0xff0000, 5.0f);

	DrawFormatString(100, 100, 0xffffff, "%.2f", rpm);
}

void CarBase::AccelUpdate(void)
{
}
