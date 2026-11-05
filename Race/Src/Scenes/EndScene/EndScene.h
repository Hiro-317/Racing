#pragma once

#include"../SceneBase.h"

#include "../../Manager/Input/InputManager.h"


class EndScene : public SceneBase
{
public:
	EndScene();
	~EndScene()override = default;

private:

	// ‰Šú‰»iƒƒCƒ“ˆ—‚ÌŒãj
	void SubPreLoad(void) override;
	
	bool UseSkyBox(void)const override { return false; }
};