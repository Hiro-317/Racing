#pragma once
#include <algorithm>
#include <functional>

#include "../../ActorBase/ActorBase.h"
#include "../../../Application/Application.h"

#include "../../../Manager/Input/InputManager.h"


class Meter;

class CarBase : public ActorBase {
	
public:

	CarBase(const Input::KEYPAD_NO& keypadNo, const Vector3& pos, const int pNo, const std::function<int(void)> camNo);
	~CarBase(void) = default;

	virtual void Load(void) override;

	const Vector3& GetMoveVec(void) { return moveVec; }

	void OnCollision(COLLIDER_TAG ownTag, const ColliderBase& other, const CollisionResult& result) override;

	const int GetPlayerNo(void) { return pNo; }

protected:

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

	/// <summary>
	/// 車速、ステア入力、アクセル入力から後輪の滑り量とグリップを更新する
	/// </summary>
	/// <param name="currentSpeed">現在の車速(km/h)</param>
	/// <param name="steerInput">現在のステア入力(-1～1)</param>
	/// <param name="throttleInput">現在のアクセル入力(0～1)</param>
	void UpdateRearGrip(float currentSpeed, float steerInput, float throttleInput);

	// 空ぶかしで上がる回転数
	static constexpr float UP_RPM = 125.0f;
	// 空ぶかしで下がる回転数
	static constexpr float DOWN_RPM = 200.0f;

	// 駆動力損失率
	static constexpr float LOST_RATIO = 0.9f;

	// 空気密度
	static constexpr float AIR_DENSITY = 1.20f;

	// エンジンブレーキ係数
	static constexpr float ENGINE_BRAKE = 0.0075f;

	// 後輪が滑り始める速度(km/h)
	static constexpr float REAR_SLIP_START_SPEED = 45.0f;
	// 後輪の滑り量が最大になる速度(km/h)
	static constexpr float REAR_SLIP_FULL_SPEED = 100.0f;
	// 通常時に移動方向を車体前方へ戻す強さ(1/秒)
	static constexpr float REAR_GRIP_MAX = 12.0f;
	// 滑走中にも残す最低グリップ(1/秒)
	static constexpr float REAR_GRIP_MIN = 1.5f;
	// 滑り量が目標値へ追従する速さ(1/秒)
	static constexpr float DRIFT_RESPONSE = 4.0f;

	// エンジン稼働状態
	bool engineFlg;

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

	// フューエルカット状態
	bool fuelCut;

	// ステア角度
	float nowSteer;
	float prevSteer;

	// 進む向き
	Vector3 moveVec;

	// 車体横方向の速度
	float sideSpeed;

	// 横方向のグリップ
	float grip;
	// ドリフト状態
	float driftAmount; 

	// 回転スピード
	float yawSpeed;

protected:

	// アイドリング回転数
	static constexpr float MIN_RPM = 1000.0f;

	// 各車のパラメーター構造体
	struct PRAMETER {

		// レブリミット
		float MAX_RPM;

		// 最高トルク(N-m)
		float MAX_TORQUE;
		// 最高トルクを出す回転数
		float MAX_TORQUE_RPM;

		// 立ち上がり幅
		float RISE_WIDTH;
		// 下降幅
		float FALL_WIDTH;

		// 最高ギアの段数
		int GEAR_NUM;
		// それぞれのギア比
		std::vector<float> GEAR_RATIO;
		// ファイナルギア比
		float FINAL_GEAR_RATIO;

		// タイヤ半径
		float TIRE_SIZE;

		// 車重
		float CAR_WEIGHT;
		// CD値
		float CD;

		// 前面投影面積
		float FPA;
		// 転がり抵抗係数
		float RR;
	};

	// パラメーター取得
	PRAMETER param;

	// 最低速度
	float idleSpeed;

	// 速度
	float speed;

	// 今のギア
	int nowGear;

	// 回転数
	float rpm;

	// この車の操作パッド
	const Input::KEYPAD_NO& keypadNo;

	// 中心で描画されるカメラ
	const std::function<int(void)> camNo;

	// 何Pか
	const int pNo;
};
