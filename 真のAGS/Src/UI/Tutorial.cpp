#include "Tutorial.h"
#include <DxLib.h>
#include "../Application.h"
#include "../Manager/ResourceManager.h"
#include <cstdio>
#include <cmath>

#ifndef KEY_TUTORIAL_NEXT
#define KEY_TUTORIAL_NEXT KEY_INPUT_Z
#endif

static std::vector<std::string> SplitUTF8(const std::string& s)
{
	std::vector<std::string> out;
	for (size_t i = 0; i < s.size();)
	{
		unsigned char c = static_cast<unsigned char>(s[i]);

		// Shift-JISの先頭バイト判定
		if ((0x81 <= c && c <= 0x9F) || (0xE0 <= c && c <= 0xFC))
		{
			// 2バイト文字
			out.push_back(s.substr(i, 2));
			i += 2;
		}
		else
		{
			// 1バイト文字
			out.push_back(s.substr(i, 1));
			i += 1;
		}
	}
	return out;
}

Tutorial::Tutorial()
	: steps_(), currentIndex_(-1), active_(false), inputDelay_(0),
	animCounter_(0), revealIndex_(0), typeTick_(0), fontHandle_(-1)
{
}

Tutorial::~Tutorial()
{
	if (fontHandle_ != -1)
	{
		DeleteFontToHandle(fontHandle_);
		fontHandle_ = -1;
	}
}

void Tutorial::Init()
{
	steps_.clear();

	// フォントを作成
	if (fontHandle_ != -1)
	{
		DeleteFontToHandle(fontHandle_);
		fontHandle_ = -1;
	}
	fontHandle_ = CreateFontToHandle("Yu Gothic UI", fontSize_, 400, DX_FONTTYPE_ANTIALIASING);
}

void Tutorial::ClearSteps()
{
	steps_.clear();
	currentIndex_ = -1;
	revealIndex_ = 0;
	typeTick_ = 0;
}

void Tutorial::AddStep(const std::string& text, ConditionFunc cond, OnEnterFunc onEnter, int enoguHandle)
{
	steps_.push_back({ text, cond, onEnter, enoguHandle });
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
	revealIndex_ = 0;
	typeTick_ = 0;
	splitText_ = SplitUTF8(steps_[0].text);

	if (steps_[0].onEnter) steps_[0].onEnter();
}

void Tutorial::Update()
{
	if (!active_) return;

	++animCounter_;
	if (inputDelay_ > 0) --inputDelay_;
	if (currentIndex_ < 0 || currentIndex_ >= static_cast<int>(steps_.size())) return;

	const auto& cur = steps_[currentIndex_];
	bool condResult = cur.condition ? cur.condition() : false;
	bool userNextKey = (inputDelay_ == 0) &&
		(CheckHitKey(KEY_TUTORIAL_NEXT) || CheckHitKey(KEY_INPUT_RETURN));

	// タイプ表示進行
	if (revealIndex_ < static_cast<int>(splitText_.size()))
	{
		if (++typeTick_ >= TYPE_SPEED) { ++revealIndex_; typeTick_ = 0; }
		if (userNextKey) { revealIndex_ = static_cast<int>(splitText_.size()); inputDelay_ = DELAY_MAX; return; }
	}

	// 条件付きならcondResultが真で進む。
	bool ready = condResult || (revealIndex_ >= static_cast<int>(splitText_.size()) && userNextKey);

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
			revealIndex_ = 0;
			typeTick_ = 0;
			splitText_ = SplitUTF8(steps_[currentIndex_].text);
			if (steps_[currentIndex_].onEnter) steps_[currentIndex_].onEnter();
		}
	}
}

// Layoutの計算
Tutorial::Layout Tutorial::ComputeLayout() const
{
	Layout l;
	l.screenW = Application::SCREEN_SIZE_X;
	l.screenH = Application::SCREEN_SIZE_Y;
	l.margin = 24;

	// テキストの行数を概算
	int lines = 1;
	if (currentIndex_ >= 0 && currentIndex_ < static_cast<int>(steps_.size()))
	{
		const std::string& txt = steps_[currentIndex_].text;
		for (char c : txt) if (c == '\n') ++lines;
	}
	const int lineH = fontSize_ + lineSpacing_;
	int bubbleH = lines * lineH + bubblePaddingY_ * 2;
	if (bubbleH < minBubbleH_) bubbleH = minBubbleH_;
	if (bubbleH > maxBubbleH_) bubbleH = maxBubbleH_;

	// 全体ボックス高さは顔領域＋吹き出し（既存の見た目を保つため +40）
	l.boxH = bubbleH + 40;

	l.x0 = l.margin;
	l.y0 = l.screenH - l.boxH - l.margin;
	l.x1 = l.screenW - l.margin;
	l.y1 = l.screenH - l.margin;
	l.faceW = 120;
	l.faceX = l.x0 + 12;
	l.bubbleX = l.x0 + l.faceW + 24;
	l.bubbleY = l.y0 + 20;
	l.bubbleW = l.x1 - l.bubbleX - 18;
	l.bubbleH = bubbleH;
	l.tailW = 20;
	l.tailH = 28;
	l.tailX = l.bubbleX - l.tailW + 2;
	l.tailY = l.bubbleY + l.bubbleH / 2 - l.tailH / 2;
	return l;
}

void Tutorial::FillBox(int x1, int y1, int x2, int y2, int r, int g, int b, int alpha)
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	DrawBox(x1, y1, x2, y2, GetColor(r, g, b), true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void Tutorial::StrokeBox(int x1, int y1, int x2, int y2, int r, int g, int b)
{
	DrawBox(x1, y1, x2, y2, GetColor(r, g, b), false);
}

// 絵の具描画
void Tutorial::DrawEnogu(int handle, const Layout& l)
{
	if (handle <= 0) return;

	int gw = 0, gh = 0;
	GetGraphSize(handle, &gw, &gh);

	if (gw > 0 && gh > 0)
	{
		const float scale = (std::min)(static_cast<float>(l.faceW) / gw, static_cast<float>(l.faceW) / gh);
		const int w = static_cast<int>(gw * scale);
		const int h = static_cast<int>(gh * scale);
		const int x = l.faceX + (l.faceW - w) / 2;
		const int y = l.y0 + (l.boxH - h) / 2;
		DrawExtendGraph(x, y, x + w, y + h, handle, true);
	}
	else
	{
		const int y = l.y0 + (l.boxH - l.faceW) / 2;
		DrawExtendGraph(l.faceX, y, l.faceX + l.faceW, y + l.faceW, handle, true);
	}
}

void Tutorial::Draw() const
{
	if (!active_ || currentIndex_ < 0 || currentIndex_ >= static_cast<int>(steps_.size())) return;

	// レイアウトを一つの構造体で取得
	const Layout l = ComputeLayout();

	// 吹き出し
	FillBox(l.bubbleX, l.bubbleY, l.bubbleX + l.bubbleW, l.bubbleY + l.bubbleH, 30, 40, 60, 220);
	StrokeBox(l.bubbleX, l.bubbleY, l.bubbleX + l.bubbleW, l.bubbleY + l.bubbleH, 200, 200, 220);

	// 尾
	//FillBox(l.tailX, l.tailY, l.tailX + l.tailW, l.tailY + l.tailH, 30, 40, 60, 220);
	//StrokeBox(l.tailX, l.tailY, l.tailX + l.tailW, l.tailY + l.tailH, 200, 200, 220);

	// 絵の具の描画
	int selectedHandle = -1;
	const auto& cur = steps_[currentIndex_];
	if (cur.enoguHandle >= 0)
	{
		selectedHandle = cur.enoguHandle;
	}

	// 絵の具描画
	DrawEnogu(selectedHandle, l);

	int visible = revealIndex_;
	if (visible > static_cast<int>(splitText_.size())) visible = static_cast<int>(splitText_.size());
	std::string sub;
	sub.reserve(visible * 3);
	for (int i = 0; i < visible; ++i) sub += splitText_[i];

	int textX = l.bubbleX + 16, textY = l.bubbleY + 12; // 上余白を少し減らす
	// Yu Gothic UIフォントを使って描画
	if (fontHandle_ != -1)
	{
		DrawStringToHandle(textX, textY, sub.c_str(), GetColor(235, 235, 235), fontHandle_);
	}
	else
	{
		DrawString(textX, textY, sub.c_str(), GetColor(235, 235, 235));
	}

	if (visible >= static_cast<int>(splitText_.size()) && ((animCounter_ / 12) % 2 == 0))
	{
		const char* cont = "▼";
		if (fontHandle_ != -1)
			DrawStringToHandle(l.bubbleX + l.bubbleW - 28, l.bubbleY + l.bubbleH - 28, cont, GetColor(200, 200, 140), fontHandle_);
		else
			DrawString(l.bubbleX + l.bubbleW - 28, l.bubbleY + l.bubbleH - 28, cont, GetColor(200, 200, 140));
	}

	int total = static_cast<int>(steps_.size());

	char progressMsg[64];
	snprintf(progressMsg, sizeof(progressMsg), "進行: %d / %d", currentIndex_ + 1, total);
	if (fontHandle_ != -1)
		DrawStringToHandle(l.x1 - 160, l.y1 - 52, progressMsg, GetColor(180, 180, 180), fontHandle_);
	else
		DrawString(l.x1 - 160, l.y1 - 52, progressMsg, GetColor(180, 180, 180));
}