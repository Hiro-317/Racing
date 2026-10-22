#include "Point.h"

#include "../Common/Collider/CapsuleCollider.h"


Point::Point(void) : ActorBase()
{
}

void Point::Load(void)
{
	AddCollider(new CapsuleCollider(COLLIDER_TAG::Enemy, Vector3(0.0f, 0.0f, 1000.0f), Vector3(0.0f, 100.0f, 1000.0f), 20.0f));
}

void Point::OnCollision(COLLIDER_TAG ownTag, const ColliderBase& other, const CollisionResult& result)
{
	hitNum++;
	ColliderSerch<CapsuleCollider>(GetColliders()).back()->SetTransformPos(Vector3(1000.0f, 0.0f, 1000.0f) * hitNum);
}
