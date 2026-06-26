#include "../../../Utility/AsoUtility.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/Resource.h"
#include "../../../Manager/SceneManager.h"
#include "../../Collider/ColliderModel.h"
#include "Stage.h"

Stage::Stage(int modelID)
	:
	stageID_(-1),
	StageBase()
{
	transform_.SetModel(modelID);
}

Stage::~Stage(void)
{
}

void Stage::InitLoad(void)
{
	//transform_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::TUTORIAL_STAGE));
}

void Stage::InitTransform(void)
{
	transform_.scl = STAGE_DEFAULT_SCALE;
	transform_.quaRot = Quaternion::Identity();

	transform_.quaRotLocal = Quaternion::Identity();

	transform_.pos = STAGE_DEFAULT_POS;
	transform_.Update();
}

void Stage::InitAnimation(void)
{
}

void Stage::InitPost(void)
{
	stageType_ = STAGE_TYPE::MAIN_STAGE;
}
