#pragma once
#include <map>
#include <vector>
#include <Dxlib.h>

#include "../../Common/Vector3.h"


class InputBase
{
public:

	// キー情報
	struct Info
	{
		int key = -1;				// キーID
		bool keyOld = false;		// 1フレーム前の押下状態
		bool keyNew = false;		// 現フレームの押下状態
		bool keyTrgDown = false;	// 現フレームでボタンが押されたか
		bool keyTrgUp = false;		// 現フレームでボタンが離されたか
	};

	// ゲームコントローラータイプ
	// DxLib定数、DX_OTHER等に対応
	enum class JOYPAD_TYPE
	{
		NON = -1,

		OTHER,
		XBOX_360,
		XBOX_ONE,
		DUAL_SHOCK_3,
		DUAL_SHOCK_4,
		DUAL_SENSE,
		SWITCH_JOY_CON_L,
		SWITCH_JOY_CON_R,
		SWITCH_PRO_CTRL,

		MAX
	};

	// コンストラクタ
	InputBase(void);
	// デストラクタ
	~InputBase(void) = default;

	// 初期化
	virtual void Init(void) = 0;
	// 更新
	virtual void Update(void) = 0;
	void Update(std::pair<const int, Info>& p);
	// リソースの破棄
	virtual void Release(void) = 0;

	// 判定を行うキーを追加
	void Add(int key);

	// キーがもう登録されているか
	bool Added(int key);

	// 判定を行うキーをクリア
	void Clear(void) { infos.clear(); }

	// キーの状態を取得
	Info GetKey(int key) const { return infos.at(key); }

	// 何かしているか
	bool GetAnyone(void) { return anyone; }
	// 何か押しているか
	bool GetAnyoneTrg(void) { return anyoneTrg; }
	// 今何か押されたか
	bool GetAnyoneTrgDown(void) { return anyoneTrgDown; }

protected:

	// キー情報
	std::map<int, Info> infos;

	// 今何か押されたか
	bool anyoneTrgDown;
	// 何か押されているか
	bool anyoneTrg;
	// 何かしているか
	bool anyone;
};