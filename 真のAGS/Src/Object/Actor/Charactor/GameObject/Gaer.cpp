#include "../../../../Manager/ResourceManager.h"
#include "../../../../Utility/AsoUtility.h"
#include "../../../../Common/Quaternion.h"
#include "Gaer.h"

Gaer::Gaer(SceneBase::WORLD world, VECTOR ansVec, OBJECT_TYPE type)
	: 
	ObjectBase(world, ansVec, type),
	gearRot_(0.0f)
{
}

void Gaer::InitLoad(void)
{
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::GEAR));
}

void Gaer::InitTransform(void)
{
}

void Gaer::InitCollider(void)
{
}

void Gaer::InitAnimation(void)
{
}

void Gaer::InitPost(void)
{
}

void Gaer::ObjectUpdateProcess(void)
{
	gearRot_ += 5.0f;

	transform_.quaRotLocal = Quaternion::AngleAxis(AsoUtility::Deg2RadD(gearRot_),
		AsoUtility::AXIS_Z);
}