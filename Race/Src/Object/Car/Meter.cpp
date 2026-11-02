#include "Meter.h"


Meter::Meter(const float& MAX_SCALE, const int& RED_ZONE, const float& speed, const float& rpm, const int& gear, const int& pNo, const std::function<int(void)>& camNo) :
	ActorBase(),
	MAX_SCALE(MAX_SCALE),
	RED_ZONE(RED_ZONE),
	speed(speed),
	rpm(rpm),
	gear(gear),
	camNo(camNo),
	pNo(pNo),
	needlePos(TACHOMETER_POS.x + NEEDLE_LENGTH * cosf(NEEDLE_MIN_RAD), TACHOMETER_POS.y + NEEDLE_LENGTH * -sinf(NEEDLE_MIN_RAD)),
	SLOP((NEEDLE_MAX_RAD - NEEDLE_MIN_RAD) / MAX_SCALE)
{
}

void Meter::Load()
{
	SetDrawType(ACTOR_DRAW_TYPE::Ui);

	// 手前の描画
	imageFront = MakeScreen(METER_SIZE, METER_SIZE, TRUE);
	SetDrawScreen(imageFront);
	ClearDrawScreen();

	// レッドライン
	DrawCircleAA(METER_SIZE / 2, METER_SIZE / 2, NEEDLE_LENGTH / 2.5f + 3.0f, 32, 0xff0000);
	// ディスプレイ
	DrawCircleAA(METER_SIZE / 2, METER_SIZE / 2, NEEDLE_LENGTH / 2.5f, 32, 0x000000);
	// ギア数ディスプレイ
	DrawBoxAA(METER_SIZE / 2 - NEEDLE_LENGTH * 3.0f / 2.0f - 10.0f, METER_SIZE / 2 - 20.0f, METER_SIZE / 2 - NEEDLE_LENGTH * 3.0f / 2.0f + 30.0f, METER_SIZE / 2 + 20.0f, 0x000000, true);
	DrawBoxAA(METER_SIZE / 2 - NEEDLE_LENGTH * 3.0f / 2.0f - 10.0f, METER_SIZE / 2 - 20.0f, METER_SIZE / 2 - NEEDLE_LENGTH * 3.0f / 2.0f + 30.0f, METER_SIZE / 2 + 20.0f, 0xffffff, false, 2.0f);

	// 後ろの描画
	imageBack = MakeScreen(METER_SIZE, METER_SIZE, TRUE);
	SetDrawScreen(imageBack);
	ClearDrawScreen();

	// 点線の範囲
	for (int i = 0; i <= (int)MAX_SCALE / 100; i++) {
		// 間隔は100回転ずつで
		float rad = SLOP * (i * 100) + NEEDLE_MIN_RAD;
		// 何も書かない根元の位置
		Vector2 brank = Vector2(METER_SIZE / 2 + BLANK_LENGTH * cosf(rad), METER_SIZE / 2 + BLANK_LENGTH * -sinf(rad));

		// 1000回転ずつに
		if (i % 10 == 0) {
			// 線先の位置
			Vector2 line = Vector2(METER_SIZE / 2 + (NEEDLE_LENGTH + 3.0f) * cosf(rad), METER_SIZE / 2 + (NEEDLE_LENGTH + 3.0f) * -sinf(rad));
			// 根本少し内側に数字
			SetFontSize(10);
			Vector2 number = Vector2(METER_SIZE / 2 + (BLANK_LENGTH - 15.0f) * cosf(rad), METER_SIZE / 2 + (BLANK_LENGTH - 15.0f) * -sinf(rad));
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
			Vector2 line = Vector2(METER_SIZE / 2 + NEEDLE_LENGTH * cosf(rad), METER_SIZE / 2 + NEEDLE_LENGTH * -sinf(rad));
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
			Vector2 line = Vector2(METER_SIZE / 2 + (NEEDLE_LENGTH - 3.0f) * cosf(rad), METER_SIZE / 2 + (NEEDLE_LENGTH - 3.0f) * -sinf(rad));
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
	SetDrawScreen(DX_SCREEN_BACK);
}

void Meter::SubInit(void)
{
}

void Meter::SubUpdate(void)
{
	// ラジアン205～-25までで回転数に応じ針を動かす
	float rad = SLOP * rpm + NEEDLE_MIN_RAD;
	// その針先の位置
	needlePos = Vector2(TACHOMETER_POS.x + NEEDLE_LENGTH * cosf(rad), TACHOMETER_POS.y + NEEDLE_LENGTH * -sinf(rad));

	time++;
}

void Meter::SubDraw(void)
{
	if (pNo != camNo()) { 
		return; 
	}

	DrawRotaGraph(TACHOMETER_POS.x, TACHOMETER_POS.y, 1.0, 0.0, imageBack, true);

	// 針の描画
	DrawLineAA(TACHOMETER_POS.x, TACHOMETER_POS.y, needlePos.x, needlePos.y, 0xff0000, 5.0f);

	DrawRotaGraph(TACHOMETER_POS.x, TACHOMETER_POS.y, 1.0, 0.0, imageFront, true);

	// 速度表示のためフォントサイズ拡大
	SetFontSize(30);
	// 速度を10のあまりにして
	int s = static_cast<int>(speed) % 10;
	// 下1桁描画
	DrawFormatString((int)TACHOMETER_POS.x + 12, (int)TACHOMETER_POS.y - 15, 0xffffff, "%d", s);
	// 10で割り2桁目3桁目の2桁にしそれが1以上なら
	if ((s = static_cast<int>(speed) / 10) >= 1) {
		// 二桁目描画
		DrawFormatString((int)TACHOMETER_POS.x - 8, (int)TACHOMETER_POS.y - 15, 0xffffff, "%d", s % 10);
	}
	// 100で割り3桁目のみにしそれが１以上なら
	if ((s = static_cast<int>(speed) / 100) >= 1) {
		// 三桁目描画
		DrawFormatString((int)TACHOMETER_POS.x - 28, (int)TACHOMETER_POS.y - 15, 0xffffff, "%d", s);
	}
	// 今のギア数
	DrawFormatString((int)(TACHOMETER_POS.x - NEEDLE_LENGTH * 3.0f / 2.0f), (int)(TACHOMETER_POS.y - 15.0f), 0xffffff, "%d", gear + 1);
	// 変えたフォントサイズを戻す
	SetFontSize(16);
}

void Meter::SubRelease(void)
{
	DeleteGraph(imageBack);
	DeleteGraph(imageFront);
}
