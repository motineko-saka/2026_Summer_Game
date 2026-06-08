#include "Tutorial.h"
#include <DxLib.h>
#include "../Application.h"
#include <cstdio>
#include <cmath>

#ifndef KEY_TUTORIAL_NEXT
#define KEY_TUTORIAL_NEXT KEY_INPUT_Z
#endif

Tutorial::Tutorial()
	: steps_()
	, currentIndex_(-1)
	, active_(false)
	, inputDelay_(0)
	, animCounter_(0)
{
}

Tutorial::~Tutorial()
{
}

void Tutorial::Init()
{
	steps_.clear();
	currentIndex_ = -1;
	active_ = false;
	inputDelay_ = 0;
	animCounter_ = 0;
}

void Tutorial::ClearSteps()
{
	steps_.clear();
	currentIndex_ = -1;
}

void Tutorial::AddStep(const std::string& text, ConditionFunc cond, OnEnterFunc onEnter)
{
	StepInfo s;
	s.text = text;
	s.condition = cond;
	s.onEnter = onEnter;
	steps_.push_back(std::move(s));
}

void Tutorial::Start()
{
	if (steps_.empty())
	{
		active_ = false;
		currentIndex_ = -1;
		return;
	}
	currentIndex_ = 0;
	active_ = true;
	inputDelay_ = DELAY_MAX;
	animCounter_ = 0;
	if (steps_[0].onEnter) steps_[0].onEnter();
}

void Tutorial::Update()
{
	if (!active_) return;

	// アニメーションカウンタ更新（描画で使用）
	++animCounter_;
	if (inputDelay_ > 0) --inputDelay_;

	if (currentIndex_ < 0 || currentIndex_ >= static_cast<int>(steps_.size())) return;

	const auto& curStep = steps_[currentIndex_];
	bool ready = false;
	if (curStep.condition)
	{
		// 条件チェック（描画でも使うため副作用を避ける設計が望ましい）
		ready = curStep.condition();
	}
	else
	{
		if (inputDelay_ == 0 &&
			(CheckHitKey(KEY_TUTORIAL_NEXT) || CheckHitKey(KEY_INPUT_RETURN) || CheckHitKey(KEY_INPUT_SPACE)))
		{
			ready = true;
		}
	}

	if (ready && inputDelay_ == 0)
	{
		++currentIndex_;
		inputDelay_ = DELAY_MAX;
		if (currentIndex_ >= static_cast<int>(steps_.size()))
		{
			active_ = false;
			currentIndex_ = -1;
		}
		else
		{
			if (steps_[currentIndex_].onEnter) steps_[currentIndex_].onEnter();
		}
	}
}

void Tutorial::Draw() const
{
	if (!active_ || currentIndex_ < 0 || currentIndex_ >= static_cast<int>(steps_.size())) return;

	const int screenW = Application::SCREEN_SIZE_X;
	const int screenH = Application::SCREEN_SIZE_Y;

	// チュートリアルボックスサイズ
	const int margin = 24;
	const int boxH = 200;
	int x0 = margin;
	int y0 = screenH - boxH - margin;
	int x1 = screenW - margin;
	int y1 = screenH - margin;

	// 背景の半透明マスク（全画面暗転）
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 120);
	DrawBox(0, 0, screenW, screenH, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// メインボックス（2層でグラデ風）
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
	DrawBox(x0, y0, x1, y1, GetColor(18, 24, 40), TRUE); // ダークブルー
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 120);
	DrawBox(x0, y0, x1, y0 + 48, GetColor(48, 68, 120), TRUE); // ヘッダー風
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// ヘッダテキスト（大きめ）
	const char* header = "操作チュートリアル";
	DrawString(x0 + 18, y0 + 8, header, GetColor(255, 230, 120));

	// ステップ番号表示
	char stepbuf[64];
	int total = static_cast<int>(steps_.size());
	snprintf(stepbuf, sizeof(stepbuf), "STEP %d / %d", currentIndex_ + 1, total);
	DrawString(x1 - 160, y0 + 12, stepbuf, GetColor(200, 200, 200));

	// プログレスバー（下部）
	const int barX = x0 + 18;
	const int barY = y1 - 34;
	const int barW = x1 - x0 - 36;
	const int barH = 12;
	// 背景
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 140);
	DrawBox(barX, barY, barX + barW, barY + barH, GetColor(80, 80, 80), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// 進捗幅
	float progress = 0.0f;
	if (total > 0) progress = float(currentIndex_) / float(total);
	int fillW = static_cast<int>(barW * progress);

	// 説明テキスト（改行対応）
	const char* text = steps_[currentIndex_].text.c_str();
	const int tx = x0 + 18;
	const int ty = y0 + 58;
	DrawString(tx, ty, text, GetColor(235, 235, 235));

	// 現在ステップの「達成可能」状態を調べる（描画用）
	bool stepReady = false;
	const auto& curStep = steps_[currentIndex_];
	if (curStep.condition)
	{
		// 条件関数は副作用を起こさない想定で呼ぶ（安全のため設計注意）
		stepReady = curStep.condition();
	}

	// プログレスバーの塗り（色をゆらがせる）
	float t = animCounter_ * 0.06f;
	int r = static_cast<int>(160 + 40.0f * std::sin(t));
	int g = static_cast<int>(200 + 20.0f * std::sin(t * 0.9f));
	int b = static_cast<int>(220 + 20.0f * std::sin(t * 1.1f));
	// ready のときは緑系に変える
	if (stepReady)
	{
		// 緑系で強調
		r = static_cast<int>(120 + 40.0f * std::sin(t * 1.2f));
		g = static_cast<int>(220 + 20.0f * std::sin(t * 1.1f));
		b = static_cast<int>(140 + 20.0f * std::sin(t * 0.9f));
	}
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
	DrawBox(barX, barY, barX + fillW, barY + barH, GetColor(r, g, b), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// 行動を促す注意テキスト（目立たせる）
	const char* hint = "行動を行うまで次に進みません。";
	int hintX = x1 - 420;
	int hintY = y0 + 56;
	DrawString(hintX, hintY, hint, GetColor(220, 200, 100));

	// 矢印（STEP近くに配置） ? ready によって色/点滅を変える
	int arrowBaseX = x1 - 80;
	int arrowBaseY = y0 + 28;
	float phase = std::fmodf(animCounter_ * 0.08f, 3.14159f * 2.0f);
	int offset = static_cast<int>(std::sin(phase) * 6.0f);
	for (int i = 0; i < 3; ++i)
	{
		int ox = arrowBaseX - i * 12;
		int oy = arrowBaseY + offset - i * 3;

		// 色決定
		int ar, ag, ab;
		int alpha = 200;
		if (stepReady)
		{
			// 点滅する緑で強調
			float blink = (std::sin(animCounter_ * 0.3f + i) + 1.0f) * 0.5f; // 0..1
			ar = static_cast<int>(120 + 120.0f * blink);
			ag = static_cast<int>(200 + 55.0f * blink);
			ab = static_cast<int>(80 + 20.0f * blink);
			alpha = static_cast<int>(160 + 95.0f * blink);
		}
		else
		{
			// 通常のオレンジ系
			ar = 255; ag = 200 - i * 20; ab = 80;
		}

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
		// 上向き chevron をラインで表現（太め）
		DrawLine(ox - 8, oy - 6, ox + 6, oy, GetColor(ar, ag, ab), 6);
		DrawLine(ox - 8, oy + 6, ox + 6, oy, GetColor(ar / 2, ag / 2, ab / 2), 6);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	// ドット（プログレスバーの末端近くに配置） ? 状態を色で表現
	int dotX = barX + fillW + 12;
	int dotY = barY + (barH / 2);
	// ドット色決定
	int dr, dg, db;
	int dalpha = 200;
	if (stepReady)
	{
		// 緑に点滅
		float blink = (std::sin(animCounter_ * 0.35f) + 1.0f) * 0.5f;
		dr = static_cast<int>(120 + 120.0f * blink);
		dg = static_cast<int>(200 + 55.0f * blink);
		db = static_cast<int>(80 + 20.0f * blink);
		dalpha = static_cast<int>(140 + 115.0f * blink);
	}
	else
	{
		// 待機中はやや暗めのオレンジ／灰色（行動を促す色）
		dr = 200; dg = 160; db = 100;
		dalpha = 160;
	}

	// ドットのリングと中点で演出
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, dalpha);
	DrawCircle(dotX, dotY, 10, GetColor(dr, dg, db), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, dalpha / 2);
	DrawCircle(dotX, dotY, 16, GetColor(dr / 2, dg / 2, db / 2), FALSE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// 小さな補助説明（右下）
	char progressMsg[64];
	snprintf(progressMsg, sizeof(progressMsg), "進行: %d / %d", currentIndex_ + 1, total);
	DrawString(x1 - 160, y1 - 52, progressMsg, GetColor(180, 180, 180));
}