#pragma once

#include "../CameraBase.h"

class FollowVecCamera : public CameraBase
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="targetPos">追従対象の座標</param>
	/// <param name="targetVelocity">追従対象の速度</param>
	/// <param name="distance">追従対象から後方への距離</param>
	/// <param name="height">追従対象から上方への距離</param>
	/// <param name="lookHeight">注視点の高さ</param>
	/// <param name="fov">視野角</param>
	FollowVecCamera(
		const Vector3* targetPos,
		const Vector3* targetVelocity,
		float distance = 400.0f,
		float height = 150.0f,
		float lookHeight = 60.0f,
		float fov = 80.0f * (DX_PI_F / 180.0f)
	);

	// デストラクタ
	~FollowVecCamera()override = default;

	// 追従対象を途中で変更する
	void TargetChange(const Vector3* targetPos, const Vector3* targetVelocity);

	// カメラ位置の追従率を設定する
	void SetMoveRate(float rate);

	// カメラ回転の追従率を設定する
	void SetRotationRate(float rate);

private:

	// 進行方向を更新する最低速度の二乗
	static constexpr float MIN_VELOCITY_SQ = 0.0001f;

	// 追従対象の座標
	const Vector3* targetPos;

	// 追従対象の速度
	const Vector3* targetVelocity;

	// カメラの後方距離
	float distance;

	// カメラの高さ
	float height;

	// 注視点の高さ
	float lookHeight;

	// 最後に有効だった進行方向
	Vector3 direction;

	// カメラ位置の追従率
	float moveRate;

	// カメラ回転の追従率
	float rotationRate;

	// 更新
	void NormalUpdate(void)override;
};
