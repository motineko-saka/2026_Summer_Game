#include "../../../Utility/AsoUtility.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/Resource.h"
#include "../../../Manager/SceneManager.h"
#include "../../Collider/ColliderModel.h"
#include "AnswerStage.h"

AnswerStage::AnswerStage(void)
	:
	StageBase()
{
}

AnswerStage::~AnswerStage(void)
{
}

void AnswerStage::InitLoad(void)
{
	transform_.SetModel(resMng_.Load(ResourceManager::SRC::MAIN_STAGE).handleId_);
}

void AnswerStage::InitTransform(void)
{
	transform_.scl = STAGE_DEFAULT_SCALE;
	transform_.quaRot = Quaternion::Identity();

	transform_.quaRotLocal = Quaternion::Identity();

	transform_.pos = STAGE_DEFAULT_POS;
	transform_.pos.y -= 200.0f;
	transform_.Update();
}

void AnswerStage::InitAnimation(void)
{
}

void AnswerStage::InitPost(void)
{
	stageType_ = STAGE_TYPE::ANSWER_STAGE;
}
