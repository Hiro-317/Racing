#include "GameScene.h"

#include <cmath>

#include "../../Common/Utility.h"

#include "../../Application/Application.h"

//#include "../../Manager/Net/NetWorkManager.h"
#include "../../Manager/Camera/FollowAuto/FollowAutoCamera.h"
#include "../../Manager/Input/InputManager.h"
#include "../../Manager/Sound/SoundManager.h"
#include "../../Manager/Font/FontManager.h"

#include "../../Manager/Scene/SceneManager.h"


GameScene::GameScene() :
	SceneBase(),
	focusFlg(false),
	time(0.0f)
{
}

void GameScene::SubPostLoad(void)
{
	//Snd::GetIns().ChangeScene("Game");

	carInsRef = new CarBase();
	AddActor(carInsRef);

}

void GameScene::SubPostInit(void)
{
	//SoundManager::GetIns().Play("Battle");

	time = 0;
}

void GameScene::SubPreUpdate(void)
{
	//if (Net::GetIns().GetState() == Net::NetState::None) { return; }

	//while (auto dataPtr = Net::GetIns().GetMsgData<MsgDataGameTime>()) {
	//	time = dataPtr->time;

	//	SceneManager::GetIns().SetClearTime(time);

	//	delete dataPtr;
	//}

	//while (auto dataPtr = Net::GetIns().GetMsgData<MsgDataSystemInform>(MSG_SENDER_ID::None, true)) {
	//	SceneManager::GetIns().PushScene(std::make_unique<GamePause>(dataPtr->header.senderId));
	//	delete dataPtr;
	//}
	//// 切断 の受信
	//while (auto dataPtr = Net::GetIns().GetMsgData<MsgDataConnectInform>()) {

	//	if (dataPtr->inform == MsgDataConnectInform::INFORM_TYPE::Disconnect) {
	//		Net::GetIns().Disconnection();
	//		SceneManager::GetIns().JumpSceneFade(SCENE_ID::Lobby);
	//	}

	//	delete dataPtr;
	//}
}

void GameScene::SubPostUpdate(void)
{
	Vector3 pos = Vector3();

#pragma region 遷移判定（ポーズも含む）
	// ポーズ判定
	//if (Key::GetIns().GetInfo(KEY_TYPE::PAUSE).down) {

	//	//Net::GetIns().EventInformSend(MsgDataSystemInform::INFORM_TYPE::PushGamePause);
	//	//EffectManager::GetIns()->PauseEffect();
	//	//SceneManager::GetIns().PushScene(std::make_unique<GamePause>(Net::GetIns().GetSenderId()));
	//	return;
	//}

	//// ゲームクリア判定
	//if (bossInsRef->GetIsDeath()) {

	//	// 同期
	//	if (Net::GetIns().GetState() != Net::NetState::None && Net::GetIns().IsHost()) { Net::GetIns().Send(MsgDataGameTime(time)); }
	//	SceneManager::GetIns().SetClearTime(time);


	//	// シーン切り替え時に前シーンのエフェクトを残さない
	//	if (EffectManager::GetIns() != nullptr) { EffectManager::GetIns()->StopEffectAll(); }

	//	switch (SceneManager::GetIns().GetSelectBossType())
	//	{
	//	case BOSS_TYPE::Tomato:
	//		SceneManager::GetIns().ChangeSceneFade(SCENE_ID::Clear, FADE_TYPE::DEFAULT, 90, 0xe33434, 0x000000);
	//		break;
	//	case BOSS_TYPE::Grape:
	//		SceneManager::GetIns().ChangeSceneFade(SCENE_ID::Clear, FADE_TYPE::DEFAULT, 90, 0x9338e8, 0x000000);
	//		break;
	//	case BOSS_TYPE::Banana:
	//		SceneManager::GetIns().ChangeSceneFade(SCENE_ID::Clear, FADE_TYPE::DEFAULT, 90, 0xfcf80a, 0x000000);
	//		break;
	//	default:
	//		break;
	//	}
	//	return;
	//}

	//// ゲームオーバー判定
	//if (ObjSerch<PlayerManager>(objects)->IsPlayerAllDeath()) {

	//	// シーン切り替え時に前シーンのエフェクトを残さない
	//	if (EffectManager::GetIns() != nullptr) { EffectManager::GetIns()->StopEffectAll(); }

	//	SceneManager::GetIns().ChangeSceneFade(SCENE_ID::GameOver, FADE_TYPE::DEFAULT, 90, 0xffffff, 0x000000);
	//	return;
	//}

#if _DEBUG
	// シーンを再読み込み
	//if (Key::GetIns().GetInfo(KEY_TYPE::DEBUG_RELOAD).down) {
	//	SceneManager::GetIns().ChangeSceneFade(SCENE_ID::Game);

	//	// シーン切り替え時に前シーンのエフェクトを残さない
	//	if (EffectManager::GetIns() != nullptr) { EffectManager::GetIns()->StopEffectAll(); }

	//	return;
	//}
#endif // _DEBUG

	//EffectManager::GetIns()->UpdateEffect();
	//EffectManager::GetIns()->ReceptionUpdate();
	//UpdateEffekseer3D();

#pragma endregion

#pragma region 時間計測

	// 時間更新
	time += 1 / App::FRAME_PAR_SECOND;

	//// 同期
	//if (Net::GetIns().GetState() != Net::NetState::None&&Net::GetIns().IsHost()) {
	//	if ((char)time % MULTI_TIME_SEND_COUNT == 0) { Net::GetIns().Send(MsgDataGameTime(time)); }
	//}
#pragma endregion
}

void GameScene::SubUiDraw(void)
{
	DrawString(0, 0, std::to_string(time).c_str(), 0xffffff);
}

void GameScene::SubPostRelease(void)
{
	//Net::GetIns().ReceptionDataReset();
	carInsRef = nullptr;
}


void GameScene::CreateCamera(void)
{
	camera = new FollowAutoCamera(&carInsRef->GetTrans().pos, &carInsRef->GetFocusPoint(), 600.0f, 610.0f, 0.8f);
}
