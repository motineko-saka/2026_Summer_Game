#include "../../../../Manager/InputManager.h"
#include "../../../../Manager/ResourceManager.h"
#include "../../../../Utility/AsoUtility.h"
#include "../../../../Common/Quaternion.h"
#include "../../../../Object/Actor/Charactor/Player.h"
#include "Button.h"

Button::Button(SceneBase::WORLD world, VECTOR ansVec, OBJECT_TYPE type, Player& player)
	: 
	ObjectBase(world, ansVec, type),
	player_(player)
{
}

void Button::InitLoad(void)
{
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::BUTTON));
	
	isHoldable_ = true;
}

void Button::ObjectUpdateProcess(void)
{
	VECTOR objectPos = transform_.pos;

	bool isNearButton = false;

	for (auto& player : player_)
	{
		// プレイヤーとの距離チェック
		VECTOR playerPos = player.player_->GetTransform().pos;
		float distance1 = VSize(VSub(playerPos, objectPos));
		if (distance1 < 80.0f)
		{
			isNearButton = true;
		}
	}

	// ボタンの近くにいて、スペースキーか左ボタンが押されたら
	if (isNearButton &&
		(InputManager::GetInstance()->IsTrgDown(KEY_INPUT_F) || InputManager::GetInstance()->IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT)))
	{
		// ボタンが押されたときの処理（例：ゲームクリア、ドアが開くなど）

		isButtomPushed_ = true;
		obj.SetButtomPushed(true);

		objects_[2]->Release();
		objects_.erase(objects_.begin() + 2);
		for (auto& player : players_)
		{
			player.player_->HitColliderErase(4);
		}
		// 直接追加せず、一時リストに格納
		//ObjectBase* newObj = new ObjectBase(SceneBase::WORLD::LEFT, ANSWER_VECTOR_LENGTH[1], ObjectBase::OBJECT_TYPE::AKEG);
		//newObjects.push_back(newObj);
	}
}
