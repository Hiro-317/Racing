#include "CarBase.h"
#include <algorithm>

#include "../../Common/Collider/CapsuleCollider.h"


CarBase::CarBase(const Input::KEYPAD_NO& keypadNo, const Vector3& pos, const int pNo, const std::function<int(void)> camNo) :
	ActorBase(),
	engineFlg(false),
	torque(0.0f),
	torqueMax(0.0f),
	torqueMaxRpm(0.0f),
	hoursepower(0.0f),
	hoursepowerMax(0.0f),
	hoursepowerMaxRpm(0.0f),
	fuelCut(false),
	nowSteer(0.0f),
	prevSteer(0.0f),
	moveVec(Vector3::Zonly(1.0f)),
	sideSpeed(0.0f),
	grip(REAR_GRIP_MAX),
	driftAmount(0.0f),
	yawSpeed(0.0f),
	spinYawSpeed(0.0f),
	spinTorqueReleased(false),
	counterRecoveryRequired(false),
	idleSpeed(0.0f),
	speed(0.0f),
	nowGear(0),
	rpm(0.0f),
	keypadNo(keypadNo),
	camNo(camNo),
	pNo(pNo)
{
	trans.pos = pos;
}

void CarBase::Load(void)
{
	AddCollider(new CapsuleCollider(COLLIDER_TAG::Player, Vector3(0.0f, 80.0f, 150.0f), Vector3(0.0f, 80.0f, -150.0f), 80.0f));
}

void CarBase::OnCollision(COLLIDER_TAG ownTag, const ColliderBase& other, const CollisionResult& result)
{
}

void CarBase::SubUpdate(void)
{
	// ステア角度の保持
	prevSteer = nowSteer;
	nowSteer = Input::GetIns().GetAKeyLClamp(keypadNo).x;

	// エンジンオフなら
	if (!engineFlg) {
		// 回転数があるなら
		if (rpm > 0.0f) {
			//徐々に下げる
			rpm -= DOWN_RPM;
		}
		// ゼロ下回っちゃったら
		else if (rpm < 0.0f) {
			// ゼロにしてあげる
			rpm = 0.0f;
		}
		// ブレーキ踏みながらキーで
		if (Input::GetIns().GetKey(Input::COMMAND::ACTION, keypadNo).keyTrgDown && (Input::GetIns().GetKey(Input::COMMAND::SIDE, keypadNo).keyNew || Brake() == -1.0f)) {

			// アクセルオンで最低回転数にもっていく
			engineFlg = true;
			rpm = MIN_RPM;
		}
	}
	// エンジンオンなら
	else {
		// 今オーバートップ未満なら
		if (nowGear < param.GEAR_NUM - 1) {
			// ギアを上げた際最低回転数下回らないなら
			if (rpm * (param.GEAR_RATIO[nowGear + 1] / param.GEAR_RATIO[nowGear]) >= MIN_RPM) {
				// 下ボタン押すか最高回転数に行ったら
				if (Input::GetIns().GetKey(Input::COMMAND::DOWN, keypadNo).keyTrgDown || rpm >= param.MAX_RPM) {
					// ギアを上げ回転数を落とす
					rpm = rpm * (param.GEAR_RATIO[nowGear + 1] / param.GEAR_RATIO[nowGear]);
					nowGear++;
				}
			}
		}
		// 今セカンドギア以上なら
		if (nowGear > 0) {
			// ギアを下げた際最高回転数上回らないなら
			if (rpm * (param.GEAR_RATIO[nowGear - 1] / param.GEAR_RATIO[nowGear]) <= param.MAX_RPM) {
				// 上を押すか最低回転数に行ったら
				if (Input::GetIns().GetKey(Input::COMMAND::UP, keypadNo).keyTrgDown || rpm < MIN_RPM) {
					// ギアを落とし回転数を上げる
					rpm = rpm * (param.GEAR_RATIO[nowGear - 1] / param.GEAR_RATIO[nowGear]);
					nowGear--;
				}
			}
		}
		// その回転数のトルク出力を計算しておく
		GetTorque();

		// ブレーキ踏んでいて
		if (Brake() != 0.0f || Input::GetIns().GetKey(Input::COMMAND::SIDE, keypadNo).keyNew) {
			// 速度がゼロで
			if (speed == 0.0f) {
				// アクセルを踏んでいるとき
				if (Accel() != 0) {
					// フューエルカットが働いてないなら
					if (!fuelCut) {
						// 回転数を上げ
						rpm += UP_RPM * Accel();
						// ローンチの回転数に行ったら
						if (rpm > param.MAX_TORQUE_RPM) {
							// フューエルカットをかける
							fuelCut = true;
						}
					}
					// フューエルカットが働いているなら
					else {
						// 回転数を落としフューエルカットを落とす
						rpm -= DOWN_RPM;
						fuelCut = false;
					}
				}
				else {
					// 回転数が最少じゃないなら
					if (rpm > MIN_RPM) {
						// 回転数を落とす
						rpm -= DOWN_RPM;
					}
					// 最少回転数を下回っちゃったら
					if (rpm < MIN_RPM) {
						// 最少回転数にする
						rpm = MIN_RPM;
					}
				}
			}
			// 速度が出ているとき
			else {
				// エンジンブレーキとブレーキで速度を落としそこから回転数を出す
				if (Input::GetIns().GetKey(COMMAND::SIDE, keypadNo).keyNew) speed -= 0.5f;
				speed += 0.35f * Brake();
				speed += EngineBrake() * App::GetPassTime();
				rpm = (speed * param.GEAR_RATIO[nowGear] * param.FINAL_GEAR_RATIO * 60.0f) / (3.6f * 2.0f * DX_PI_F * param.TIRE_SIZE);
				// 速度がゼロ以下になったら
				if (speed <= 0.0f) {
					// 速度をゼロにする
					speed = 0.0f;
					// 最少回転数は下回っているはずだから最少回転数にする
					rpm = MIN_RPM;
				}
			}
		}
		// ブレーキ踏んでおらず最低速度を下回っているとき
		else if (idleSpeed > speed) {
			// その差分クラッチとし最低半クラを25%として
			float clatch = (1.0f - ((idleSpeed - speed) / idleSpeed));
			clatch =  std::clamp(clatch, 0.25f, 1.0f);
			float driveRate = clatch + ((1.0f - clatch) * Accel());

			// 加速
			speed += Acceleration(driveRate) * App::GetPassTime();
			// 速度からエンジン回転数を出し
			float spRpm = (speed * param.GEAR_RATIO[nowGear] * param.FINAL_GEAR_RATIO * 60.0f) / (3.6f * 2.0f * DX_PI_F * param.TIRE_SIZE);
			// 速度の方は全クラッチつながっているときの回転数になるから
			// エンジン回転数を1-クラッチ率、速度からの方をクラッチ率で足す
			rpm = rpm * (1.0f - clatch) + spRpm * clatch;
		}
		// 最低速度以上なら(クラッチ全つながりしてる)
		else {
			// アクセルオンかつフューエルカットが働いてないなら
			if (Accel() != 0.0f && !fuelCut) {
				// 加速
				speed += Acceleration(Accel()) * App::GetPassTime();
				// 回転数を速度から出す
				rpm = (speed * param.GEAR_RATIO[nowGear] * param.FINAL_GEAR_RATIO * 60.0f) / (3.6f * 2.0f * DX_PI_F * param.TIRE_SIZE);
				// レブリミットにかかったら
				if (rpm > param.MAX_RPM) {
					// フューエルカットをかける
					fuelCut = true;
				}
			}
			// アクセルオフかフューエルカットが働いているなら
			else {
				// エンブレかけて
				speed += EngineBrake() * App::GetPassTime();
				// 回転数を速度から出す
				rpm = (speed * param.GEAR_RATIO[nowGear] * param.FINAL_GEAR_RATIO * 60.0f) / (3.6f * 2.0f * DX_PI_F * param.TIRE_SIZE);
				// フューエルカットがかかってるなら
				if (fuelCut) {
					if (rpm < param.MAX_RPM) {
						// フューエルカットを落とす
						fuelCut = false;
					}
				}
			}
			// エンブレで最低速度を下回ったら
			if (speed < idleSpeed) {
				// 最低速度、最低回転数にする
				speed = idleSpeed;
				rpm = MIN_RPM;
			}
		}
	}

	float temp = nowSteer - prevSteer;
	nowSteer = prevSteer + temp * 0.3f;
	const float steerChange = fabsf(nowSteer - prevSteer);
	const float throttleInput = Accel();
	const float brakeInput = fabsf(Brake());
	const bool handBrake = Input::GetIns().GetKey(Input::COMMAND::SIDE, keypadNo).keyNew;

	if (speed != 0.0f) {
		// ブレーキ時の前荷重とサイドブレーキによる後輪ロックで、車体の向きを変えやすくする
		const float turnInRate = 1.0f + brakeInput * BRAKE_TURN_IN + (handBrake ? HANDBRAKE_TURN_IN : 0.0f);
		float targetYawSpeed = nowSteer * 2.0f * turnInRate;

		// 回転速度を滑らかに変化させる
		yawSpeed += (targetYawSpeed - yawSpeed) * 5.0f * App::GetPassTime();

		trans.AddAngle(Vector3::Yonly(yawSpeed * App::GetPassTime()));
	}

	MATRIX rot = trans.rotation.ToMatrix();
	float yaw = atan2f(rot.m[0][2], rot.m[2][2]);

	Vector3 forward = Vector3(-sinf(yaw), 0.0f, cosf(yaw));
	Vector3 right = Vector3(cosf(yaw), 0.0f, sinf(yaw));

	// 車体の向きと実際の移動方向を分離する。
	// 高速旋回中は移動方向の追従を遅らせ、車体だけが先に旋回することでリアを外側へ滑らせる。
	UpdateRearGrip(speed, nowSteer, steerChange, throttleInput, brakeInput, handBrake);
	const Vector3 currentDirection = moveVec.LengthSq() > 0.0f ? moveVec.Normalized() : forward;
	ApplySlipDynamics(forward, currentDirection, nowSteer);
	const float directionBlend = 1.0f - std::exp(-grip * App::GetPassTime());
	Vector3 nextDirection = currentDirection + (forward - currentDirection) * directionBlend;
	if (nextDirection.LengthSq() == 0.0f) { nextDirection = forward; }

	moveVec = nextDirection.Normalized() * speed;
	sideSpeed = moveVec.Dot(right);

	trans.pos += moveVec;

	// ここからデータロガー
	//
	// 最高トルク
#ifdef _DEBUG

	if (torqueMax < torque) { torqueMax = torque; torqueMaxRpm = rpm; }
	// 最高馬力
	hoursepower = (torque * rpm) / 9550.0f;
	if (hoursepowerMax < hoursepower) { hoursepowerMax = hoursepower; hoursepowerMaxRpm = rpm; }

#endif // _DEBUG
}

void CarBase::SubDraw(void)
{
	// ここからはデータロガー	
	if (App::GetIns().IsDrawDebug()) {

		if (pNo != camNo()) {
			return;
		}

		// 現在の回転数
		DrawFormatString(100, 20, 0xffffff, "%.2f", rpm);
		// 現在のトルクと最高トルクとその回転数
		DrawFormatString(200, 20, 0xffffff, "%.2f", torque);
		DrawFormatString(200, 40, 0xffffff, "%.2f", torqueMax);
		DrawFormatString(200, 60, 0xffffff, "%.2f", torqueMaxRpm);
		// 現在の馬力と最高馬力とその回転数
		DrawFormatString(300, 20, 0xffffff, "%.2f", hoursepower);
		DrawFormatString(300, 40, 0xffffff, "%.2f", hoursepowerMax);
		DrawFormatString(300, 60, 0xffffff, "%.2f", hoursepowerMaxRpm);
		// 現在の座標
		DrawFormatString(400, 20, 0xffffff, "%.2f : %.2f : %.2f", trans.pos.x, trans.pos.y, trans.pos.z);
		// 現在のステア角度と旋回速度
		DrawFormatString(700, 20, 0xffffff, "%.2f", nowSteer);
		DrawFormatString(700, 40, 0xffffff, "%.5f", yawSpeed);
		DrawFormatString(700, 60, 0xffffff, "%.5f", trans.rotation.y);
		DrawFormatString(700, 80, 0xffffff, "%.2f : %.2f : %.2f", moveVec.x, moveVec.y, moveVec.z);

		//DrawSphere3D((trans.pos + moveVec * 100.0f).ToVECTOR(), 200.0f, 32, 0xffffff, 0xfffffff, true);
	}
}

void CarBase::SubRelease(void)
{
}

void CarBase::GetTorque(void)
{
	// 最大トルク回転数からの距離
	float diff = rpm - param.MAX_TORQUE_RPM;

	// 最大トルクより低回転か、高回転かで幅を切り替える
	float width = (diff < 0.0f) ? param.RISE_WIDTH : param.FALL_WIDTH;

	width = (std::max)(width, 1.0f);

	// 最大トルクを頂点とする非対称曲線
	float x = diff / width;

	torque = param.MAX_TORQUE * std::exp(-0.5f * x * x);
}

float CarBase::Acceleration(float c)
{
	// エンジンパワーをトルク、ギア比、損失率、クラッチ率(アクセル開度)、タイヤサイズで出す
	float ep = (torque * param.GEAR_RATIO[nowGear] * param.FINAL_GEAR_RATIO * LOST_RATIO * c) / param.TIRE_SIZE;
	// エンジンパワーから抵抗を引いた分を車重で割って時速に戻す
	return (ep - AirResistance() - RollingResistance()) / param.CAR_WEIGHT * 3.6f;
}

float CarBase::EngineBrake(void)
{
	// エンジンブレーキ力を回転数、ギア比、エンブレ係数、タイヤサイズで出す
	float ep = (rpm * param.GEAR_RATIO[nowGear] * param.FINAL_GEAR_RATIO * ENGINE_BRAKE) / (param.TIRE_SIZE);
	// エンジンブレーキ力に抵抗を足し車重で割って時速に戻す
	return -(ep + AirResistance() + RollingResistance()) / param.CAR_WEIGHT * 3.6f;
}

float CarBase::AirResistance(void)
{
	// 空気抵抗を空気密度、CD値、前面投影面積、速度で出す
	return AIR_DENSITY * param.CD * param.FPA * (speed / 3.6f) * (speed / 3.6f) * 0.5f;
}

float CarBase::RollingResistance(void)
{
	// 転がり抵抗を出す
	return param.RR * 9.81f * param.CAR_WEIGHT;
}

float CarBase::Accel(void)
{
	if (keypadNo == Input::KEYPAD_NO::KEY) {
		if (Input::GetIns().GetKey(COMMAND::ACCEL, keypadNo).keyNew) {
			return 1.0f;
		}
	}
	else {
		return Input::GetIns().GetRTrigger(keypadNo) / 255.0f;
	}
	return 0.0f;
}

float CarBase::Brake(void)
{
	if (keypadNo == Input::KEYPAD_NO::KEY) {
		if (Input::GetIns().GetKey(COMMAND::BRAKE, keypadNo).keyNew) {
			return -1.0f;
		}
	}
	else {
		return -Input::GetIns().GetLTrigger(keypadNo) / 255.0f;
	}
	return 0.0f;
}

void CarBase::UpdateRearGrip(float currentSpeed, float steerInput, float steerChange, float throttleInput, float brakeInput, bool handBrake)
{
	// 滑り始める速度から最大になる速度までを0～1へ正規化する
	const float speedRange = REAR_SLIP_FULL_SPEED - REAR_SLIP_START_SPEED;
	const float speedRate = std::clamp(
		(fabsf(currentSpeed) - REAR_SLIP_START_SPEED) / speedRange,
		0.0f,
		1.0f
	);

	// 入力値を安全な範囲へ収め、左右どちらの旋回でも同じ滑り量として扱う
	const float steerRate = std::clamp(fabsf(steerInput), 0.0f, 1.0f);
	const float steerChangeRate = std::clamp(steerChange * 4.0f, 0.0f, 1.0f);
	const float throttleRate = std::clamp(throttleInput, 0.0f, 1.0f);
	const float brakeRate = std::clamp(brakeInput, 0.0f, 1.0f);

	// 高速旋回ではアクセルに関係なく限界へ近づくが、単独では急激なスピンまで発展させない
	const float cornerDriftAmount = speedRate * steerRate * CORNER_DRIFT_POWER;

	// パワースライドはアクセル開度を直接反映し、アクセルオフでは発生させない
	const float powerSpeedRange = POWER_SLIP_FULL_SPEED - POWER_SLIP_START_SPEED;
	const float powerSpeedRate = std::clamp(
		(fabsf(currentSpeed) - POWER_SLIP_START_SPEED) / powerSpeedRange,
		0.0f,
		1.0f
	);
	const float powerDriftAmount =
		powerSpeedRate * steerRate * throttleRate * POWER_DRIFT_POWER;

	// 通常ブレーキでは前荷重でリアが軽くなるため、切り返しが速いほど滑りやすくする
	const float brakeSpeedRange = REAR_SLIP_FULL_SPEED - BRAKE_SLIP_START_SPEED;
	const float brakeSpeedRate = std::clamp(
		(fabsf(currentSpeed) - BRAKE_SLIP_START_SPEED) / brakeSpeedRange,
		0.0f,
		1.0f
	);
	const float brakeDriftAmount = brakeSpeedRate * brakeRate * steerRate *
		(BRAKE_DRIFT_POWER + steerChangeRate * (1.0f - BRAKE_DRIFT_POWER));

	// サイドブレーキは低い車速から後輪グリップを失わせ、急な切り返しでは最大まで滑らせる
	const float handBrakeDriftAmount = handBrake ?
		brakeSpeedRate * HANDBRAKE_DRIFT_POWER * (0.4f + (std::max)(steerRate, steerChangeRate) * 0.6f) :
		0.0f;

	// 同時に成立した要因のうち最も強いものを採用し、滑り量が過剰に加算されないようにする
	const float targetDriftAmount = (std::max)(
		{ cornerDriftAmount, powerDriftAmount, brakeDriftAmount, handBrakeDriftAmount }
	);

	// フレーム時間を使って滑り始めとグリップ回復を滑らかにする
	const float response = std::clamp(DRIFT_RESPONSE * App::GetPassTime(), 0.0f, 1.0f);
	driftAmount += (targetDriftAmount - driftAmount) * response;

	// 滑り量に応じて、移動方向を車体前方へ戻す力を弱める
	grip = REAR_GRIP_MAX + (REAR_GRIP_MIN - REAR_GRIP_MAX) * driftAmount;
}

void CarBase::ApplySlipDynamics(const Vector3& forward, const Vector3& moveDirection, float steerInput)
{
	// 内積と外積から、車体前方に対する移動方向の符号付き角度を求める
	const float directionDot = std::clamp(forward.Dot(moveDirection), -1.0f, 1.0f);
	const float directionCross = forward.Cross(moveDirection).y;
	const float slipAngle = atan2f(directionCross, directionDot);
	const float absSlipAngle = fabsf(slipAngle);

	// 横向きまで回ったらタイヤが生む追加の回転力を止め、以降は回転慣性だけで滑らせる
	if (absSlipAngle >= SPIN_FULL_ANGLE) { spinTorqueReleased = true; }

	// 車体と移動方向が揃い、回転も十分に収まったときだけ次のスピンに備えて状態を戻す
	if (spinTorqueReleased && absSlipAngle <= SPIN_RESET_ANGLE && fabsf(spinYawSpeed) <= SPIN_RESET_YAW_SPEED) {
		spinTorqueReleased = false;
	}

	// 20度を越えてから急激に回転力を立ち上げ、GT7動画のスナップオーバーステアへ近づける
	const float spinAngleRange = SPIN_FULL_ANGLE - SPIN_START_ANGLE;
	const float spinLinearRate = std::clamp(
		(absSlipAngle - SPIN_START_ANGLE) / spinAngleRange,
		0.0f,
		1.0f
	);
	const float spinRate = spinTorqueReleased ? 0.0f :
		spinLinearRate * spinLinearRate * (3.0f - 2.0f * spinLinearRate);

	// スピン開始後は現在の回転方向を維持し、角度が180度を超えた際の符号反転を防ぐ
	const float spinDirection = fabsf(spinYawSpeed) > 0.001f ?
		(spinYawSpeed >= 0.0f ? 1.0f : -1.0f) :
		(slipAngle >= 0.0f ? 1.0f : -1.0f);

	// 滑り方向と逆のステアだけをカウンターとして0～1へ変換する
	const float counterSteerInput = std::clamp(-steerInput * spinDirection, 0.0f, 1.0f);

	// 滑り角が大きいほど前輪が進行方向を変えられなくなるため、70度でカウンター効果を失わせる
	const float counterFadeRange = COUNTER_STEER_FADE_END - COUNTER_STEER_FADE_START;
	const float counterFadeRate = std::clamp(
		(absSlipAngle - COUNTER_STEER_FADE_START) / counterFadeRange,
		0.0f,
		1.0f
	);
	const float counterSteerRate = counterSteerInput * (1.0f - counterFadeRate);

	// 限界角を超えたら、ステアを戻しただけでは解除されない滑走状態へ移行する
	if (absSlipAngle >= SPIN_START_ANGLE) { counterRecoveryRequired = true; }

	// 車体と移動方向が揃った場合、または停止した場合に滑走状態を解除する
	const bool recovered = absSlipAngle <= SPIN_RESET_ANGLE &&
		fabsf(spinYawSpeed) <= SPIN_RESET_YAW_SPEED;
	const bool stopped = fabsf(speed) <= 1.0f && fabsf(spinYawSpeed) <= SPIN_RESET_YAW_SPEED;
	if (recovered || stopped) { counterRecoveryRequired = false; }

	if (counterRecoveryRequired) {
		// ニュートラルでは低いグリップでゆっくり収束し、カウンター入力に応じて回復を早める
		grip = NEUTRAL_SLIDE_GRIP +
			(REAR_GRIP_MAX - NEUTRAL_SLIDE_GRIP) * counterSteerRate;
	}
	else {
		// 限界前は滑り角に応じて通常グリップから最低グリップまで落とす
		const float slipGripRate = std::clamp(absSlipAngle / SPIN_START_ANGLE, 0.0f, 1.0f);
		const float slipGrip = REAR_GRIP_MAX + (REAR_GRIP_MIN - REAR_GRIP_MAX) * slipGripRate;
		grip = (std::min)(grip, slipGrip);
	}

	// カウンターを当てない場合は滑っている側へ回転を育て、一回転するスピンへ移行させる
	spinYawSpeed += spinDirection * SPIN_ACCELERATION * spinRate *
		(1.0f - counterSteerRate) * App::GetPassTime();

	// カウンター中は現在のスピン回転を直接弱め、追加の減衰で車体を収束させる
	if (spinYawSpeed * spinDirection > 0.0f) {
		const float counterBrake = COUNTER_STEER_BRAKE * counterSteerRate * App::GetPassTime();
		const float brakeAmount = (std::min)(fabsf(spinYawSpeed), counterBrake);
		spinYawSpeed -= spinDirection * brakeAmount;
	}
	// 横向き以降は追加回転力の代わりに減衰を増やし、惰性で半回転前後してから収束させる
	const float postLimitDamping = spinTorqueReleased ? POST_LIMIT_SPIN_DAMPING : 0.0f;
	const float spinDamping = SPIN_DAMPING + postLimitDamping + COUNTER_STEER_DAMPING * counterSteerRate;
	spinYawSpeed /= 1.0f + spinDamping * App::GetPassTime();
	trans.AddAngle(Vector3::Yonly(spinYawSpeed * App::GetPassTime()));

	// 横滑りとスピン中のタイヤ抵抗で速度を失わせ、アクセル全開でも加速し続ける状態を防ぐ
	const float slipRate = std::clamp(absSlipAngle / SPIN_FULL_ANGLE, 0.0f, 1.0f);
	const float spinSpeedRate = std::clamp(fabsf(spinYawSpeed) / SPIN_ACCELERATION, 0.0f, 1.0f);
	const float speedLossRate = slipRate * slipRate * SLIP_SPEED_LOSS + spinSpeedRate * SPIN_SPEED_LOSS;
	const float speedLoss = fabsf(speed) * speedLossRate * App::GetPassTime();

	// 現在は前進のみなので、減速によって速度が負にならないようにする
	speed = (std::max)(0.0f, speed - speedLoss);
}
