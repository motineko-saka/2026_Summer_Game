#include "../../../../Manager/ResourceManager.h"
#include "../../../../Utility/AsoUtility.h"
#include "../../../../Common/Quaternion.h"
#include "Gaer.h"

Gaer::Gaer(SceneBase::WORLD world, VECTOR ansVec, OBJECT_TYPE type, ObjectBase& object)
	:
	ObjectBase(world, ansVec, type),
	gearRot_(0.0f),
	object_(object)
{
}

void Gaer::InitLoad(void)
{
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::GEAR));
	isHoldable_ = true;
}

void Gaer::InitPost(void)
{
	isAnswerObject_ = false;
	isGrav = false;
}

void Gaer::ObjectUpdateProcess(void)
{
	if (isRot_)
	{
		gearRot_ += 5.0f;
	}

	transform_.quaRotLocal = Quaternion::AngleAxis(AsoUtility::Deg2RadD(gearRot_),
		AsoUtility::AXIS_Z);

	// 歯車距離処理（後で消す）
	auto& objectPos = transform_.pos;

	// ステージモデルのコライダーをプレイヤーに登録
	VECTOR objectPos1 = object_.GetTransform().pos;

	float distance1 = VSize(VSub(objectPos1, transform_.pos));
	bool hit = (distance1 < 60.0f);
	if (hit)
	{
		isRot_ = true;
	}
}