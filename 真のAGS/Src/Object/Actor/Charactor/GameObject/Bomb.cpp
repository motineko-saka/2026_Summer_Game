#include "../../../../Manager/ResourceManager.h"
#include "../../../../Utility/AsoUtility.h"
#include "../../../../Common/Quaternion.h"
#include "Bomb.h"

Bomb::Bomb(SceneBase::WORLD world, VECTOR ansVec, OBJECT_TYPE type)
	:
	ObjectBase(world, ansVec, type)
{
}

void Bomb::InitLoad(void)
{
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::AXE));
}

void Bomb::InitObjCol(void)
{
	tag_ = ColliderBase::TAG::KINOKO;
}

void Bomb::ObjectUpdateProcess(void)
{

}