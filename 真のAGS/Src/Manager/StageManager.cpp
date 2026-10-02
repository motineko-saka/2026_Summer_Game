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
	if (!stages_.empty())
	{
		Release();
	}

	int modelID;

	if (scene_ == SceneManager::SCENE::TUTORIAL)
	{
		modelID = ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::TUTORIAL_STAGE);
	}
	else
	{
		modelID = ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::MAIN_STAGE);
	}

	for(int i = 0; i < STAGE_NUM; i++)
	{
		StageBase* s;
		if (static_cast<StageBase::STAGE_TYPE>(i) == StageBase::STAGE_TYPE::MAIN_STAGE)
		{
			s = new Stage(modelID);
		}
		else
		{
			s = new AnswerStage();
		}
		
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
		stage->DrawDebug();
	}

}

void StageManager::DrawDebug(void)
{
}

void StageManager::Release(void)
{
	for (auto stage : stages_)
	{
		if (stage)
		{
			stage->Release();
			delete stage;
		}
	}
	stages_.clear();
}