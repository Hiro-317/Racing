#pragma once

class FPS
{
public:
	FPS(int fps);
	~FPS();

	//‰Šú‰»
	void Init(void);

	bool UpdateFrameRate(void);

	void CalcFrameRate(void);

	void DrawFrameRate(void);

private:

	const float FRAME_RATE;

	int currentTime;
	int prevFrameTime;

	int frameCnt;
	int updateFrameRateTime;

	float frameRate;
};

