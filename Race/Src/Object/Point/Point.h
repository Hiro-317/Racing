#pragma once

#include "../ActorBase/ActorBase.h"

class Point : public ActorBase {

public:

	Point(void);
	~Point(void) = default;

	void Load(void) override;

	void OnCollision(COLLIDER_TAG ownTag, const ColliderBase& other, const CollisionResult& result) override;

private:

	float hitNum;
};