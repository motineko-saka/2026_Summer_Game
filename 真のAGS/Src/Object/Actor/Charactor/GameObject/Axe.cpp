#include "../../../../Manager/ResourceManager.h"
#include "../../../../Utility/AsoUtility.h"
#include "../../../../Common/Quaternion.h"
#include "Axe.h"

Axe::Axe(SceneBase::WORLD world, VECTOR ansVec, OBJECT_TYPE type)
	:
	ObjectBase(world, ansVec, type)
{
}

void Axe::InitLoad(void)
{
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::AXE));
}

void Axe::InitObjCol(void)
{
	tag_ = ColliderBase::TAG::KINOKO;
	capsule_r = 5.0f;
}

void Axe::ObjectUpdateProcess(void)
{
}