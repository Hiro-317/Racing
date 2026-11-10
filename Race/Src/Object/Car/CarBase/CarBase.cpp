#include "CarBase.h"
#include <algorithm>

#include "../../Common/Collider/CapsuleCollider.h"


CarBase::CarBase(const Input::KEYPAD_NO& keypadNo, const Vector3& pos, const int pNo, const std::function<int(void)> camNo) :
	ActorBase(),
	engineFlg(false),
	rpm(),
	nowGear(0),
	grip(1.0f),
	driftAmount(0.0f),
	keypadNo(keypadNo),
	camNo(camNo),
	pNo(pNo)
{
	moveVec = Vector3::Zonly(1.0f);

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
		if (Input::GetIns().GetKey(Input::COMMAND::ACTION, keypadNo).keyTrgDown && Input::GetIns().GetKey(Input::COMMAND::SIDE, keypadNo).keyNew) {

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
			float clatch = (1.0f - ((idleSpeed - speed) / idleSpeed)) + 0.25f;
			clatch =  std::clamp(clatch, 0.0f, 1.0f);

			// 加速
			speed += Acceleration(clatch) * App::GetPassTime();
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
	nowSteer = prevSteer + temp * 0.25f;

	// 旋回によって生じる横速度の目標値
	float targetSideSpeed = nowSteer * fabsf(speed) * 0.1f;

	// 横速度を目標値に近づける
	sideSpeed += (targetSideSpeed - sideSpeed) * App::GetPassTime();

	if (speed != 0.0f) {
		float targetYawSpeed = nowSteer * 2.0f;

		// 回転速度を滑らかに変化させる
		yawSpeed += (targetYawSpeed - yawSpeed) * 5.0f * App::GetPassTime();

		trans.AddAngle(Vector3::Yonly(yawSpeed * App::GetPassTime()));
	}

	MATRIX rot = trans.rotation.ToMatrix();
	float yaw = atan2f(rot.m[0][2], rot.m[2][2]);

	Vector3 forward = Vector3(-sinf(yaw), 0.0f, cosf(yaw));
	Vector3 right = Vector3(cosf(yaw), 0.0f, sinf(yaw));

	moveVec = forward * speed + right * sideSpeed;

	trans.pos += moveVec;

	float cameraSideFactor = std::clamp(fabsf(speed) / 20.0f, 0.0f, 1.0f);

	moveVec = forward * speed + right * sideSpeed * cameraSideFactor;

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
		if (Input::GetIns().GetAKeyLClamp(keypadNo).z > 0.0f) {
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
		if (Input::GetIns().GetAKeyLClamp(keypadNo).z < 0.0f) {
			return -1.0f;
		}
	}
	else {
		return -Input::GetIns().GetLTrigger(keypadNo) / 255.0f;
	}
	return 0.0f;
}
