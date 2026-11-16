#pragma once

#include "../ActorBase/ActorBase.h"

class Goal : public ActorBase {

public:

	Goal(int num);
	~Goal(void) = default;

	void Load(void) override;

	void OnCollision(COLLIDER_TAG ownTag, const ColliderBase& other, const CollisionResult& result) override;

	void Hit(int no) { hitNum[no]++; }

private:

	void SubInit(void)override;

	int hitNum[4];

	const int num;
};