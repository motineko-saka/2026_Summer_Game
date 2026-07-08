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
	void AddStep(const std::string& text, ConditionFunc cond, OnEnterFunc onEnter = nullptr, ResourceManager::SRC face = ResourceManager::SRC::ENOGU);

private:
	struct StepInfo
	{
		std::string text;
		ConditionFunc condition;
		OnEnterFunc onEnter;
		ResourceManager::SRC faceSrc = ResourceManager::SRC::ENOGU;
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

	mutable int enoguHandle_ = -1;
	mutable Resource::TYPE enoguType_ = Resource::TYPE::NONE;
};