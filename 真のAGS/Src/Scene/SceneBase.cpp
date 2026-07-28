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
    bool isWallCreate = false;
    for (auto& stage : stageM.GetStage())
    {
        if (isWallCreate) continue;

        auto& bb = stage->GetBoundingBox();
		const auto& stageCutPos = stage->GetStageCutPos();

        float centerX = (bb.minPos.x + bb.maxPos.x) * 0.5f;
        float centerZ = (bb.minPos.z + bb.maxPos.z) * 0.5f;

        // 真ん中
        walls_.push_back(std::make_unique<Wall>(VECTOR(centerX, 0, centerZ)));

       //walls_.push_back(std::make_unique<Wall>(VECTOR(stageCutPos, 0, bb.minPos.z), true));
       //walls_.push_back(std::make_unique<Wall>(VECTOR(centerX, 0, bb.maxPos.z), true));

        //// 奥手前（Z固定、Xはステージ中央）
        //walls_.push_back(std::make_unique<Wall>(VECTOR(centerX, 0, bb.minPos.z), true));
        //walls_.push_back(std::make_unique<Wall>(VECTOR(centerX, 0, bb.maxPos.z), true));

        //// 左右（X固定、Zはステージ中央）
        //walls_.push_back(std::make_unique<Wall>(VECTOR(bb.minPos.x, 0, centerZ)));
        //walls_.push_back(std::make_unique<Wall>(VECTOR(bb.maxPos.x, 0, centerZ)));

        isWallCreate = true;
    }

	for (auto& wall : walls_)
	{
		wall->Init();
	}
}

void SceneBase::CreateWallGame(StageManager& stageM)
{
    bool isWallCreate = false;
    for (auto& stage : stageM.GetStage())
    {
        if (isWallCreate) continue;

        auto& bb = stage->GetBoundingBox();
        const auto& stageCutPos = stage->GetStageCutPos();

        float centerX = (bb.minPos.x + bb.maxPos.x) * 0.5f;
        float centerZ = (bb.minPos.z + bb.maxPos.z) * 0.5f;

        stage->STAGE_CUT_NUM;

        // 真ん中
        walls_.push_back(std::make_unique<Wall>(VECTOR(centerX, 0, centerZ)));

        // ２分割
        for (int i = 0; i < 2; i++)
        {
            walls_.push_back(std::make_unique<Wall>(
                VECTOR(stageCutPos[i].x + -1500, stageCutPos[i].y, stageCutPos[i].z), true, true));

            walls_.push_back(std::make_unique<Wall>(
                VECTOR(stageCutPos[i].x + 1500, stageCutPos[i].y, stageCutPos[i].z), true, true));
        }
        //walls_.push_back(std::make_unique<Wall>(VECTOR(stageCutPos, 0, bb.minPos.z), true));
        //walls_.push_back(std::make_unique<Wall>(VECTOR(centerX, 0, bb.maxPos.z), true));

         //// 奥手前（Z固定、Xはステージ中央）
         //walls_.push_back(std::make_unique<Wall>(VECTOR(centerX, 0, bb.minPos.z), true));
         //walls_.push_back(std::make_unique<Wall>(VECTOR(centerX, 0, bb.maxPos.z), true));

         //// 左右（X固定、Zはステージ中央）
         //walls_.push_back(std::make_unique<Wall>(VECTOR(bb.minPos.x, 0, centerZ)));
         //walls_.push_back(std::make_unique<Wall>(VECTOR(bb.maxPos.x, 0, centerZ)));

        isWallCreate = true;
    }

    for (auto& wall : walls_)
    {
        wall->Init();
    }
}

