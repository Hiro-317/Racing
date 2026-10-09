#include "DisplayRemoteCamera.h"

#include "../../../Application/Application.h"

#include "../../../Manager/Input/InputManager.h"

DisplayRemoteCamera::DisplayRemoteCamera(const Vector3& fixedLookAtPos, const Vector3& lookAtDiff, float ROT_POWER, const Vector3& angle, float fov) :
	CameraBase(Vector3(), Vector3(), fov),

	fixedLookAtPos(fixedLookAtPos),
	lookAtDiff(lookAtDiff),

	ROT_POWER(ROT_POWER),

	controlAngle(angle)
{
	// 現在の追従対象の座標と角度情報から自身(カメラ)の座標を算出する
	pos = fixedLookAtPos + lookAtDiff.TransMat(MatrixAllMultXY({ Vector3::XYonly(controlAngle.x,controlAngle.y) }));

	// 角度
	this->angle = CalcCameraAngle(pos, fixedLookAtPos);

	// カメラ回転が有効だったらマウスを固定しておく
	//if (ROT_POWER != 0.0f) { Key::GetIns().SetMouseFixed(true); }
}

void DisplayRemoteCamera::NormalUpdate(void)
{
#pragma region 角度

	// コントローラーの右スティックベクトルを代入
	Vector3 rot = Vector3::XYonly(Input::GetIns().GetPriorityAKeyRNorm().x, Input::GetIns().GetPriorityAKeyRNorm().z);

	// 最終的に入力が1つでもあれば回転させる
	if (rot != 0.0f) {

		// 回転させる
		controlAngle += rot * ROT_POWER;

		// 回転の数値制御
		if (controlAngle.y <= Deg2Rad(0.0f)) { controlAngle.y += Deg2Rad(360.0f); }
		if (controlAngle.y >= Deg2Rad(360.0f)) { controlAngle.y -= Deg2Rad(360.0f); }
		if (controlAngle.x <= Deg2Rad(0.0f)) { controlAngle.x += Deg2Rad(360.0f); }
		if (controlAngle.x >= Deg2Rad(360.0f)) { controlAngle.x -= Deg2Rad(360.0f); }
	}
#pragma endregion

	// 現在の追従対象の座標と角度情報から自身(カメラ)の座標を算出する
	pos = fixedLookAtPos + lookAtDiff.TransMat(MatrixAllMultXY({ Vector3::XYonly(controlAngle.x,controlAngle.y) }));

	// 角度
	angle = CalcCameraAngle(pos, fixedLookAtPos);
}

void DisplayRemoteCamera::SubRelease(void)
{
	// 生成時固定したマウスを戻す
	//Key::GetIns().SetMouseFixed(false);
}