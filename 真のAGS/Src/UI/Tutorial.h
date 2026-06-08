#pragma once
#include <string>
#include <vector>
#include <functional>

class Tutorial
{
public:
	Tutorial();
	~Tutorial();

	void Init();
	void Start();
	void Update();
	void Draw() const;
	bool IsActive() const { return active_; }

	// 新 API: ステップを登録する（条件ラムダが true を返したら次へ）
	using ConditionFunc = std::function<bool()>;
	using OnEnterFunc = std::function<void()>;
	void ClearSteps();
	void AddStep(const std::string& text, ConditionFunc cond, OnEnterFunc onEnter = nullptr);

private:
	struct StepInfo
	{
		std::string text;
		ConditionFunc condition;
		OnEnterFunc onEnter;
	};

	std::vector<StepInfo> steps_;
	int currentIndex_;

	bool active_;
	int inputDelay_;                 // 入力デバウンス
	static constexpr int DELAY_MAX = 8;

	// 描画アニメーション用
	mutable int animCounter_;        // 描画アニメーション用カウンタ（Updateで増加）
};