#pragma once
#include <string>
#include <vector>
#include <functional>
#include "../Manager/ResourceManager.h"

class Tutorial
{
public:
	using ConditionFunc = std::function<bool()>;
	using OnEnterFunc = std::function<void()>;

	Tutorial();
	~Tutorial();

	void Init();
	void Start();
	void Update();
	void Draw() const;
	bool IsActive() const { return active_; }

	void ClearSteps();
	// 画像ハンドルをステップごとに指定できるように第4引数を追加
	void AddStep(const std::string& text, ConditionFunc cond, OnEnterFunc onEnter = nullptr, int enoguHandle = -1);

private:
	struct StepInfo
	{
		std::string text;
		ConditionFunc condition;
		OnEnterFunc onEnter;
		int enoguHandle = -1; // ステップ固有の絵の具ハンドル
	};

	std::vector<StepInfo> steps_;
	int currentIndex_ = -1;

	bool active_ = false;
	int inputDelay_ = 0;
	static constexpr int DELAY_MAX = 8;

	mutable int animCounter_ = 0;

	mutable int revealIndex_ = 0;
	mutable int typeTick_ = 0;
	static constexpr int TYPE_SPEED = 3;

	mutable std::vector<std::string> splitText_;

	// Yu GothicUIを使）
	int fontHandle_ = -1;
	int fontSize_ = 20;        // フォントの高さ
	int lineSpacing_ = 4;      // 行間ピクセル
	int bubblePaddingY_ = 8;   // 吹き出し上下の余白
	int minBubbleH_ = 64;      // 最小の吹き出し高さ
	int maxBubbleH_ = 200;     // 最大の吹き出し高さ）

	// 描画レイアウト
	struct Layout
	{
		int screenW;
		int screenH;
		int margin;
		int boxH;     
		int x0, y0, x1, y1;
		int faceW;
		int faceX;
		int bubbleX, bubbleY, bubbleW, bubbleH;
		int tailW, tailH, tailX, tailY;
	};

	Layout ComputeLayout() const;
	static void FillBox(int x1, int y1, int x2, int y2, int r, int g, int b, int alpha = 255);
	static void StrokeBox(int x1, int y1, int x2, int y2, int r, int g, int b);
	static void DrawEnogu(int handle, const Layout& l);
};