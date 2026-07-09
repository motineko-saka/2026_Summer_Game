#include "../../../Collider/ColliderLine.h"
#include "../../../Collider/ColliderCapsule.h"
#include "../../../Collider/ColliderModel.h"
#include "../../../../Manager/ResourceManager.h"
#include "../../../../Utility/AsoUtility.h"
#include "../../../../Common/Quaternion.h"
#include "Object.h"

Object::Object(SceneBase::WORLD world, VECTOR ansVec, OBJECT_TYPE type)
	:
	ObjectBase(world, ansVec, type)
{
}

void Object::InitLoad(void)
{
	switch (type_)
	{
	case OBJECT_TYPE::WBOX:
		transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::WOODBOX));
		break;
	case OBJECT_TYPE::AKEG:
		transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::BARREL));
		break;
	case OBJECT_TYPE::SCENE_PROP:
		transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::WALL));
		break;
	case OBJECT_TYPE::DEFAULT:
		transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::CUBE));
		break;
	case OBJECT_TYPE::CHEST:
		transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::Chest));
		break;
	default:
		break;
	}
}

void Object::InitObjTrans(void)
{
	if (type_ == OBJECT_TYPE::CHEST)
	{
		transform_.quaRotLocal = Quaternion::AngleAxis(AsoUtility::Deg2RadD(90.0f),
			AsoUtility::AXIS_Y);

		isHoldable_ = true;
	}
}

void Object::InitPost(void)
{
}

void Object::ObjectUpdateProcess(void)
{
}
