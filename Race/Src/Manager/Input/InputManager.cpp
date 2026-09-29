#include <fstream>
#include <vector>
#include "InputManager.h"
#include "KeyMouse/Keyboard.h"
#include "../../Application/Application.h"
#include "../../Common/Utility.h"
#include "../../Common/Vector3.h"


InputManager* InputManager::inst_ = nullptr;

void InputManager::CreateInstance(void)
{
	inst_ = new InputManager();
}

InputManager& InputManager::GetInstance(void)
{
	if (inst_ == nullptr) {

		CreateInstance();
	}
	return *inst_;
}

// デフォルトをここに登録
// 登録はDXLIBのDifineで
// パッドはintの被りに注意して
void InputManager::CommandLoad(void)
{
	// ラン
	SetCommand(COMMAND::RUN, KEY_INPUT_LSHIFT, KEYPAD_NO::KEY);
	SetCommand(COMMAND::RUN, PAD_INPUT_R, KEYPAD_NO::PAD1);

	// コンボ
	SetCommand(COMMAND::COMBO, MOUSE_INPUT_LEFT, KEYPAD_NO::MOUSE);
	SetCommand(COMMAND::COMBO, XINPUT_BUTTON_Y, KEYPAD_NO::PAD1);

	// 攻撃
	SetCommand(COMMAND::ATTACK, MOUSE_INPUT_RIGHT, KEYPAD_NO::MOUSE);
	SetCommand(COMMAND::ATTACK, XINPUT_BUTTON_B, KEYPAD_NO::PAD1);

	// 回避
	SetCommand(COMMAND::DODGE, KEY_INPUT_SPACE, KEYPAD_NO::KEY);
	SetCommand(COMMAND::DODGE, XINPUT_BUTTON_A, KEYPAD_NO::PAD1);

	// ロックオン
	SetCommand(COMMAND::LOCK_ON, MOUSE_INPUT_MIDDLE, KEYPAD_NO::MOUSE);
	SetCommand(COMMAND::LOCK_ON, KEY_INPUT_RCONTROL, KEYPAD_NO::KEY);
	SetCommand(COMMAND::LOCK_ON, KEY_INPUT_TAB, KEYPAD_NO::KEY);
	SetCommand(COMMAND::LOCK_ON, PAD_INPUT_L, KEYPAD_NO::PAD1);

	// 使う
	SetCommand(COMMAND::USE, KEY_INPUT_F, KEYPAD_NO::KEY);
	SetCommand(COMMAND::USE, XINPUT_BUTTON_X, KEYPAD_NO::PAD1);

	// 左キー
	SetCommand(COMMAND::LEFT, KEY_INPUT_Q, KEYPAD_NO::KEY);
	SetCommand(COMMAND::LEFT, KEY_INPUT_LEFT, KEYPAD_NO::KEY);
	SetCommand(COMMAND::LEFT, XINPUT_BUTTON_DPAD_LEFT, KEYPAD_NO::PAD1);

	// 右キー
	SetCommand(COMMAND::RIGHT, KEY_INPUT_E, KEYPAD_NO::KEY);
	SetCommand(COMMAND::RIGHT, KEY_INPUT_RIGHT, KEYPAD_NO::KEY);
	SetCommand(COMMAND::RIGHT, XINPUT_BUTTON_DPAD_RIGHT, KEYPAD_NO::PAD1);

	// 上キー
	SetCommand(COMMAND::UP, KEY_INPUT_W, KEYPAD_NO::KEY);
	SetCommand(COMMAND::UP, KEY_INPUT_UP, KEYPAD_NO::KEY);
	SetCommand(COMMAND::UP, XINPUT_BUTTON_DPAD_UP, KEYPAD_NO::PAD1);

	// 下キー
	SetCommand(COMMAND::DOWN, KEY_INPUT_S, KEYPAD_NO::KEY);
	SetCommand(COMMAND::DOWN, KEY_INPUT_DOWN, KEYPAD_NO::KEY);
	SetCommand(COMMAND::DOWN, XINPUT_BUTTON_DPAD_DOWN, KEYPAD_NO::PAD1);

	// ポーズキー
	SetCommand(COMMAND::PAUSE, KEY_INPUT_ESCAPE, KEYPAD_NO::KEY);
	SetCommand(COMMAND::PAUSE, XINPUT_BUTTON_BACK, KEYPAD_NO::PAD1);
	SetCommand(COMMAND::PAUSE, XINPUT_BUTTON_START, KEYPAD_NO::PAD1);

	// 決定キー
	SetCommand(COMMAND::DECIDE, MOUSE_INPUT_LEFT, KEYPAD_NO::MOUSE);
	SetCommand(COMMAND::DECIDE, KEY_INPUT_SPACE, KEYPAD_NO::KEY);
	SetCommand(COMMAND::DECIDE, KEY_INPUT_RETURN, KEYPAD_NO::KEY);
	SetCommand(COMMAND::DECIDE, XINPUT_BUTTON_A, KEYPAD_NO::PAD1);

	// キャンセルキー
	SetCommand(COMMAND::CANCEL, MOUSE_INPUT_RIGHT, KEYPAD_NO::MOUSE);
	SetCommand(COMMAND::CANCEL, KEY_INPUT_ESCAPE, KEYPAD_NO::KEY);
	SetCommand(COMMAND::CANCEL, XINPUT_BUTTON_B, KEYPAD_NO::PAD1);
}

// 初期化
void InputManager::Init(void){

	// キーマウ取得
	keyboard_ = new Keyboard();
	keyboard_->Init();
	mouse_ = new Mouse();
	mouse_->Init();

	// パッド取得
	for (int num = static_cast<int>(KEYPAD_NO::PAD1); num < static_cast<int>(KEYPAD_NO::PAD4); num++) {

		pads_.emplace_back(new Controller(num + 1));
		pads_.back()->Init();
	}

	// デフォルトコマンド登録
	CommandLoad();
}

// 更新
void InputManager::Update(void){

	// 優先を初期化
	orderOfPriority_.clear();
	KEYPAD_NO tmp = mostPriority_;
	Controller::JOYPAD_TYPE tmpType = mostPriorityType_;
	mostPriority_ = KEYPAD_NO::NON;
	mostPriorityType_ = Controller::JOYPAD_TYPE::NON;

	// 現在の接続しているパッドの数のみ更新(1番から)
	for (int num = static_cast<int>(KEYPAD_NO::PAD1); num < GetJoypadNum(); num++) {

		pads_.at(num)->Update();
		// 優先順に動いているか
		orderOfPriority_.emplace_back(pads_.at(num)->GetAnyone());
		// 最も優先的に動いているもの
		if (orderOfPriority_.back() && mostPriority_ == KEYPAD_NO::NON) {
			mostPriority_ = static_cast<KEYPAD_NO>(num);
			mostPriorityType_ = pads_.at(num)->GetJPadType();
		}
	}

	// キーマウ更新
	keyboard_->Update();
	mouse_->Update();
	orderOfPriority_.emplace_back(keyboard_->GetAnyone() || mouse_->GetAnyone());

	// 最も優先的に動いているもの
	if (mostPriority_ == KEYPAD_NO::NON) {
		if (orderOfPriority_.back()) {
			mostPriority_ = KEYPAD_NO::KEY;
			mostPriorityType_ = Controller::JOYPAD_TYPE::NON;
		}
		// どれも動いてないときは直近のものを取るようにする
		// 直近がないならPAD1
		// 繋いでなくても仮想的に
		else {
			if (tmp == KEYPAD_NO::NON) {
				orderOfPriority_.emplace_back(true);
				mostPriority_ = KEYPAD_NO::PAD1;
				mostPriorityType_ = pads_.at(static_cast<int>(KEYPAD_NO::PAD1))->GetJPadType();
				if (mostPriorityType_ == Controller::JOYPAD_TYPE::NON) {
					mostPriorityType_ = Controller::JOYPAD_TYPE::XBOX_360;
				}
			}
			else {
				mostPriority_ = tmp;
				mostPriorityType_ = tmpType;
			}
		}
	}
}

// リソースの破棄
void InputManager::Release(void){

	// 解放
	keyboard_->Release();
	mouse_->Release();
	for (auto p : pads_) {

		p->Release();
		delete p;
	}
	delete keyboard_;
	delete mouse_;
	pads_.clear();
}

std::map<InputManager::KEYPAD_NO, VECTOR>  InputManager::GetDirectionXZAKeyL(void)
{
	std::map<KEYPAD_NO, VECTOR>  res;

	Vector3 nowPos = Vector3();

	// WASDをアナログに変換
	if (keyboard_->GetKey(KEY_INPUT_W).keyNew) {
		nowPos.z += 1.0f;
	}
	if (keyboard_->GetKey(KEY_INPUT_S).keyNew) {
		nowPos.z -= 1.0f;
	}
	if (keyboard_->GetKey(KEY_INPUT_D).keyNew) {
		nowPos.x += 1.0f;
	}
	if (keyboard_->GetKey(KEY_INPUT_A).keyNew) {
		nowPos.x -= 1.0f;
	}
	if (nowPos != Vector3()) {
		nowPos = nowPos.Normalized().ToVECTOR();
	}
	res[KEYPAD_NO::KEY] = nowPos.ToVECTOR();

	// 現在の接続しているパッドの数のみ取得(1番から)
	for (int num = static_cast<int>(KEYPAD_NO::PAD1); num < GetJoypadNum(); num++) {

		nowPos = pads_.at(num)->GetDirectionXZAKeyL();
		res[static_cast<KEYPAD_NO>(num)] = nowPos.ToVECTOR();
	}
	return res;
}

std::map<InputManager::KEYPAD_NO, VECTOR>  InputManager::GetDirectionXZAKeyR(void)
{
	std::map<KEYPAD_NO, VECTOR>  res;

	Vector3 nowPos = Vector3();

	// WASDをアナログに変換
	if (keyboard_->GetKey(KEY_INPUT_UP).keyNew) {
		nowPos.z += 1.0f;
	}
	if (keyboard_->GetKey(KEY_INPUT_DOWN).keyNew) {
		nowPos.z -= 1.0f;
	}
	if (keyboard_->GetKey(KEY_INPUT_LEFT).keyNew) {
		nowPos.x += 1.0f;
	}
	if (keyboard_->GetKey(KEY_INPUT_RIGHT).keyNew) {
		nowPos.x -= 1.0f;
	}
	if (nowPos != Vector3()) {
		nowPos = nowPos.Normalized().ToVECTOR();
	}
	res[KEYPAD_NO::KEY] = nowPos.ToVECTOR();

	//　中心からの差分をとる
	nowPos = Vector3(mouse_->GetMousePos());
	VECTOR diffPos = VGet(nowPos.x - Application::SCREEN_SIZE_X_HALF, 0.0f, nowPos.y - Application::SCREEN_SIZE_Y_HALF);
	res[KEYPAD_NO::KEY] = VScale(diffPos, Mouse::MOUSE_SENSI);

	// 現在の接続しているパッドの数のみ取得(1番から)
	for (int num = static_cast<int>(KEYPAD_NO::PAD1); num < GetJoypadNum(); num++) {

		nowPos = pads_.at(num)->GetDirectionXZAKeyR();
		res[static_cast<KEYPAD_NO>(num)] = VScale(nowPos.ToVECTOR(), Controller::CameraSensi);
	}
	return res;
}

InputBase::Info InputManager::GetKey(const COMMAND& com, const KEYPAD_NO& no)
{
	// 状態を格納する変数
	std::vector<InputBase::Info> infos;
	// キーマウ
	if (no == KEYPAD_NO::KEY) {
		// コマンドが登録されているか
		if (keyCommand_.contains(com)) {
			// コマンドに紐づけられているキーをすべて回す
			for (int i : keyCommand_.at(com)) {

				infos.emplace_back(keyboard_->GetKey(i));
			}
		}
		// コマンドが登録されているか
		if (mouseCommand_.contains(com)) {
			// コマンドに紐づけられているキーをすべて回す
			for (int i : mouseCommand_.at(com)) {

				infos.emplace_back(mouse_->GetKey(i));
			}
		}
	}
	// パッド
	else {
		// コマンドに紐づけられているキーをすべて回す
		for (auto i : padCommand_.at(com)) {

			infos.emplace_back(pads_.at(static_cast<int>(no))->GetKey(i));
		}
	}
	// キーの情報を統合
	return GetKeyInfo(infos);
}

InputBase::Info InputManager::GetPriorityKey(const COMMAND& com)
{
	return GetKey(com, mostPriority_);
}

std::vector<InputBase::Info> InputManager::GetPriorityKey(const COMMAND& com, int& num)
{
	// 返り値を格納する変数
	std::vector<InputBase::Info> temp;

	// 優先順に回す
	for (int i = 0; i < num; i++) {
		// 優先順確認
		if (orderOfPriority_.at(i)) {
			// 各情報の取得
			switch (i)
			{
			case static_cast<int>(KEYPAD_NO::PAD1):
				temp.emplace_back(GetKey(com, KEYPAD_NO::PAD1));
				break;
			case static_cast<int>(KEYPAD_NO::PAD2):
				temp.emplace_back(GetKey(com, KEYPAD_NO::PAD2));
				break;
			case static_cast<int>(KEYPAD_NO::PAD3):
				temp.emplace_back(GetKey(com, KEYPAD_NO::PAD3));
				break;
			case static_cast<int>(KEYPAD_NO::PAD4):
				temp.emplace_back(GetKey(com, KEYPAD_NO::PAD4));
				break;
			case static_cast<int>(KEYPAD_NO::KEY):
				temp.emplace_back(GetKey(com, KEYPAD_NO::KEY));
				break;
			default:
				break;
			}
		}
	}
	// 少なくてもそのまま
	return temp;
}

bool InputManager::GetPriorityAnyoneTrg(void)
{
	// 最優先を確認し探す
	switch (mostPriority_)
	{
	case KEYPAD_NO::PAD1:
		return pads_.at(static_cast<int>(KEYPAD_NO::PAD1))->GetAnyoneTrg();
		break;
	case KEYPAD_NO::PAD2:
		return pads_.at(static_cast<int>(KEYPAD_NO::PAD2))->GetAnyoneTrg();
		break;
	case KEYPAD_NO::PAD3:
		return pads_.at(static_cast<int>(KEYPAD_NO::PAD3))->GetAnyoneTrg();
		break;
	case KEYPAD_NO::PAD4:
		return pads_.at(static_cast<int>(KEYPAD_NO::PAD4))->GetAnyoneTrg();
		break;
	case KEYPAD_NO::KEY:
		return keyboard_->GetAnyoneTrg() || mouse_->GetAnyoneTrg();
		break;
	default:
		return false;
		break;
	}
}

bool InputManager::GetPriorityAnyoneTrgDown(void)
{
	// 最優先を確認し探す
	switch (mostPriority_)
	{
	case KEYPAD_NO::PAD1:
		return pads_.at(static_cast<int>(KEYPAD_NO::PAD1))->GetAnyoneTrgDown();
		break;
	case KEYPAD_NO::PAD2:
		return pads_.at(static_cast<int>(KEYPAD_NO::PAD2))->GetAnyoneTrgDown();
		break;
	case KEYPAD_NO::PAD3:
		return pads_.at(static_cast<int>(KEYPAD_NO::PAD3))->GetAnyoneTrgDown();
		break;
	case KEYPAD_NO::PAD4:
		return pads_.at(static_cast<int>(KEYPAD_NO::PAD4))->GetAnyoneTrgDown();
		break;
	case KEYPAD_NO::KEY:
		return keyboard_->GetAnyoneTrgDown() || mouse_->GetAnyoneTrgDown();
		break;
	default:
		return false;
		break;
	}
}

InputManager::InputManager(void)
{
}

InputManager::~InputManager(void)
{
}

InputBase::Info InputManager::GetKeyInfo(std::vector<InputBase::Info> infos)
{
	// Vectorのものを統合
	for (InputBase::Info info : infos) {
		if (info.keyNew || info.keyOld) { return info; }
	}
	return infos.front();
}
