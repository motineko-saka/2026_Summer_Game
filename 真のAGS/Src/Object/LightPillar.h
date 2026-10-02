#pragma once
#include <DxLib.h>

class LightPillar
{
public:

	void Init(VECTOR pos);

	void Update();

	void Draw();


private:

	VECTOR pos_;

	float scale_;
	int alpha_;
	int timer_;

	bool active_;
};