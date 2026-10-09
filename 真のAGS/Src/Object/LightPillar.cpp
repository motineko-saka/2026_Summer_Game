#include <DxLib.h>
#include <algorithm>
#include "LightPillar.h"

LightPillar::LightPillar()
	:
	pos_{ 0, 0, 0 },
	scale_{ 0.0f },
	alpha_{ 0 },
	timer_{ 0 },
	active_{ false }
{
}

LightPillar::~LightPillar()
{
}

void LightPillar::Init(VECTOR pos)
{
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

	float radius = timer_ * 1.0f;

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}