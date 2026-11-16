#include "EndScene.h"

#include "../../Application/Application.h"


EndScene::EndScene() :
	SceneBase()
{
}

void EndScene::SubPreLoad(void)
{
	App::GetIns().GameEnd();
}
