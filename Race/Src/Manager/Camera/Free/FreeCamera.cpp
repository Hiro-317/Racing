#include "FreeCamera.h"

#include "../../../Application/Application.h"

#include "../../../Manager/Input/InputManager.h"

FreeCamera::FreeCamera(float MOVE_POWER, float ROT_POWER, AngleTag, const Vector3& pos, const Vector3& angle, float fov) :
	CameraBase(pos, angle, fov),

	MOVE_POWER(MOVE_POWER),
	ROT_POWER(ROT_POWER)
{
	// カメラ回転が有効だったらマウスを固定しておく
	//if (ROT_POWER != 0.0f) { Key::GetIns().SetMouseFixed(true); }
}

FreeCamera::FreeCamera(float MOVE_POWER, float ROT_POWER, LookAtTag, const Vector3& pos, const Vector3& lookPos, float fov) :
	CameraBase(pos, CalcCameraAngle(pos, lookPos), fov),

	MOVE_POWER(MOVE_POWER),
	ROT_POWER(ROT_POWER)
{
	// カメラ回転が有効だったらマウスを固定しておく
	//if (ROT_POWER != 0.0f) { Key::GetIns().SetMouseFixed(true); }
}

void FreeCamera::NormalUpdate(void)
{
#pragma region 角度 (コントローラースティック -> マウス -> ボタン の順に確認して入力があったもので回転させる)

	// コントローラーの右スティックベクトルを代入
	Vector3 rot = Vector3::XYonly(Input::GetIns().GetPriorityAKeyRNorm().x, Input::GetIns().GetPriorityAKeyRNorm().z);

	// 最終的に入力が１つでもあれば回転させる
	if (rot != 0.0f) {

		// 回転させる
		angle += rot * ROT_POWER;

		// 回転の数値制御
		if (angle.y <= Deg2Rad(0.0f)) { angle.y += Deg2Rad(360.0f); }
		if (angle.y >= Deg2Rad(360.0f)) { angle.y -= Deg2Rad(360.0f); }
		if (angle.x <= Deg2Rad(0.0f)) { angle.x += Deg2Rad(360.0f); }
		if (angle.x >= Deg2Rad(360.0f)) { angle.x -= Deg2Rad(360.0f); }
	}
#pragma endregion

#pragma region 移動 (コントローラースティック -> ボタン の順に確認して入力があったもので移動させる)
	// コントローラーの左スティックベクトルを代入
	Vector3 dir = Input::GetIns().GetPriorityAKeyLNorm();

	// 入力があれば、方向×スピードで移動量を作って、座標に足して移動
	if (dir != 0.0f) { pos += dir.TransMat(MGetRotY(angle.y)) * MOVE_POWER; }
#pragma endregion
}

void FreeCamera::SubRelease(void)
{
	// 生成時固定したマウスを戻す
	//Key::GetIns().SetMouseFixed(false);
}