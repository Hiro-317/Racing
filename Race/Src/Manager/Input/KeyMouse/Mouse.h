#pragma once
#include <map>
#include <vector>
#include <Dxlib.h>
#include "../InputBase.h"

class Mouse : public InputBase
{
public:

	// マウス感度
	static constexpr float MOUSE_SENSI = 0.0004f;

	// コンストラクタ
	Mouse(void);
	// デストラクタ
	~Mouse(void) = default;

	// 初期化
	void Init(void) override;
	// 更新
	void Update(void) override;
	// リソースの破棄
	void Release(void) override;

	// 判定を行うキーをクリア
	void Clear(void) { infos_.clear(); }

	// マウス座標の取得
	Vector3 GetMousePos(void) const { return Vector3(mousePos_); }

private:

	// マウスカーソルの位置
	Vector3 mousePos_;

	// マウスボタンの入力状態
	int mouseInput_;

	// マウスの入力取得
	bool CheckMouse(int key);
};