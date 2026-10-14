#include "CarBase.h"

#include "../../Manager/Input/InputManager.h"


CarBase::CarBase(void) : 
	ActorBase(), engineFlg(false), rpm(), needlePos(TACHOMETER_POS.x + NEEDLE_LENGTH * cosf(NEEDLE_MIN_RAD), TACHOMETER_POS.y + NEEDLE_LENGTH * -sinf(NEEDLE_MIN_RAD)), nowGear(0)
{
	// この車体のアイドリング時の最低速度
	idleSpeed = (MIN_RPM * 3.6f * 2.0f * DX_PI_F * TIRE_SIZE) / (GEAR_RATIO[0] * FINAL_GEAR_RATIO * 60.0f);
	// 焦点距離最初に設定
	focusPos = trans.pos + FOCUS_POINT.TransMat({ trans.rotation.ToMatrix()});
}

CarBase::~CarBase(void)
{
}

void CarBase::Load(void)
{
	trans.LoadModel("Car/kari");
}

void CarBase::SubInit(void)
{
}

void CarBase::SubUpdate(void)
{
	// 焦点距離更新
	focusPos = trans.pos + FOCUS_POINT.TransMat({ trans.rotation.ToMatrix()});

	// ステア角度の保持
	prevSteer = nowSteer;
	nowSteer = Input::GetIns().GetPriorityAKeyL().x;

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
		if (Input::GetIns().GetPriorityKey(Input::COMMAND::USE).keyTrgDown && Input::GetIns().GetPriorityKey(Input::COMMAND::RUN).keyNew) {

			// アクセルオンで最低回転数にもっていく
			engineFlg = true;
			rpm = MIN_RPM;
		}
	}
	// エンジンオンなら
	else {
		// 今オーバートップ未満なら
		if (nowGear < GEAR_NUM - 1) {
			// ギアを上げた際最低回転数下回らないなら
			if (rpm * (GEAR_RATIO[nowGear + 1] / GEAR_RATIO[nowGear]) >= MIN_RPM) {
				// 下ボタン押すか最高回転数に行ったら
				if (Input::GetIns().GetPriorityKey(Input::COMMAND::DOWN).keyTrgDown) {
					// ギアを上げ回転数を落とす
					rpm = rpm * (GEAR_RATIO[nowGear + 1] / GEAR_RATIO[nowGear]);
					nowGear++;
				}
			}
		}
		// 今セカンドギア以上なら
		if (nowGear > 0) {
			// ギアを下げた際最高回転数上回らないなら
			if (rpm * (GEAR_RATIO[nowGear - 1] / GEAR_RATIO[nowGear]) <= MAX_RPM) {
				// 上を押すか最低回転数に行ったら
				if (Input::GetIns().GetPriorityKey(Input::COMMAND::UP).keyTrgDown || rpm <= MIN_RPM) {
					// ギアを落とし回転数を上げる
					rpm = rpm * (GEAR_RATIO[nowGear - 1] / GEAR_RATIO[nowGear]);
					nowGear--;
				}
			}
		}
		// その回転数のトルク出力を計算しておく
		torque = MAX_TORQUE * powf(rpm / MAX_TORQUE_RPM, TORQUE_CURVE) * expf(TORQUE_CURVE * (1 - rpm / MAX_TORQUE_RPM));
		// ブレーキ踏んでいて
		if (Input::GetIns().GetPriorityAKeyL().z < 0.0f || Input::GetIns().GetPriorityKey(Input::COMMAND::RUN).keyNew) {
			// 速度がゼロで
			if (speed == 0.0f) {
				// アクセルを踏んでいるとき
				if (Input::GetIns().GetPriorityAKeyL().z > 0) {
					// フューエルカットが働いてないなら
					if (!fuelCut) {
						// 回転数を上げ
						rpm += UP_RPM * Input::GetIns().GetPriorityAKeyL().z;
						// ローンチの回転数に行ったら
						if (rpm > MAX_TORQUE_RPM) {
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
				speed += 0.25f * Input::GetIns().GetPriorityAKeyL().z;
				speed += EngineBrake() / Application::FRAME_PAR_SECOND;
				rpm = (speed * GEAR_RATIO[nowGear] * FINAL_GEAR_RATIO * 60.0f) / (3.6f * 2.0f * DX_PI_F * TIRE_SIZE);
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
			float clatch = 1.0f - (idleSpeed - speed) / idleSpeed + 0.25f;
			if (clatch > 1.0f) {
				clatch = 1.0f;
			}
			// 加速
			speed += Acceleration(clatch)/ Application::FRAME_PAR_SECOND;
			// 速度からエンジン回転数を出し
			float spRpm = (speed * GEAR_RATIO[nowGear] * FINAL_GEAR_RATIO * 60.0f) / (3.6f * 2.0f * DX_PI_F * TIRE_SIZE);
			// 速度の方は全クラッチつながっているときの回転数になるから
			// エンジン回転数を1-クラッチ率、速度からの方をクラッチ率で足す
			rpm = rpm * (1.0f - clatch) + spRpm * clatch;
		}
		// 最低速度以上なら(クラッチ全つながりしてる)
		else {
			// アクセルオンかつフューエルカットが働いてないなら
			if (Input::GetIns().GetPriorityAKeyL().z != 0.0f && !fuelCut) {
				// 加速
				speed += Acceleration(Input::GetIns().GetPriorityAKeyL().z)/ Application::FRAME_PAR_SECOND;
				// 回転数を速度から出す
				rpm = (speed * GEAR_RATIO[nowGear] * FINAL_GEAR_RATIO * 60.0f) / (3.6f * 2.0f * DX_PI_F * TIRE_SIZE);
				// レブリミットにかかったら
				if (rpm > MAX_RPM) {
					// フューエルカットをかける
					fuelCut = true;
				}
			}
			// アクセルオフかフューエルカットが働いているなら
			else {
				// エンブレかけて
				speed += EngineBrake() / Application::FRAME_PAR_SECOND;
				// 回転数を速度から出す
				rpm = (speed * GEAR_RATIO[nowGear] * FINAL_GEAR_RATIO * 60.0f) / (3.6f * 2.0f * DX_PI_F * TIRE_SIZE);
				// フューエルカットがかかってるなら
				if (fuelCut) {
					// フューエルカットを落とす
					fuelCut = false;
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
	// ラジアン205～-25までで回転数に応じ針を動かす
	float rad = SLOP * rpm + NEEDLE_MIN_RAD;
	// その針先の位置
	needlePos = Vector2(TACHOMETER_POS.x + NEEDLE_LENGTH * cosf(rad), TACHOMETER_POS.y + NEEDLE_LENGTH * -sinf(rad));

	Vector3 forward = Vector3(sinf(trans.rotation.x), 0.0f, cosf(trans.rotation.y));

	float temp = nowSteer - prevSteer;
	nowSteer = prevSteer + temp * 0.5f;

	Vector3 steerDirection = Vector3(sinf(trans.rotation.x + nowSteer), 0.0f, cosf(trans.rotation.z + nowSteer));

	float steerRate = fabsf(nowSteer);
	float forwardRate = 1 - steerRate;

	Vector3 vec = forward * forwardRate + steerDirection * steerRate;

	vec = vec.Normalized() * speed;

	trans.pos += vec;

	yawSpeed = (nowSteer * 2.0f * speed / 100.0f) / Application::FRAME_PAR_SECOND;

	//float yaw = trans.rotation.y + yawSpeed;
	//trans.SetAngle(yaw);
	trans.AddAngleYDeg(Rad2Deg(yawSpeed));

	// ここからデータロガー
	//
	// 最高トルク
	if (torqueMax < torque) { torqueMax = torque; torqueMaxRpm = rpm; }
	// 最高馬力
	hoursepower = (torque * rpm) / 9550.0f;
	if (hoursepowerMax < hoursepower) { hoursepowerMax = hoursepower; hoursepowerMaxRpm = rpm; }
}

void CarBase::SubDraw(void)
{
	// 点線の範囲
	for (int i = 0; i <= (int)MAX_SCALE / 100; i++) {
		// 間隔は100回転ずつで
		float rad = SLOP * (i * 100) + NEEDLE_MIN_RAD;
		// 何も書かない根元の位置
		Vector2 brank = Vector2(TACHOMETER_POS.x + BLANK_LENGTH * cosf(rad), TACHOMETER_POS.y + BLANK_LENGTH * -sinf(rad));

		// 1000回転ずつに
		if (i % 10 == 0) {
			// 線先の位置
			Vector2 line = Vector2(TACHOMETER_POS.x + (NEEDLE_LENGTH + 3.0f) * cosf(rad), TACHOMETER_POS.y + (NEEDLE_LENGTH + 3.0f) * -sinf(rad));
			// 根本少し内側に数字
			Vector2 number = Vector2(TACHOMETER_POS.x + (BLANK_LENGTH - 15.0f) * cosf(rad), TACHOMETER_POS.y + (BLANK_LENGTH - 15.0f) * -sinf(rad));
			DrawFormatString((int)number.x - 5, (int)number.y - 5, 0xffffff, "%d", i / 10);
			// レッドゾーン以下なら太い白線
			if (i < RED_ZONE) {
				DrawLineAA(line.x, line.y, brank.x, brank.y, 0xffffff, 3.0f);
			}
			// レッドゾーン以上なら太い赤線
			else {
				DrawLineAA(line.x, line.y, brank.x, brank.y, 0xff0000, 3.0f);
			}
		}
		// 1000回転を抜いた500回転ずつに
		else if (i % 5 == 0) {
			// 線先の位置
			Vector2 line = Vector2(TACHOMETER_POS.x + NEEDLE_LENGTH * cosf(rad), TACHOMETER_POS.y + NEEDLE_LENGTH * -sinf(rad));
			// レッドゾーン以下なら少し太い白線
			if (i < RED_ZONE) {
				DrawLineAA(line.x, line.y, brank.x, brank.y, 0xffffff, 2.0f);
			}
			// レッドゾーン以上なら少し太い赤線
			else {
				DrawLineAA(line.x, line.y, brank.x, brank.y, 0xff0000, 2.0f);
			}
		}
		// それ以外の100回転ずつに
		else {
			// 線先の位置
			Vector2 line = Vector2(TACHOMETER_POS.x + (NEEDLE_LENGTH - 3.0f) * cosf(rad), TACHOMETER_POS.y + (NEEDLE_LENGTH - 3.0f) * -sinf(rad));
			// レッドゾーン以下なら細い白線
			if (i < RED_ZONE) {
				DrawLineAA(line.x, line.y, brank.x, brank.y, 0xffffff);
			}
			// レッドゾーン以上なら細い赤線
			else {
				DrawLineAA(line.x, line.y, brank.x, brank.y, 0xff0000);
			}
		}
	}
	// 針の描画
	DrawLineAA(TACHOMETER_POS.x, TACHOMETER_POS.y, needlePos.x, needlePos.y, 0xff0000, 5.0f);
	// レッドライン
	DrawCircleAA(TACHOMETER_POS.x, TACHOMETER_POS.y, NEEDLE_LENGTH / 2.0f + 3.0f, 32, 0xff0000);
	// ディスプレイ
	DrawCircleAA(TACHOMETER_POS.x, TACHOMETER_POS.y, NEEDLE_LENGTH / 2.0f, 32, 0x000000);
	// 速度表示のためフォントサイズ拡大
	SetFontSize(40);
	// 速度を10のあまりにして
	int s = static_cast<int>(speed) % 10;
	// 下1桁描画
	DrawFormatString((int)TACHOMETER_POS.x + 12, (int)TACHOMETER_POS.y - 20, 0xffffff, "%d", s);
	// 10で割り2桁目3桁目の2桁にしそれが1以上なら
	if ((s = static_cast<int>(speed) / 10) >= 1) {
		// 二桁目描画
		DrawFormatString((int)TACHOMETER_POS.x - 12, (int)TACHOMETER_POS.y - 20, 0xffffff, "%d", s % 10);
	}
	// 100で割り3桁目のみにしそれが１以上なら
	if ((s = static_cast<int>(speed) / 100) >= 1) {
		// 三桁目描画
		DrawFormatString((int)TACHOMETER_POS.x - 36, (int)TACHOMETER_POS.y - 20, 0xffffff, "%d", s);
	}
	// 今のギア数
	DrawBoxAA(TACHOMETER_POS.x - NEEDLE_LENGTH * 3.0f / 2.0f - 10.0f, TACHOMETER_POS.y - 20.0f, TACHOMETER_POS.x - NEEDLE_LENGTH * 3.0f / 2.0f + 30.0f, TACHOMETER_POS.y + 20.0f, 0xffffff, false, 2.0f);
	DrawFormatString((int)(TACHOMETER_POS.x - NEEDLE_LENGTH * 3.0f / 2.0f), (int)(TACHOMETER_POS.y - 20.0f), 0xffffff, "%d", nowGear + 1);
	// 変えたフォントサイズを戻す
	SetFontSize(16);

	// ここからはデータロガー
	//
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
}

void CarBase::SubRelease(void)
{
}

float CarBase::Acceleration(float c)
{
	// エンジンパワーをトルク、ギア比、損失率、クラッチ率(アクセル開度)、タイヤサイズで出す
	float ep = (torque * GEAR_RATIO[nowGear] * FINAL_GEAR_RATIO * LOST_RATIO * c) / TIRE_SIZE;
	// エンジンパワーから抵抗を引いた分を車重で割って時速に戻す
	return (ep - AirResistance() - RollingResistance()) / CAR_WEIGHT * 3.6f;
}

float CarBase::EngineBrake(void)
{
	// エンジンブレーキ力を回転数、ギア比、エンブレ係数、タイヤサイズで出す
	float ep = (rpm * GEAR_RATIO[nowGear] * FINAL_GEAR_RATIO * ENGINE_BRAKE) / (TIRE_SIZE);
	// エンジンブレーキ力に抵抗を足し車重で割って時速に戻す
	return -(ep + AirResistance() + RollingResistance()) / CAR_WEIGHT * 3.6f;
}

float CarBase::AirResistance(void)
{
	// 空気抵抗を空気密度、CD値、前面投影面積、速度で出す
	return AIR_DENSITY * CD * FPA * (speed / 3.6f) * (speed / 3.6f) * 0.5f;
}

float CarBase::RollingResistance(void)
{
	// 転がり抵抗を出す
	return RR * 9.81f * CAR_WEIGHT;
}
