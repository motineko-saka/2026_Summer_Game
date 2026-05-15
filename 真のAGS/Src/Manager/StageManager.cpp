#include "StageManager.h"
#include "../Object/Actor/Stage/Stage.h"
#include "../Object/Actor/Stage/AnswerStage.h"

StageManager::StageManager()
{
}

StageManager::~StageManager(void)
{
}

void StageManager::InitStage(void)
{
	if (!stages_.empty())
	{
		Release();
	}

	for(int i = 0; i < STAGE_NUM; i++)
	{
		auto s = new Stage();
		s->Init();

		stages_.push_back(s);
	}
}

void StageManager::Update(void)
{
	for (const auto& stage : stages_)
	{
		stage->Update();
	}
}

void StageManager::Draw(void)
{
	for (const auto& stage : stages_)
	{
		stage->Draw();
	}
}

void StageManager::Release(void)
{
	for (const auto& stage : stages_)
	{
		stage->Release();
	}
	stages_.clear();
}