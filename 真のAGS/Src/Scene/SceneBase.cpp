#include "../Manager/ResourceManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/StageManager.h"
#include "../Object/Actor/Wall.h"
#include "SceneBase.h"

SceneBase::SceneBase(void)
	:
	resMng_(ResourceManager::GetInstance())
{
}

SceneBase::~SceneBase(void)
{
}

void SceneBase::Init(void)
{
}

void SceneBase::Load(void)
{
}

void SceneBase::Update(void)
{
}

void SceneBase::Draw(void)
{
}

void SceneBase::Release(void)
{
}

void SceneBase::CreateWall(StageManager& stageM)
{
	// ‚S–Ê•Çì¬
	bool isWallCreate = false;
	for (auto& stage : stageM.GetStage())
	{
		if (isWallCreate) continue;
		auto& bb = stage->GetBoundingBox();
		// ^‚ñ’†
		walls_.push_back(std::make_unique<Wall>(VECTOR(0, 0, 0)));

		// ‰œè‘O
		walls_.push_back(std::make_unique<Wall>(VECTOR(0, 0, bb.minPos.z), true));
		walls_.push_back(std::make_unique<Wall>(VECTOR(0, 0, bb.maxPos.z), true));

		// ¶‰E
		walls_.push_back(std::make_unique<Wall>(VECTOR(bb.minPos.x, 0, 0)));
		walls_.push_back(std::make_unique<Wall>(VECTOR(bb.maxPos.x, 0, 0)));

		isWallCreate = true;
	}

	for (auto& wall : walls_)
	{
		wall->Init();
	}
}

