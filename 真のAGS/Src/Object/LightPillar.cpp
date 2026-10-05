#include "LightPillar.h"
#include <algorithm>

void LightPillar::Init(VECTOR pos)
{
	pos_ = pos;
	scale_ = 0.0f;
	alpha_ = 0;
	timer_ = 0;
	active_ = true;
}

void LightPillar::Update()
{
	if (!active_) return;

	timer_++;

	// èoåª
	if (timer_ < 30)
	{
		scale_ += 0.05f;
		alpha_ += 8;
	}
	// è¡Ç¶ÇÈ
	else if (timer_ > 90)
	{
		alpha_ -= 5;
	}

	if (alpha_ <= 0)
	{
		active_ = false;
		return;
	}

	scale_ = (std::min)(scale_, 1.0f);
	alpha_ = std::clamp(alpha_, 0, 255);
}

void LightPillar::Draw()
{
	if (!active_) return;

	// îºìßñæ
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha_);

	//// íåÇÃï`âÊ
	//DrawCylinder3D(
	//	pos_,
	//	VAdd(pos_, VGet(0, 700 * scale_, 0)),
	//	40 * scale_,
	//	32,
	//	GetColor(255, 240, 150),
	//	GetColor(255, 240, 150),
	//	FALSE
	//);

	float radius = timer_ * 1.0f;

	//DrawCylinder3D(
	//	VAdd(pos_, VGet(0, 1.0f, 0)),
	//	VAdd(pos_, VGet(0, 3.0f, 0)),
	//	radius,
	//	64,
	//	GetColor(255, 220, 100),
	//	GetColor(255, 220, 100),
	//	TRUE
	//);

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}