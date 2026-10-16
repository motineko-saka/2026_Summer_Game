#include "StageManager.h"
#include "../Object/Actor/Stage/Stage.h"
#include "../Object/Actor/Stage/AnswerStage.h"
#include "ResourceManager.h"

StageManager::StageManager(SceneManager::SCENE scene)
	:
	scene_(scene)
{
}

StageManager::~StageManager(void)
{
	Release();
}

void StageManager::InitStage(void)
{
	// Šù‚É‚ ‚ê‚Îì‚è’¼‚·
	Release();

	for (int i = 0; i < STAGE_NUM; i++)
	{
		StageBase* stage = CreateStage(static_cast<StageBase::STAGE_TYPE>(i));
		stage->Init();
		stages_.push_back(stage);
	}
}

StageBase* StageManager::CreateStage(StageBase::STAGE_TYPE type) const
{
	if (type == StageBase::STAGE_TYPE::MAIN_STAGE)
	{
		return new Stage(LoadStageModel());
	}
	return new AnswerStage();
}

int StageManager::LoadStageModel(void) const
{
	const auto src = (scene_ == SceneManager::SCENE::TUTORIAL)
		? ResourceManager::SRC::TUTORIAL_STAGE
		: ResourceManager::SRC::MAIN_STAGE;

	return ResourceManager::GetInstance().LoadModelDuplicate(src);
}

void StageManager::Update(void)
{
	for (StageBase* stage : stages_)
	{
		stage->Update();
	}
}

void StageManager::Draw(void)
{
	for (StageBase* stage : stages_)
	{
		stage->Draw();
		stage->DrawDebug();
	}
}

void StageManager::DrawDebug(void)
{
}

void StageManager::Release(void)
{
	for (StageBase* stage : stages_)
	{
		if (stage == nullptr) continue;

		stage->Release();
		delete stage;
	}
	stages_.clear();
}