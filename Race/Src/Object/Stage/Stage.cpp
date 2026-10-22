#include "Stage.h"

#include "../../Application/Application.h"

Stage::Stage() : ActorBase() {}

void Stage::Load(void)
{

}

void Stage::SubDraw(void)
{
	DrawCube3D(VECTOR{ -100000.0f, 0.0f, -100000.0f }, VECTOR{ 100000.0f, 0.0f, 100000.0f }, 0xffffff, 0xffffff, true);

	for (float i = 0; i < 200.0f; i++) {
		DrawLine3D(VECTOR{ -100000.0f, 0.5f, i * 1000.0f - 100000.0f }, VECTOR{ 100000.0f, 0.5f, i * 1000.0f - 100000.0f }, 0xff0000);
		DrawLine3D(VECTOR{ i * 1000.0f - 100000.0f, 0.5f, -100000.0f }, VECTOR{ i * 1000.0f - 100000.0f, 0.5f, 100000.0f }, 0x0000ff);
	}
}
