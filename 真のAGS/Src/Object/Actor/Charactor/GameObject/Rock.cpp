#include "../../../Collider/ColliderLine.h"
#include "../../../Collider/ColliderCapsule.h"
#include "../../../Collider/ColliderModel.h"
#include "../../../../Manager/ResourceManager.h"
#include "../../../../Utility/AsoUtility.h"
#include "../../../../Common/Quaternion.h"
#include "Rock.h"

Rock::Rock(SceneBase::WORLD world, VECTOR ansVec, OBJECT_TYPE type)
	:
	ObjectBase(world, ansVec, type)
{
}

void Rock::InitLoad(void)
{
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::ROCK));
	isHoldable_ = true;

}

void Rock::InitObjCol(void)
{
	// モデルのコライダ
	tag_ = ColliderBase::TAG::KINOKO;
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

		// 3D距離を計算
		float distance = VSize(diff);
		const float BUTTON_TRIGGER_DISTANCE = 100.0f; // 適切な値に調整

		if (distance < BUTTON_TRIGGER_DISTANCE)
		{
			// 踏んだ時の処理
			isRockExist_ = false;
			break;
		}
	}
}