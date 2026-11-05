#include "Point.h"

#include "../Common/Collider/BoxCollider.h"
#include "../../Manager/Scene/SceneManager.h"

#include "../Car/CarBase/CarBase.h"


Point::Point(Goal* goalInsRef) : 
	ActorBase(),
	goalInsRef(goalInsRef)
{
}

void Point::Load(void)
{
	trans.LoadModel("Point/Point");
	subTrans.LoadModel("Point/Point");
	trans.pos = Vector3(850.0f, 0.0f, 10000.0f);
	subTrans.pos = Vector3(-850.0f, 0.0f, 10000.0f);
	trans.scale = Vector3(3.0f);
	AddCollider(new BoxCollider(COLLIDER_TAG::Enemy, Vector3(1700.0f, 1500.0f, 30.0f), Vector3::Xonly(-850.0f)));
	SetPushFlg(false);
}

void Point::OnCollision(COLLIDER_TAG ownTag, const ColliderBase& other, const CollisionResult& result)
{
	if (other.GetTag() == COLLIDER_TAG::Player) {
		int no = dynamic_cast<CarBase*>(other.GetActor())->GetPlayerNo();
		if (!hitNo[no]) {
			hitNo[no] = true;
			goalInsRef->Hit(no);
		}
	}
}

void Point::SubInit(void)
{
	for (bool no : hitNo) {
		no = false;
	}
}

void Point::SubDraw(void)
{
	subTrans.Draw();
}
