#include <DxLib.h>
#include <algorithm>
#include "../Application.h"
#include "Fader.h"

void Fader::SetFade(STATE state)
{
	state_ = state;
	if (state_ == STATE::NONE) return;

	isPreEnd_ = false;
	isEnd_ = false;
}

void Fader::Update(void)
{
	if (isEnd_ || state_ == STATE::NONE) return;

	// 暗転なら不透明へ、明転なら透明へ向かう
	const bool isFadeOut = (state_ == STATE::FADE_OUT);
	const float target = isFadeOut ? MAX_ALPHA : 0.0f;
	const float delta = isFadeOut ? SPEED_ALPHA : -SPEED_ALPHA;

	alpha_ = std::clamp(alpha_ + delta, 0.0f, MAX_ALPHA);
	if (alpha_ != target) return;

	// 目標に到達したら、1フレーム後(Draw後)に終了とする
	if (isPreEnd_) isEnd_ = true;
	isPreEnd_ = true;
}

void Fader::Draw(void)
{
	if (state_ == STATE::NONE) return;

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(alpha_));
	DrawBox(
		0, 0,
		Application::SCREEN_SIZE_X,
		Application::SCREEN_SIZE_Y,
		0x000000, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}