#include "FollowVecCamera.h"

#include <algorithm>
#include <cmath>

FollowVecCamera::FollowVecCamera(
	const Vector3* targetPos,
	const Vector3* targetVelocity,
	float distance,
	float height,
	float lookHeight,
	float fov
) :
	CameraBase(Vector3(), Quaternion::Identity(), fov),

	targetPos(targetPos),
	targetVelocity(targetVelocity),

	distance(distance),
	height(height),
	lookHeight(lookHeight),

	direction(0.0f, 0.0f, 1.0f),

	moveRate(DEFAULT_SMOOTH_CAMERA_MOVE_RATE),
	rotationRate(0.15f)
{
	// 初期位置と回転を設定する
	if (targetPos == nullptr || targetVelocity == nullptr) { return; }

	Vector3 moveDir = *targetVelocity;
	moveDir.y = 0.0f;

	if (moveDir.LengthSq() > MIN_VELOCITY_SQ) {
		direction = moveDir.Normalized();
	}

	pos = *targetPos - direction * distance + Vector3::Yonly(height);
	rotation = CalcCameraRotation(pos, *targetPos + Vector3::Yonly(lookHeight));
}

void FollowVecCamera::TargetChange(const Vector3* targetPos, const Vector3* targetVelocity)
{
	// 安全処理
	if (targetPos == nullptr || targetVelocity == nullptr) { return; }

	this->targetPos = targetPos;
	this->targetVelocity = targetVelocity;
}

void FollowVecCamera::SetMoveRate(float rate)
{
	moveRate = std::clamp(rate, 0.0f, 1.0f);
}

void FollowVecCamera::SetRotationRate(float rate)
{
	rotationRate = std::clamp(rate, 0.0f, 1.0f);
}

void FollowVecCamera::NormalUpdate(void)
{
	// 追従対象が設定されていなかったら処理をしない(安全)
	if (targetPos == nullptr || targetVelocity == nullptr) { return; }

	// 進行方向を取得する
	Vector3 moveDir = *targetVelocity;
	moveDir.y = 0.0f;

	// 停止中は最後に有効だった進行方向を維持する
	if (moveDir.LengthSq() > MIN_VELOCITY_SQ) {
		direction = moveDir.Normalized();
	}

	// 目標カメラ位置
	const Vector3 desiredPos =
		*targetPos - direction * distance + Vector3::Yonly(height);

	// 時間倍率に合わせてカメラ位置を補間する
	const float moveT = std::clamp(moveRate * GetTimeScale(), 0.0f, 1.0f);
	pos += (desiredPos - pos) * moveT;

	// 注視点を算出
	const Vector3 lookAtPos = *targetPos + Vector3::Yonly(lookHeight);

	// 目標回転を算出
	const Quaternion desiredRotation = CalcCameraRotation(pos, lookAtPos);

	// 時間倍率に合わせてカメラ回転を補間する
	const float rotationT = std::clamp(rotationRate * GetTimeScale(), 0.0f, 1.0f);
	rotation = Quaternion::Slerp(rotation, desiredRotation, rotationT);
}
