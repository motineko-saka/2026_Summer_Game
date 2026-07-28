#include "../../../Collider/ColliderLine.h"
#include "../../../Collider/ColliderCapsule.h"
#include "../../../Collider/ColliderModel.h"
#include "../../../../Manager/EffekseerEffect.h"
#include "Panel.h"
#include "Board.h"

Panel::Panel(SceneBase::WORLD world, VECTOR ansVec, OBJECT_TYPE type)
	: ObjectBase(world, ansVec, type)
{
}

void Panel::InitLoad()
{
}

void Panel::InitObjCol()
{
}

void Panel::ObjectUpdateProcess()
{
 //   isButtonPushed_ = false;

 //   if (!IsHitPlayer())
 //   {
 //       stepLock_ = false;
 //       return;
 //   }

 //   //if (stepLock_)
 //   //    return;

 //   stepLock_ = true;
 //   isButtonPushed_ = true;

 //   if (board_)
 //   {
 //       board_->PushPanel(indexX_, indexY_);
 //   }

	//// プレイヤーのカプセルコライダを取得
	//int capsuleType = static_cast<int>(COLLIDER_TYPE::MODEL);
	//if (ownColliders_.count(capsuleType) == 0) return;

<<<<<<< HEAD
    ColliderModel* objModel =
        dynamic_cast<ColliderModel*>(ownColliders_.at(capsuleType).get());
    if (objModel == nullptr) return;
=======
	//ColliderModel* objModel =
	//	dynamic_cast<ColliderModel*>(ownColliders_.at(capsuleType));
	//if (objModel == nullptr) return;
>>>>>>> 7f92e3119438388ea932cf35fb2f9c2c36ed985f

	//// 衝突しているコライダをチェック
	//for (const auto& hitCol : hitColliders_)
	//{
	//	if (hitCol == nullptr) continue;

	//	// オブジェクトのカプセルコライダのみ対象
	//	if (hitCol->GetTag() != ColliderBase::TAG::PLAYER)continue;

	//	const ColliderCapsule* playerCapsule =
	//		dynamic_cast<const ColliderCapsule*>(hitCol);
	//	if (playerCapsule == nullptr) continue;

 //       // モデルとプレイヤーカプセルの衝突判定
 //       auto result =
 //           MV1CollCheck_Capsule(
 //               transform_.modelId,
 //               -1,
 //               playerCapsule->GetPosTop(),
 //               playerCapsule->GetPosDown(),
 //               playerCapsule->GetRadius()
 //           );

 //       if (result.HitNum > 0)
 //       {
 //           int a = 0;
 //       }
	//}
}

Board::ELEMENT Panel::GetElement() const
{
    if (board_)
    {
        return board_->GetElement(indexX_, indexY_);
    }
    return Board::ELEMENT::WATER;
}
