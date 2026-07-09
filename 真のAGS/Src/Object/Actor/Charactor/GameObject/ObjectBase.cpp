#include "ObjectBase.h"
#include "../../../../Utility/AsoUtility.h"
#include "../../../../Manager/ResourceManager.h"
#include "../../../../Manager/Resource.h"
#include "../../../../Manager/SceneManager.h"
#include "../../../../Application.h"
#include "../../../Collider/ColliderLine.h"
#include "../../../Collider/ColliderCapsule.h"
#include "../../../Collider/ColliderModel.h"
#include "../../../../Manager/InputManager.h"

ObjectBase::ObjectBase(SceneBase::WORLD world, VECTOR ansVec, OBJECT_TYPE type)
	:
	CharactorBase(),
	isAnswerPosition_(false),
	isGrabbed_(false),
	isPushButton_(false),
	world_(world),
	viewWorld_(world),
	ansVec_(ansVec),
	type_(type),
	pushPow_(AsoUtility::VECTOR_ZERO)
{
}

ObjectBase::~ObjectBase()
{
}

void ObjectBase::Draw(void)
{
	ActorBase::Draw();
}

void ObjectBase::Release(void)
{
	ActorBase::Release();
}

void ObjectBase::Push(const VECTOR& direction, float speed)
{
	// 押す力を加算
	pushPow_ = VAdd(pushPow_, VScale(direction, speed));
}

void ObjectBase::InitLoad(void)
{
	switch (type_)
	{
	case OBJECT_TYPE::WBOX:
		transform_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::WOODBOX));
		break;
	case OBJECT_TYPE::AKEG:
		transform_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::BARREL));
		break;
	case OBJECT_TYPE::SCENE_PROP:
		transform_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::WALL));
		break;
	case OBJECT_TYPE::BUTTON:
		transform_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::BUTTON));
		break;
	case OBJECT_TYPE::DEFAULT:
		transform_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::CUBE));
		break;
	case OBJECT_TYPE::PRESS_BUTTON:
		transform_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::CUBE));
		break;
	case OBJECT_TYPE::GEAR:
		transform_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::GEAR));
		break;
	case OBJECT_TYPE::ROCK:
		transform_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::ROCK));
		break;
	case OBJECT_TYPE::KINOKO:
		transform_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::KINOKO));
	case OBJECT_TYPE::CHEST:
		transform_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::Chest));
		break;
	default:
		break;
	}

	tag_ = ColliderBase::TAG::OBJECT;
}

void ObjectBase::InitTransform(void)
{
	// モデルの位置
	transform_.scl = defaultScale_;
	transform_.quaRot = Quaternion::Identity();

	transform_.quaRotLocal = Quaternion::Identity();

	if(type_ == OBJECT_TYPE::CHEST)
	{
		transform_.quaRotLocal = Quaternion::AngleAxis(AsoUtility::Deg2RadD(90.0f),
			AsoUtility::AXIS_Y);
	}

	transform_.pos = { -1000.0f, 80.0f, -10.0f };
	transform_.Update();
}

void ObjectBase::InitCollider(void)
{
	MV1SetupCollInfo(transform_.modelId);

	tag_ = ColliderBase::TAG::OBJECT;

	InitObjCol();

	// モデルのコライダ
	ColliderModel* colModel =
		new ColliderModel(tag_, &transform_);
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::MODEL), colModel);

	// 主に地面との衝突で使用する線分コライダ
	ColliderLine* colLine = new ColliderLine(
		ColliderBase::TAG::PLAYER, &transform_,
		COL_LINE_START_LOCAL_POS, COL_LINE_END_LOCAL_POS);
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::LINE), colLine);

	// 主に壁や木などの衝突で仕様するカプセルコライダ
	ColliderCapsule* colCapsule = new ColliderCapsule(
		tag_, &transform_,
		COL_CAPSULE_TOP_LOCAL_POS, COL_CAPSULE_DOWN_LOCAL_POS,
		COL_CAPSULE_RADIUS);
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::CAPSULE), colCapsule);

	if (type_ == OBJECT_TYPE::BUTTON || type_ == OBJECT_TYPE::CHEST)
	{
		if (colLine) colLine->SetGrabbable(false);
		if (colCapsule) colCapsule->SetGrabbable(false);
		if (colModel) colModel->SetGrabbable(false);
	}
}

void ObjectBase::InitAnimation(void)
{
	animController_ = new AnimationController(transform_.modelId);
}

void ObjectBase::InitPost(void)
{
	if(type_ == OBJECT_TYPE::GEAR) isGrav = false;
}

void ObjectBase::UpdateProcess(void)
{
	if(isRot_)
	{
		gearRot_ += 5.0f;
	}

	if (transform_.pos.y < -2000.0f)
	{
		transform_.pos = { -1000.0f, 80.0f, -10.0f };
	}

	//if (!(type_ == OBJECT_TYPE::BUTTON ||
	//	type_ == OBJECT_TYPE::PRESS_BUTTON))
	//{
	//	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_C))
	//	{
	//		world_ = (world_ == SceneBase::WORLD::LEFT) ? SceneBase::WORLD::RIGHT : SceneBase::WORLD::LEFT;
	//		transform_.pos.x = -transform_.pos.x;
	//	}
	//}

	isGrabbed_ = false; // デフォルトはつかまれていない
	isPushButton_ = false; // デフォルトはボタンが踏まれていない

	// PUSH_BUTTON タイプの場合、プレイヤーが乗っているか判定
	if (type_ == OBJECT_TYPE::PRESS_BUTTON)
	{
		PressButton();
		//for (const auto& hitCol : hitColliders_)
		//{
		//	if (hitCol == nullptr) continue;
		//	if (hitCol->GetTag() != ColliderBase::TAG::PLAYER) continue;
		//
		//	const Transform* playerTransform = hitCol->GetFollow();
		//	if (playerTransform == nullptr) continue;
		//
		//	VECTOR playerPos = playerTransform->pos;
		//	VECTOR diff = VSub(playerPos, transform_.pos);
		//	
		//	// 3D距離を計算
		//	float distance = VSize(diff);
		//	const float BUTTON_TRIGGER_DISTANCE = 100.0f; // 適切な値に調整
		//
		//	if (distance < BUTTON_TRIGGER_DISTANCE)
		//	{
		//		// 踏んだ時の処理
		//		isPushButtom_ = true;
		//		break;
		//	}
		//}
	}

	ObjectUpdateProcess();

	// 掴まれているコライダがあれば transform をそれに同期する
	for (const auto& ct : ownColliders_)
	{
		ColliderBase* col = ct.second;
		if (col == nullptr) continue;

		const Transform* follow = col->GetFollow();
		if (follow == nullptr || follow == &transform_) continue;

		// ここに到達 = つかまれている
		isGrabbed_ = true;

		const VECTOR localPos = col->GetLocalPos();
		const VECTOR worldPos = VAdd(follow->pos, follow->quaRot.PosAxis(localPos));
		transform_.pos = worldPos;
		transform_.quaRot = follow->quaRot;
		pushPow_ = { 0.0f, 0.0f, 0.0f };
		break;
	}

	// 押された力を位置に適用（減衰）
	transform_.pos = VAdd(transform_.pos, pushPow_);
	pushPow_ = VScale(pushPow_, PUSH_RESISTANCE);

	CollisionCapsule();
	transform_.Update();

	// つかまれていなかったら答えチェック
	if (!isGrabbed_)
	{
		CheckAnswer();
	}
}

void ObjectBase::PushButton(void)
{

}

void ObjectBase::PressButton(void)
{
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
			isPushButton_ = true;
			break;
		}
	}
}

void ObjectBase::UpdateProcessPost(void)
{
}

void ObjectBase::CheckAnswer(void)
{
	bool isHit = false;

	float distance1 = VSize(VSub(transform_.pos, ansVec_));

	isAnswerPosition_ = (distance1 < 150.0f);
}