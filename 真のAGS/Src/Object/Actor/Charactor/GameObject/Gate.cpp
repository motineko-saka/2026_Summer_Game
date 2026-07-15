#include "../../../../Manager/ResourceManager.h"
#include "../../../../Utility/AsoUtility.h"
#include "../../../../Common/Quaternion.h"
#include "Gate.h"

Gate::Gate(SceneBase::WORLD world, VECTOR ansVec, OBJECT_TYPE type)
	:
	ObjectBase(world, ansVec, type)
{
}

void Gate::InitLoad(void)
{
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::GATE));
	isHoldable_ = true;
}

void Gate::InitObjTrans(void)
{
	tag_ = ColliderBase::TAG::STAGE;
}

void Gate::InitPost(void)
{
	isAnswerObject_ = false;
	isGrav = false;
}

void Gate::ObjectUpdateProcess(void)
{
}
