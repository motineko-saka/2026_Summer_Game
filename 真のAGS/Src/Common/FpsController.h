#pragma once
#include <chrono>
#include <cstddef>
#include <deque>

class FpsController
{
public:

	// コンストラクタ
	// ( DxLib_Init前に呼ぶこと )
	FpsController(int fixedFps);
	~FpsController(void) = default;

	// 1フレームごとのFPS調整
	// ( ScreenFlip後に呼ぶこと )
	void Wait(void);

	// 画面右上にFPSを表示
	void Draw(void) const;

	// 現在の平均FPSを取得
	float GetFPS(void) const { return fps_; }

private:

	using Clock = std::chrono::steady_clock;

	// 最大FPS
	static constexpr int MAX_FPS = 1200;

	// 平均FPS計算に使用するフレーム数
	static constexpr std::size_t AVG_FPS_COUNT = 60;

	// 平均FPSの右上描画位置調整
	static constexpr int MARGIN = 20;

	// 描画フォーマット
	static constexpr const char* TEXT_FORMAT = "FPS : %.2f";

	// 平均FPSを更新
	void UpdateAverage(double deltaTime);

	// 指定された固定フレームレート
	const int fixedFps_;

	// 1フレームの理想時間(秒)
	const double idealFrameTime_;

	// 計測した平均FPS
	float fps_ = 0.0f;

	// 直近フレームの経過時間(秒)
	std::deque<double> timeList_;

	// timeList_ の合計(秒)
	double totalTime_ = 0.0;

	// 前フレームの時刻
	Clock::time_point prevTime_;
};