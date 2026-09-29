#pragma once
#include <Dxlib.h>
#include <map>
#include <list>
#include <string>
#include "Controller/Controller.h"
#include "KeyMouse/Keyboard.h"
#include "KeyMouse/Mouse.h"


class Controller;
class Keyboard;
class Mouse;

class InputManager{

public:

	// ここに登録
	// コマンド
	enum class COMMAND {

		NON = -1,

		RUN,
		ATTACK,
		COMBO,
		DODGE,
		LOCK_ON,
		USE,
		LEFT,
		RIGHT,
		UP,
		DOWN,
		PAUSE,
		DECIDE,
		CANCEL,

		MAX,
	};

	// キーボード,ゲームコントローラー認識番号
	enum class KEYPAD_NO
		{
		NON = -1,

		PAD1,				// パッド１入力
		PAD2,				// パッド２入力
		PAD3,				// パッド３入力
		PAD4,				// パッド４入力
		KEY,				// キー入力
		MOUSE,				// マウス入力

		MAX,
	};

	// インスタンスを明示的に生成
	static void CreateIns(void);
	// インスタンスの取得
	static InputManager& GetIns(void);
	// インスタンスの破棄
	static void DeleteIns(void) { inst_->Release(); delete inst_; inst_ = nullptr; }

	// 初期化
	void Init(void);
	// 更新
	void Update(void);

	// 方向を取得
	Vector3 GetAKeyL(const KEYPAD_NO& no);
	Vector3  GetAKeyR(const KEYPAD_NO& no);

	// 特定機種ののコマンドの状態を取得
	// マウスはKEYで
	InputBase::Info GetKey(const COMMAND& com, const KEYPAD_NO& no);

	// 優先順に一つ取得
	InputBase::Info GetPriorityKey(const COMMAND& com);
	Vector3 GetPriorityAKeyL(void);
	Vector3 GetPriorityAKeyR(void);
	// 指定数(優先機種を0から)
	std::vector<InputBase::Info> GetPriorityKey(const COMMAND& com, int& num);
	std::vector<Vector3> GetPriorityAKeyL(const COMMAND& com, int& num);
	std::vector<Vector3> GetPriorityAKeyR(const COMMAND& com, int& num);

	// 最優先されたタイプを取得
	KEYPAD_NO GetMostPriority(void) { return mostPriority_; }
	Controller::JOYPAD_TYPE GetMostPriorityType(void) { return mostPriorityType_; }

	// (今のフレームで)最優先の物が何か押しているか
	bool GetPriorityAnyoneTrg(void);
	// (今のフレームで)最優先の物が今何か押したか
	bool GetPriorityAnyoneTrgDown(void);

	// キーの登録
	// パッドは1のみでいいよ
	void SetCommand(const COMMAND& com, const int& key, const KEYPAD_NO& type) 
	{
		// コマンド登録が済んでいるかどうかフラグ
		bool flg = false;
		
		switch (type)
		{
		
		// パッドの登録
		case InputManager::KEYPAD_NO::PAD1:
		case InputManager::KEYPAD_NO::PAD2:
		case InputManager::KEYPAD_NO::PAD3:
		case InputManager::KEYPAD_NO::PAD4:
		{
			// コマンド登録されているか
			if (padCommand_.find(com) != padCommand_.end()) {
				//すでに入っているキーは何か
				for (int i = 0; i < padCommand_[com].size(); i++) {
					//同じキーが入っていればフラグを立てる
					if (padCommand_[com][i] == key) {
						flg = true;
					}
				}
			}
			// コマンド登録されてないか同じキーでないなら登録
			if (padCommand_.find(com) == padCommand_.end() || !flg) {
				padCommand_[com].emplace_back(key);
			}

			// 更新用キーが登録されているか(一回でいいため)
			if (!pads_.at((int)type)->Added(key)) {
				// されていないなら登録
				for (int i = 0; i < (int)KEYPAD_NO::PAD4; i++) {
					pads_[i]->Add(key);
				}
			}
			break;
		}

		// キーボードの登録
		case InputManager::KEYPAD_NO::KEY:
		{
			// コマンド登録されているか
			if (keyCommand_.find(com) != keyCommand_.end()) {
				//すでに入っているキーは何か
				for (int i = 0; i < keyCommand_[com].size(); i++) {
					//同じキーが入っていればフラグを立てる
					if (keyCommand_[com][i] == key) {
						flg = true;
					}
				}
			}
			// コマンド登録されてないか同じキーでないなら登録
			if (keyCommand_.find(com) == keyCommand_.end() || !flg) {
				keyCommand_[com].emplace_back(key);
			}

			// 更新用キーが登録されているか(一回でいいため)
			if (!keyboard_->Added(key)) {
				// されていないなら登録
				keyboard_->Add(key);
			}
			break;
		}

		// マウスの登録
		case InputManager::KEYPAD_NO::MOUSE:
		{
			// コマンド登録されているか
			if (mouseCommand_.find(com) != mouseCommand_.end()) {
				//すでに入っているキーは何か
				for (int i = 0; i < mouseCommand_[com].size(); i++) {
					//同じキーが入っていればフラグを立てる
					if (mouseCommand_[com][i] == key) {
						flg = true;
					}
				}
			}
			// コマンド登録されてないか同じキーでないなら登録
			if (mouseCommand_.find(com) == mouseCommand_.end() || !flg) {
				mouseCommand_[com].emplace_back(key);
			}

			// 更新用キーが登録されているか(一回でいいため)
			if (!mouse_->Added(key)) {
				// されていないなら登録
				mouse_->Add(key);
			}
		}
		break;
		default:
			break;
		}
	}

	// コマンド再設定前のリセット
	void CommandClear() {
	
		// コマンドの消去
		padCommand_.clear();
		keyCommand_.clear();
		mouseCommand_.clear();

		// 更新用登録の消去
		for (int i = 0; i < (int)KEYPAD_NO::PAD4; i++) {
			pads_[i]->Clear();
		}
		keyboard_->Clear();
		mouse_->Clear();
	}

private:

	InputManager(void);
	~InputManager(void);

	// コピー・ムーブ操作を禁止
	InputManager(const InputManager&) = delete;
	InputManager& operator=(const InputManager&) = delete;
	InputManager(InputManager&&) = delete;
	InputManager& operator=(InputManager&&) = delete;

	// リソースの破棄
	void Release(void);

	// コマンドロード
	void CommandLoad(void);

	// コマンドの複数の押下判定の統合
	InputBase::Info GetKeyInfo(std::vector<InputBase::Info> infos);

	//インスタンス
	static InputManager* inst_;

	// パッド情報
	std::vector<Controller*> pads_;

	// キーマウ情報
	Keyboard* keyboard_;
	Mouse* mouse_;

	// キーマウコマンド
	std::map<InputManager::COMMAND, std::vector<int>> keyCommand_;
	std::map<InputManager::COMMAND, std::vector<int>> mouseCommand_;
	//パッドコマンド
	std::map<InputManager::COMMAND, std::vector<int>> padCommand_;

	// 優先
	std::vector<bool> orderOfPriority_;
	KEYPAD_NO mostPriority_;
	Controller::JOYPAD_TYPE mostPriorityType_;
};

using Input = InputManager;