#include <DxLib.h>
#include "Loading.h"
#include "../Application.h"

namespace
{
	// 最低でもロード画面を表示する時間(フレーム)
	// 60fps なら 30 フレーム = 0.5 秒
	constexpr int MIN_LOAD_FRAME = 30;

	// 背景色
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
	// ロード画面用の画像があればここで読み込む
	// 例) handle_ = LoadGraph("Data/Image/Loading.png");
}

void Loading::Update(void)
{
	if (!isLoading_) return;

	loadTimer_++;

	// 読み込み中のものが無くなり、かつ最低表示時間を過ぎたら終了
	if (GetASyncLoadNum() == 0 && loadTimer_ >= MIN_LOAD_FRAME)
	{
		FinishLoad();
		return;
	}

	// ロード画面を動かす処理はここに記述
}

void Loading::Draw(void) const
{
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, BG_COLOR, TRUE);

	// 画像が読み込まれていなければ描かない
	if (handle_ == -1) return;

	DrawRotaGraphF(
		static_cast<float>(pos_.x), static_cast<float>(pos_.y),
		1.0,		// 拡大率
		0.0,		// 回転角
		handle_,
		TRUE);		// 透過
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
	SetUseASyncLoadFlag(TRUE);
}

void Loading::EndAsyncLoad(void)
{
	// 以降の読み込みを同期に戻す（読み込み中のものは継続）
	SetUseASyncLoadFlag(FALSE);
}

void Loading::FinishLoad(void)
{
	isLoading_ = false;
	loadTimer_ = 0;
}