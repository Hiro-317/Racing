#include "Goal.h"

#include "../Common/Collider/BoxCollider.h"
#include "../../Manager/Scene/SceneManager.h"

#include "../Car/CarBase/CarBase.h"


Goal::Goal(int num) : 
	ActorBase(),
	num(num)
{
}

void Goal::Load(void)
{
	trans.LoadModel("Goal/Goal");
	trans.pos = Vector3(0.0f, 0.0f, 20000.0f);
	trans.scale = Vector3(8.0f);
	AddCollider(new BoxCollider(COLLIDER_TAG::Enemy, Vector3(1700.0f, 1500.0f, 30.0f)));
	SetPushFlg(false);
}

void Goal::OnCollision(COLLIDER_TAG ownTag, const ColliderBase& other, const CollisionResult& result)
{
	if (other.GetTag() == COLLIDER_TAG::Player) {
		if (hitNum[dynamic_cast<CarBase*>(other.GetActor())->GetPlayerNo()] == num) {
			SceneManager::GetIns().ChangeSceneFade(SCENE_ID::Title);
		}
	}
}

void Goal::SubInit(void)
{
	for (int no : hitNum) {
		no = 0;
	}
}
