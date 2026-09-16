#pragma once
#include "EffectBase.h"

class EffectFactory {

public:

	static EffectBase* CreateEffect(const ParameterLoad& parameter, EFFECT_NAME name, const Vector3& local, const Transform* trans,
	bool followRotX, bool followRotY, bool followRotZ) {

		EffectBase* res{};

		switch (name){

		default: { break; }
		}
		return res;
	}
};