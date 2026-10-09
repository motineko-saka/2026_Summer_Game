#include "Rock.h"
#include "../../../Collider/ColliderLine.h"
#include "../../../Collider/ColliderCapsule.h"
#include "../../../Collider/ColliderModel.h"
#include "../../../../Manager/ResourceManager.h"
#include "../../../../Utility/AsoUtility.h"
#include "../../../../Common/Quaternion.h"

Rock::Rock(SceneBase::WORLD world, VECTOR ansVec, OBJECT_TYPE type)
	:
	ObjectBase(world, ansVec, type)
{
}

void Rock::Draw()
{
	if(isRockExist_)
	{
		ActorBase::Draw();
	}
}

void Rock::InitLoad(void)
{
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::ROCK));
	isHoldAble_ = true;
}

void Rock::InitObjCol(void)
{
	tag_ = ColliderBase::TAG::OBJECT;

	capsule_radius_ = 100.0f;
}

void Rock::InitPost(void)
{
	isRockExist_ = true;
}

void Rock::ObjectUpdateProcess(void)
{
	for (const auto& hitCol : hitColliders_)
	{
		if (hitCol == nullptr) continue;
		if (hitCol->GetTag() != ColliderBase::TAG::KINOKO) continue;

		const Transform* objectTransform = hitCol->GetFollow();
		if (objectTransform == nullptr) continue;

		VECTOR objectPos = objectTransform->pos;
		VECTOR diff = VSub(objectPos, transform_.pos);

		// 3D‹——£‚ðŒvŽZ
		float distance = VSize(diff);
		const float BUTTON_TRIGGER_DISTANCE = 125.0f;

		if (distance < BUTTON_TRIGGER_DISTANCE)
		{
			// “¥‚ñ‚¾Žž‚Ìˆ—
			isRockExist_ = false;
			break;
		}
	}
}