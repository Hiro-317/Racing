#include "AnalogMeter.h"


AnalogMeter::AnalogMeter(const float& TACO_MAX_SCALE, const float& SPEED_MAX_SCALE, const int& EVERY_SPEED, const int& RED_ZONE, const float& speed, const float& rpm, const int& gear, const int& pNo, const std::function<int(void)>& camNo) :
	ActorBase(),
	TACO_MAX_SCALE(TACO_MAX_SCALE),
	SPEED_MAX_SCALE(SPEED_MAX_SCALE),
	EVERY_SPEED(EVERY_SPEED),
	RED_ZONE(RED_ZONE),
	speed(speed),
	rpm(rpm),
	gear(gear),
	camNo(camNo),
	pNo(pNo),
	tacoNeedlePos(METER_POS.x + NEEDLE_LENGTH * cosf(NEEDLE_MIN_RAD), METER_POS.y + NEEDLE_LENGTH * -sinf(NEEDLE_MIN_RAD)),
	TACO_SLOP((NEEDLE_MAX_RAD - NEEDLE_MIN_RAD) / TACO_MAX_SCALE),
	SPEED_SLOP((NEEDLE_MAX_RAD - NEEDLE_MIN_RAD) / SPEED_MAX_SCALE)
{
}

void AnalogMeter::Load()
{
	SetDrawType(ACTOR_DRAW_TYPE::Ui);

	// 手前の描画
	imageFront = MakeScreen(METER_SIZE, METER_SIZE, TRUE);
	SetDrawScreen(imageFront);
	ClearDrawScreen();

	DrawCircleAA((float)METER_SIZE / 3, (float)METER_SIZE / 2, 20.0f, 32, 0x000000, true);
	DrawCircleAA((float)METER_SIZE * 2.0f / 3, (float)METER_SIZE / 2, 20.0f, 32, 0x000000, true);
	// ギア数ディスプレイ
	DrawBoxAA((float)METER_SIZE / 2.0f - 20.0f, (float)METER_SIZE / 2.0f - 20.0f, (float)METER_SIZE / 2.0f + 20.0f, (float)METER_SIZE / 2.0f + 20.0f, 0x000000, true);
	DrawBoxAA((float)METER_SIZE / 2.0f - 20.0f, (float)METER_SIZE / 2.0f - 20.0f, (float)METER_SIZE / 2.0f + 20.0f, (float)METER_SIZE / 2.0f + 20.0f, 0xffffff, false, 2.0f);

	// 後ろの描画
	imageBack = MakeScreen(METER_SIZE, METER_SIZE, TRUE);
	SetDrawScreen(imageBack);
	ClearDrawScreen();

	// 点線の範囲
	for (int i = 0; i <= (int)TACO_MAX_SCALE / 100; i++) {
		// 間隔は100回転ずつで
		float rad = TACO_SLOP * (i * 100) + NEEDLE_MIN_RAD;
		// 何も書かない根元の位置
		Vector2 brank = Vector2(METER_SIZE / 3 + BLANK_LENGTH * cosf(rad), METER_SIZE / 2 + BLANK_LENGTH * -sinf(rad));

		// 1000回転ずつに
		if (i % 10 == 0) {
			// 線先の位置
			Vector2 line = Vector2(METER_SIZE / 3 + (NEEDLE_LENGTH + 3.0f) * cosf(rad), METER_SIZE / 2 + (NEEDLE_LENGTH + 3.0f) * -sinf(rad));
			// 根本少し内側に数字
			SetFontSize(10);
			Vector2 number = Vector2(METER_SIZE / 3 + (BLANK_LENGTH - 15.0f) * cosf(rad), METER_SIZE / 2 + (BLANK_LENGTH - 15.0f) * -sinf(rad));
			SetFontSize(16);
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
			Vector2 line = Vector2(METER_SIZE / 3 + NEEDLE_LENGTH * cosf(rad), METER_SIZE / 2 + NEEDLE_LENGTH * -sinf(rad));
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
			Vector2 line = Vector2(METER_SIZE / 3 + (NEEDLE_LENGTH - 3.0f) * cosf(rad), METER_SIZE / 2 + (NEEDLE_LENGTH - 3.0f) * -sinf(rad));
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

	// 点線の範囲
	for (int i = 0; i <= (int)SPEED_MAX_SCALE / ((EVERY_SPEED / 2) * 10); i++) {
		// 間隔は指定速度の半分ずつで
		float rad = SPEED_SLOP * (i * ((EVERY_SPEED / 2) * 10)) + NEEDLE_MIN_RAD;
		// 何も書かない根元の位置
		Vector2 brank = Vector2(METER_SIZE * 2 / 3 + BLANK_LENGTH * cosf(rad), METER_SIZE / 2 + BLANK_LENGTH * -sinf(rad));

		// 指定速度ずつに
		if (i % 2 == 0) {
			// 線先の位置
			Vector2 line = Vector2(METER_SIZE * 2 / 3 + (NEEDLE_LENGTH + 3.0f) * cosf(rad), METER_SIZE / 2 + (NEEDLE_LENGTH + 3.0f) * -sinf(rad));
			// 根本少し内側に数字
			SetFontSize(10);
			Vector2 number = Vector2(METER_SIZE * 2 / 3 + (BLANK_LENGTH - 17.5f) * cosf(rad), METER_SIZE / 2 + (BLANK_LENGTH - 17.5f) * -sinf(rad));
			SetFontSize(16);
			DrawFormatString((int)number.x - 11, (int)number.y - 7, 0xffffff, "%d", i * ((EVERY_SPEED / 2) * 10));
			// 太い白線
			DrawLineAA(line.x, line.y, brank.x, brank.y, 0xffffff, 3.0f);
		}
		// 指定速度の半分ずつに
		else {
			// 線先の位置
			Vector2 line = Vector2(METER_SIZE * 2 / 3 + (NEEDLE_LENGTH - 3.0f) * cosf(rad), METER_SIZE / 2 + (NEEDLE_LENGTH - 3.0f) * -sinf(rad));
			// 細い白線
			DrawLineAA(line.x, line.y, brank.x, brank.y, 0xffffff);
		}
	}
	SetDrawScreen(DX_SCREEN_BACK);
}

void AnalogMeter::SubInit(void)
{
}

void AnalogMeter::SubUpdate(void)
{
	// ラジアン205～-25までで回転数に応じ針を動かす
	float rad = TACO_SLOP * rpm + NEEDLE_MIN_RAD;
	// その針先の位置
	tacoNeedlePos = Vector2(METER_POS.x - METER_SIZE / 6 + NEEDLE_LENGTH * cosf(rad), METER_POS.y + NEEDLE_LENGTH * -sinf(rad));

	// ラジアン205～-25までで回転数に応じ針を動かす
	rad = SPEED_SLOP * speed + NEEDLE_MIN_RAD;
	// その針先の位置
	speedNeedlePos = Vector2(METER_POS.x + METER_SIZE / 6 + NEEDLE_LENGTH * cosf(rad), METER_POS.y + NEEDLE_LENGTH * -sinf(rad));
}

void AnalogMeter::SubDraw(void)
{
	if (pNo != camNo()) {
		return;
	}

	DrawRotaGraph((int)METER_POS.x, (int)METER_POS.y, 1.0, 0.0, imageBack, true);

	// 針の描画
	DrawLineAA(METER_POS.x - METER_SIZE / 6, METER_POS.y, tacoNeedlePos.x, tacoNeedlePos.y, 0xff0000, 5.0f);
	DrawLineAA(METER_POS.x + METER_SIZE / 6, METER_POS.y, speedNeedlePos.x, speedNeedlePos.y, 0xff0000, 5.0f);

	DrawRotaGraph((int)METER_POS.x, (int)METER_POS.y, 1.0, 0.0, imageFront, true);

	// 速度表示のためフォントサイズ拡大
	SetFontSize(30);
	// 今のギア数
	DrawFormatString((int)METER_POS.x - 10, (int)METER_POS.y - 15, 0xffffff, "%d", gear + 1);
	// 変えたフォントサイズを戻す
	SetFontSize(16);
}

void AnalogMeter::SubRelease(void)
{
	DeleteGraph(imageBack);
	DeleteGraph(imageFront);
}
