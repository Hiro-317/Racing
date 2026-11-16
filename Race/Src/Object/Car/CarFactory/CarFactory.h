#pragma once
#include <vector>
#include <functional>

#include "CarTypeDefine.h"
#include "../../../Manager/Input/InputManager.h"

#include "../Damball/Damball.h"
#include "../Rally/Rally.h"
#include "../Kanjo/Kanjo.h"
#include "../Sport/Sport.h"

std::vector<CarBase*> CarFactory(const CAR_TYPE* type, const int& num, const Input::KEYPAD_NO* keypadNo, const std::function<int(void)> camNo) {

	std::vector<CarBase*> res;

	for (int i = 0; i < num; i++) {
		switch (type[i])
		{
		case CAR_TYPE::SUPER:
		{
			res.emplace_back(new Damball(keypadNo[i], Vector3(500.0f * (i % 2), 0.0f, -500.0f * i), i, camNo));
			break;
		}
		case CAR_TYPE::RALLY:
		{
			res.emplace_back(new Rally(keypadNo[i], Vector3(500.0f * (i % 2), 0.0f, -500.0f * i), i, camNo));
			break;
		}
		case CAR_TYPE::HASHIRIYA:
		{
			res.emplace_back(new Kanjo(keypadNo[i], Vector3(500.0f * (i % 2), 0.0f, -500.0f * i), i, camNo));
			break;
		}
		case CAR_TYPE::SPORTS:
		{
			res.emplace_back(new Sport(keypadNo[i], Vector3(500.0f * (i % 2), 0.0f, -500.0f * i), i, camNo));
			break;
		}
		default:
			break;
		}
	}
	return res;
}