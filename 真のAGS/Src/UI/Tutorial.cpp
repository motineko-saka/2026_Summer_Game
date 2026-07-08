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
		size_t len = 1;
		if ((c & 0x80) == 0) len = 1;
		else if ((c & 0xE0) == 0xC0) len = 2;
		else if ((c & 0xF0) == 0xE0) len = 3;
		else if ((c & 0xF8) == 0xF0) len = 4;
		out.push_back(s.substr(i, len));
		i += len;
	}
	return out;
}

Tutorial::Tutorial()
	: steps_(), currentIndex_(-1), active_(false), inputDelay_(0),
	animCounter_(0), revealIndex_(0), typeTick_(0), enoguHandle_(-1),
	enoguType_(Resource::TYPE::NONE)
{
}

Tutorial::~Tutorial() = default;

void Tutorial::Init()
{
	steps_.clear();
	currentIndex_ = -1;
	active_ = false;
	inputDelay_ = 0;
	animCounter_ = 0;
	revealIndex_ = 0;
	typeTick_ = 0;
	enoguHandle_ = -1;
	enoguType_ = Resource::TYPE::NONE;
}

void Tutorial::ClearSteps()
{
	steps_.clear();
	currentIndex_ = -1;
	revealIndex_ = 0;
	typeTick_ = 0;
	enoguHandle_ = -1;
	enoguType_ = Resource::TYPE::NONE;
}

void Tutorial::AddStep(const std::string& text, ConditionFunc cond, OnEnterFunc onEnter, ResourceManager::SRC face)
{
	steps_.push_back({ text, cond, onEnter, face });
}

void Tutorial::Start()
{
	if (steps_.empty())
	{
		active_ = false;
		currentIndex_ = -1;
		enoguHandle_ = -1;
		enoguType_ = Resource::TYPE::NONE;
		return;
	}
	currentIndex_ = 0;
	active_ = true;
	inputDelay_ = DELAY_MAX;
	animCounter_ = 0;
	revealIndex_ = 0;
	typeTick_ = 0;
	splitText_ = SplitUTF8(steps_[0].text);
	const Resource& r0 = ResourceManager::GetInstance().Load(steps_[0].faceSrc);
	enoguHandle_ = r0.handleId_;
	enoguType_ = r0.type_;
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
		(CheckHitKey(KEY_TUTORIAL_NEXT) || CheckHitKey(KEY_INPUT_RETURN) ||
			CheckHitKey(KEY_INPUT_SPACE) || (GetMouseInput() & MOUSE_INPUT_LEFT));

	// タイプ表示進行
	if (revealIndex_ < static_cast<int>(splitText_.size()))
	{
		if (++typeTick_ >= TYPE_SPEED) { ++revealIndex_; typeTick_ = 0; }
		if (userNextKey) { revealIndex_ = static_cast<int>(splitText_.size()); inputDelay_ = DELAY_MAX; return; }
	}

	// 進行可能判定：条件付きなら condResult が真で進む。未指定なら全文表示後のユーザー入力。
	bool ready = condResult || (revealIndex_ >= static_cast<int>(splitText_.size()) && userNextKey);

	if (ready && inputDelay_ == 0)
	{
		++currentIndex_;
		inputDelay_ = DELAY_MAX;
		if (currentIndex_ >= static_cast<int>(steps_.size()))
		{
			active_ = false;
			currentIndex_ = -1;
			enoguHandle_ = -1;
			enoguType_ = Resource::TYPE::NONE;
		}
		else
		{
			const Resource& r = ResourceManager::GetInstance().Load(steps_[currentIndex_].faceSrc);
			enoguHandle_ = r.handleId_;
			enoguType_ = r.type_;
			revealIndex_ = 0;
			typeTick_ = 0;
			splitText_ = SplitUTF8(steps_[currentIndex_].text);
			if (steps_[currentIndex_].onEnter) steps_[currentIndex_].onEnter();
		}
	}
}

void Tutorial::Draw() const
{
	if (!active_ || currentIndex_ < 0 || currentIndex_ >= static_cast<int>(steps_.size())) return;

	const int screenW = Application::SCREEN_SIZE_X;
	const int screenH = Application::SCREEN_SIZE_Y;
	const int margin = 24;
	const int boxH = 200;
	int x0 = margin, y0 = screenH - boxH - margin, x1 = screenW - margin, y1 = screenH - margin;
	const int faceW = 120;
	const int faceX = x0 + 12;

	const int bubbleX = x0 + faceW + 24;
	const int bubbleY = y0 + 20;
	const int bubbleW = x1 - bubbleX - 18;
	const int bubbleH = boxH - 40;

	auto fillBox = [](int x1, int y1, int x2, int y2, int r, int g, int b, int alpha = 255)
		{
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
			DrawBox(x1, y1, x2, y2, GetColor(r, g, b), true);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		};
	auto strokeBox = [](int x1, int y1, int x2, int y2, int r, int g, int b)
		{
			DrawBox(x1, y1, x2, y2, GetColor(r, g, b), false);
	};

	// 吹き出し（背景＋枠）
	fillBox(bubbleX, bubbleY, bubbleX + bubbleW, bubbleY + bubbleH, 30, 40, 60, 220);
	strokeBox(bubbleX, bubbleY, bubbleX + bubbleW, bubbleY + bubbleH, 200, 200, 220);

	// 尾
	const int tailW = 20, tailH = 28;
	const int tailX = bubbleX - tailW + 2;
	const int tailY = bubbleY + bubbleH / 2 - tailH / 2;
	fillBox(tailX, tailY, tailX + tailW, tailY + tailH, 30, 40, 60, 220);
	strokeBox(tailX, tailY, tailX + tailW, tailY + tailH, 200, 200, 220);

	// 顔画像または簡易キャラ描画
	if (enoguHandle_ >= 0)
	{
		int gw = 0, gh = 0;
		if (GetGraphSize(enoguHandle_, &gw, &gh) == 0 && gw > 0 && gh > 0)
		{
			float scale = static_cast<float>(faceW) / static_cast<float>(gh);
			int drawW = static_cast<int>(gw * scale);
			int drawH = static_cast<int>(gh * scale);
			int drawY = y0 + (boxH - drawH) / 2;
			DrawExtendGraph(faceX, drawY, faceX + drawW, drawY + drawH, enoguHandle_, TRUE);
		}
		else
		{
			// フォールバック
			DrawGraph(faceX, y0 + 20, enoguHandle_, TRUE);
		}
	}
	else
	{
		// 簡易キャラ
		int cx = faceX;
		int cy = y0 + 58;
		fillBox(cx + 12, cy + 0, cx + 52, cy + 32, 200, 40, 40, 255); // 頭
		fillBox(cx + 18, cy + 36, cx + 34, cy + 72, 255, 255, 255, 255); // 体
		fillBox(cx + 22, cy + 44, cx + 30, cy + 60, 200, 40, 40, 255); // 前面
		fillBox(cx + 6, cy + 40, cx + 16, cy + 46, 200, 40, 40, 255); // 左腕
		fillBox(cx + 36, cy + 40, cx + 46, cy + 46, 200, 40, 40, 255); // 右腕
		fillBox(cx + 20, cy + 72, cx + 24, cy + 88, 0, 0, 0, 255); // 左足
		fillBox(cx + 28, cy + 72, cx + 32, cy + 88, 0, 0, 0, 255); // 右足
		// 目
		fillBox(cx + 20, cy + 8, cx + 24, cy + 12, 0, 0, 0, 255);
		fillBox(cx + 28, cy + 8, cx + 32, cy + 12, 0, 0, 0, 255);
	}
	const auto& cur = steps_[currentIndex_];
	int visible = revealIndex_;
	if (visible > static_cast<int>(splitText_.size())) visible = static_cast<int>(splitText_.size());
	std::string sub;
	sub.reserve(visible * 3);
	for (int i = 0; i < visible; ++i) sub += splitText_[i];

	int textX = bubbleX + 16, textY = bubbleY + 16;
	DrawString(textX, textY, sub.c_str(), GetColor(235, 235, 235));

	if (visible >= static_cast<int>(splitText_.size()) && ((animCounter_ / 12) % 2 == 0))
	{
		const char* cont = "▼";
		DrawString(bubbleX + bubbleW - 28, bubbleY + bubbleH - 28, cont, GetColor(200, 200, 140));
	}

	int total = static_cast<int>(steps_.size());

	//const char* hint = "行動を行うまで次に進みません。";
	//DrawString(x1 - 420, y0 + 56, hint, GetColor(220, 200, 100));

	char progressMsg[64];
	snprintf(progressMsg, sizeof(progressMsg), "進行: %d / %d", currentIndex_ + 1, total);
	DrawString(x1 - 160, y1 - 52, progressMsg, GetColor(180, 180, 180));
}