#include "TitleScene.h"

#include <cmath>

#include "../../Common/Utility.h"

#include "../../Application/Application.h"

//#include "../../Manager/Net/NetWorkManager.h"
#include "../../Manager/Sound/SoundManager.h"
#include "../../Manager/Font/FontManager.h"

#include "../../Manager/Scene/SceneManager.h"


TitleScene::TitleScene() :
	SceneBase()
{
	App::GetIns().SetDrawDebug(true);
}

void TitleScene::SubPostLoad(void)
{
	DevideScreen(1);
	num = 1;
	flg = false;
	tmpNum = 0;
}

void TitleScene::SubPostInit(void)
{
}

void TitleScene::SubPostUpdate(void)
{
	if (!flg) {
		if (Input::GetIns().GetPriorityKey(COMMAND::LEFT).keyTrgDown) {
			num--;
			if (num < 1) {
				num = 1;
			}
		}
		if (Input::GetIns().GetPriorityKey(COMMAND::RIGHT).keyTrgDown) {
			num++;
			if (num > 4) {
				num = 4;
			}
		}
		if (Input::GetIns().GetPriorityKey(COMMAND::DECIDE).keyTrgDown) {
			flg = true;
		}
	}
	else {
		if (Input::GetIns().GetPriorityAnyoneTrgDown()) {
			tmpNo[tmpNum] = Input::GetIns().GetMostPriority();
		}
		if (Input::GetIns().GetKey(COMMAND::DECIDE, tmpNo[tmpNum]).keyTrgDown) {
			if (tmpNum < num - 1) {
				tmpNum++;
			}
			else {
				for (int i = 0; i < num; i++) {
					SceneManager::GetIns().SetKeypadNo(tmpNo[i], i);
				}
				SceneManager::GetIns().SetPlayerNum(num);
				SceneManager::GetIns().ChangeSceneFade(SCENE_ID::Game);
			}
		}
	}
}

void TitleScene::SubUiDraw(void)
{
	DrawFormatString(10, 10, 0xffffff, "タイトル", SetFontSize(30));
	DrawFormatString(10, 40, 0xffffff, "%d人です", num);
	std::string tmp{};
	if (tmpNo[tmpNum] == Input::KEYPAD_NO::KEY){
		tmp = "キーボード";
	}
	if (tmpNo[tmpNum] == Input::KEYPAD_NO::PAD1){
		tmp = "PAD1";
	}
	if (tmpNo[tmpNum] == Input::KEYPAD_NO::PAD2){
		tmp = "PAD2";
	}
	if (tmpNo[tmpNum] == Input::KEYPAD_NO::PAD3){
		tmp = "PAD3";
	}
	if (tmpNo[tmpNum] == Input::KEYPAD_NO::PAD4){
		tmp = "PAD4";
	}
	DrawFormatString(10, 70, 0xffffff, (tmp + "です").c_str());
	SetFontSize(16);
}

void TitleScene::SubPostRelease(void)
{
}
