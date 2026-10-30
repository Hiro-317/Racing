#pragma once
#include <algorithm>

#include "../ActorBase/ActorBase.h"
#include "../../Application/Application.h"

#include "../../Manager/Input/InputManager.h"

class CarBase : public ActorBase {
	
public:

	CarBase(const Input::KEYPAD_NO& no, const Vector3& pos);
	~CarBase(void);

	void Load(void) override;

	const Vector3& GetMoveVec(void) { return moveVec; }

	void OnCollision(COLLIDER_TAG ownTag, const ColliderBase& other, const CollisionResult& result) override;

protected:

	void SubInit(void) override;
	void SubUpdate(void) override;
	void SubDraw(void) override;
	void SubRelease(void) override;

	void VelocityUpdate(bool deceleration) override {}

private:

	// トルク計算
	void GetTorque(void);

	// 加速演算
	float Acceleration(float c = 1.0f);
	// エンブレ演算
	float EngineBrake(void);
	// 空気抵抗
	float AirResistance(void);
	// 転がり抵抗
	float RollingResistance(void);

	// 0~1
	float Accel(void);
	// 0~-1
	float Brake(void);

	// 針の長さ
	static constexpr int NEEDLE_LENGTH = 140;
	// 数字のみ書く場所の広さ
	static constexpr int BLANK_LENGTH = 130;

	// アイドリング回転数
	static constexpr float MIN_RPM = 1000.0f;
	// レブリミット
	static constexpr float MAX_RPM = 8500.0f;
	// レッドゾーン
	static constexpr int RED_ZONE = 85;
	// 空ぶかしで上がる回転数
	static constexpr float UP_RPM = 125.0f;
	// 空ぶかしで下がる回転数
	static constexpr float DOWN_RPM = 200.0f;

	// 最高トルク(N-m)
	static constexpr float MAX_TORQUE = 540.0f;
	// 最高トルクを出す回転数
	static constexpr float MAX_TORQUE_RPM = 6500.0f;

	// 立ち上がり幅
	float RISE_WIDTH = 3400.0f;
	// 下降幅
	float FALL_WIDTH = 3750.0f;

	// 最高ギアの段数
	static constexpr int GEAR_NUM = 7;
	// それぞれのギア比
	static constexpr float GEAR_RATIO[GEAR_NUM]{
		/*3.571f,
		2.227f,
		1.529f,
		1.150f,
		0.869f,
		0.686f*/

		3.909f,
		2.438f,
		1.810f,
		1.458f,
		1.185f,
		0.967f,
		0.844f
	};
	// ファイナルギア比
	static constexpr float FINAL_GEAR_RATIO = 3.05f;

	// 駆動力損失率
	static constexpr float LOST_RATIO = 0.9f;

	// タイヤ半径
	static constexpr float TIRE_SIZE = 0.203f;

	// タコメーターの位置
	const Vector2 TACHOMETER_POS = Vector2(App::GetIns().GetScreenSizeXHalf(), App::GetIns().GetScreenSizeY() - 100);
	// タコメーターの最大スケール
	static constexpr float MAX_SCALE = 10000.0f;

	// 針の0時の角度(ラジアン)
	const float NEEDLE_MIN_RAD = Deg2Rad(205.0f);
	// 針のスケール最大時の角度(ラジアン)
	const float NEEDLE_MAX_RAD = Deg2Rad(-25.0f);
	// 針移動の偏差
	const float SLOP = (NEEDLE_MAX_RAD - NEEDLE_MIN_RAD) / MAX_SCALE;

	// 車重
	static constexpr float CAR_WEIGHT = 1389.0f;
	// CD値
	static constexpr float CD = 0.38f;
	// 空気密度
	static constexpr float AIR_DENSITY = 1.20f;
	// 前面投影面積
	static constexpr float FPA = 1.74f;
	// 転がり抵抗係数
	static constexpr float RR = 0.011f;
	// エンジンブレーキ係数
	static constexpr float ENGINE_BRAKE = 0.0075f;

	// エンジン稼働状態
	bool engineFlg;

	// 針先の位置
	Vector2 needlePos;

	// 回転数
	float rpm;

	// トルク
	float torque;
	// 実測値の最大トルク
	float torqueMax;
	// 実測値の最大トルク回転数
	float torqueMaxRpm;

	// 馬力
	float hoursepower;
	// 実測値の最大馬力
	float hoursepowerMax;
	// 実測値の最大馬力回転数
	float hoursepowerMaxRpm;

	// 速度
	float speed;
	// 最低速度
	float idleSpeed;

	// フューエルカット状態
	bool fuelCut;

	// 今のギア
	int nowGear;

	// ステア角度
	float nowSteer;
	float prevSteer;

	// 進む向き
	Vector3 moveVec;
	Vector3 forward;

	// 車体横方向の速度
	float sideSpeed;

	// 横方向のグリップ
	float grip;
	// ドリフト状態
	float driftAmount; 

	// 回転スピード
	float yawSpeed;

	// この車の操作パッド
	const Input::KEYPAD_NO& KeypadNo;
};