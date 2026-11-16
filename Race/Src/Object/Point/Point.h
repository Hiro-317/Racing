#pragma once

#include "../ActorBase/ActorBase.h"

#include "Goal.h"


class Point : public ActorBase {

public:

	Point(Goal* goalInsRef);
	~Point(void) = default;

	void Load(void) override;

	void OnCollision(COLLIDER_TAG ownTag, const ColliderBase& other, const CollisionResult& result) override;

private:

	void SubInit(void)override;
	void SubDraw(void)override;

	Transform subTrans;

	bool hitNo[4];

	Goal* goalInsRef;
};