#pragma once

#include "../Common/Utility.h"

// クラスの前方宣言
class FPS;

class Application
{
public:

	// 全体スクリーンサイズ
	static constexpr int DEFAULT_SIZE_X = 1920;	// スクリーン横幅;
	static constexpr int DEFAULT_SIZE_Y = 1080;	// スクリーン横幅;
	static constexpr int DEFAULT_SIZE_X_HALF = DEFAULT_SIZE_X / 2;	// スクリーン横幅;
	static constexpr int DEFAULT_SIZE_Y_HALF = DEFAULT_SIZE_Y / 2;	// スクリーン横幅;

	// エフェクト最大数
	static constexpr int EFFECT_MAX_NUM = 8000;

	// FPS
	static constexpr int FRAME_PAR_SECOND = 60;

	// シングルトン（生成・取得・削除）
	static void CreateIns(void) { if (ins == nullptr) { ins = new Application(); ins->Init(); } }
	static Application& GetIns(void) { return *ins; }
	static void DeleteIns(void) { if (ins != nullptr) delete ins; ins = nullptr; }

	void Init(void);	// 初期化
	void Run(void);		// ゲームループの開始
	void Release(void);	// 解放

	// 初期化成功／失敗の判定
	bool IsInitFail(void) const { return isInitFail; };

	// 解放成功／失敗の判定
	bool IsReleaseFail(void) const { return isReleaseFail; };

	// 現在のスクリーンサイズの変更
	void SetScreenSize(int x, int y);

	// ゲーム終了
	void GameEnd(void) { gameEnd = true; }

	// デバッグ描画（true = 「描画する」、false = 「描画しない」）
	bool IsDrawDebug(void)const { return drawDebug; }
	// デバッグ描画の設定（true = 「描画する」に設定、false = 「描画しない」に設定）
	void SetDrawDebug(bool flg) { drawDebug = flg; }
	// デバッグ描画の切り替え（現在の逆に切り替える）
	void DrawDebugSwitch(void) { drawDebug = !drawDebug; }

	int GetScreenSizeX(void){ return ScreenSizeX; }
	int GetScreenSizeY(void){ return ScreenSizeY; }
	int GetScreenSizeXHalf(void){ return ScreenSizeXHalf; }
	int GetScreenSizeYHalf(void){ return ScreenSizeYHalf; }

	static float GetPassTime(void);

private:
	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	Application(void);

	// デストラクタも同様
	~Application(void);

	// コピー・ムーブ操作を禁止
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
	Application(Application&&) = delete;
	Application& operator=(Application&&) = delete;

	// 下記をコンパイルエラーさせるため 上記を追加
	// Application copy = *Application::GetInstance();
	// Application copied(*Application::GetInstance());
	// Application moved = std::move(*Application::GetInstance());

	// 静的インスタンス
	static Application* ins;

	// 初期化失敗
	bool isInitFail;

	// 解放失敗
	bool isReleaseFail;

	// ゲーム終了
	bool gameEnd;

	// FPS
	FPS* fps;

	// デバッグ描画（true = 「描画する」、false = 「描画しない」）
	bool drawDebug;

	int ScreenSizeX;	// スクリーン横幅;
	int ScreenSizeY;	// スクリーン縦幅;

	// スクリーンの真ん中
	int ScreenSizeXHalf;
	int ScreenSizeYHalf;

};

// Applicationの省略形
using App = Application; 