#include <DxLib.h>
#include "Loading.h"
#include "../Application.h"

namespace
{
	// 60fps
	constexpr int MIN_LOAD_FRAME = 30;

	// ”wŒiF
	constexpr unsigned int BG_COLOR = 0xFFFFFF;
}

Loading* Loading::instance_ = nullptr;

void Loading::Init(void)
{
	loadTimer_ = 0;
	isLoading_ = false;
	pos_ = Vector2(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2);
}

void Loading::Load(void)
{

}

void Loading::Update(void)
{
	if (!isLoading_) return;

	loadTimer_++;

	// “Ç‚İ‚İ’†‚Ì‚à‚Ì‚ª–³‚­‚È‚èA‚©‚ÂÅ’á•\¦ŠÔ‚ğ‰ß‚¬‚½‚çI—¹
	if (GetASyncLoadNum() == 0 && loadTimer_ >= MIN_LOAD_FRAME)
	{
		FinishLoad();
		return;
	}

}

void Loading::Draw(void) const
{
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, BG_COLOR, TRUE);

	// ‰æ‘œ‚ª“Ç‚İ‚Ü‚ê‚Ä‚¢‚È‚¯‚ê‚Î•`‚©‚È‚¢
	if (handle_ == -1) return;

	DrawRotaGraphF(
		static_cast<float>(pos_.x), static_cast<float>(pos_.y),
		1.0,		// Šg‘å—¦
		0.0,		// ‰ñ“]Šp
		handle_,
		true);		// “§‰ß
}

void Loading::Release(void)
{
	if (handle_ == -1) return;

	DeleteGraph(handle_);
	handle_ = -1;
}

void Loading::StartAsyncLoad(void)
{
	isLoading_ = true;
	loadTimer_ = 0;
	SetUseASyncLoadFlag(true);
}

void Loading::EndAsyncLoad(void)
{
	// ˆÈ~‚Ì“Ç‚İ‚İ‚ğ“¯Šú‚É–ß‚·
	SetUseASyncLoadFlag(false);
}

void Loading::FinishLoad(void)
{
	isLoading_ = false;
	loadTimer_ = 0;
}