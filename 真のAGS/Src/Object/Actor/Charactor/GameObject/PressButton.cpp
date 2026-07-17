#include "../../../../Manager/ResourceManager.h"
#include "../../../../Utility/AsoUtility.h"
#include "../../../../Common/Quaternion.h"
#include "PressButton.h"

PressButton::PressButton(SceneBase::WORLD world, VECTOR ansVec, OBJECT_TYPE type)
	:
	ObjectBase(world, ansVec, type)
{
}

void PressButton::InitLoad(void)
{
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::PRESS_BUTTON));
}

void PressButton::ObjectUpdateProcess(void)
{
	// ボタンの処理
	for (const auto& hitCol : hitColliders_)
	{
		if (hitCol == nullptr) continue;
		if (hitCol->GetTag() != ColliderBase::TAG::PLAYER) continue;

		const Transform* playerTransform = hitCol->GetFollow();
		if (playerTransform == nullptr) continue;

		VECTOR playerPos = playerTransform->pos;
		VECTOR diff = VSub(playerPos, transform_.pos);

		// 3D距離を計算
		float distance = VSize(diff);
		const float BUTTON_TRIGGER_DISTANCE = 100.0f; // 適切な値に調整

		if (distance < BUTTON_TRIGGER_DISTANCE)
		{
			// 踏んだ時の処理
			isPressButton_ = true;
			break;
		}
	}
}