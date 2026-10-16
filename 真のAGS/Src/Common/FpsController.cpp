#include <DxLib.h>
#include <algorithm>
#include "../Application.h"
#include "FpsController.h"

namespace
{
	// 時刻の差を秒に変換
	double ToSeconds(std::chrono::steady_clock::duration d)
	{
		return std::chrono::duration<double>(d).count();
	}
}

FpsController::FpsController(int fixedFps)
	:
	// 1 〜 最大FPS の範囲に収める（0以下による0除算を防ぐ）
	fixedFps_(std::clamp(fixedFps, 1, MAX_FPS)),
	// 1フレームの理想時間を計算
	idealFrameTime_(1.0 / fixedFps_),
	prevTime_(Clock::now())
{
	// DxLibの垂直同期待ちを無効化
	SetWaitVSyncFlag(FALSE);
}

void FpsController::Wait(void)
{
	auto now = Clock::now();
	double deltaTime = ToSeconds(now - prevTime_);

	// 理想時間より早ければ待機
	if (deltaTime < idealFrameTime_)
	{
		// 1ms以上あれば、まずスリープで大まかに待つ
		const double waitMs = (idealFrameTime_ - deltaTime) * 1000.0;
		if (waitMs >= 1.0)
		{
			WaitTimer(static_cast<int>(waitMs));
		}

		// 残りはビジーウェイトで正確に合わせる
		while (deltaTime < idealFrameTime_)
		{
			now = Clock::now();
			deltaTime = ToSeconds(now - prevTime_);
		}
	}

	prevTime_ = now;
	UpdateAverage(deltaTime);
}

void FpsController::UpdateAverage(double deltaTime)
{
	// 合計を保持しておき、毎フレームの全件ループを避ける
	timeList_.push_back(deltaTime);
	totalTime_ += deltaTime;

	if (timeList_.size() > AVG_FPS_COUNT)
	{
		totalTime_ -= timeList_.front();
		timeList_.pop_front();
	}

	fps_ = (totalTime_ > 0.0)
		? static_cast<float>(timeList_.size() / totalTime_)
		: 0.0f;
}

void FpsController::Draw(void) const
{
	// 描画する文字列の幅を取得し、右寄せ位置を計算
	const int textWidth = GetDrawFormatStringWidth(TEXT_FORMAT, fps_);
	const int x = Application::SCREEN_SIZE_X - textWidth - MARGIN;

	DrawFormatString(x, MARGIN, GetColor(255, 255, 255), TEXT_FORMAT, fps_);
}