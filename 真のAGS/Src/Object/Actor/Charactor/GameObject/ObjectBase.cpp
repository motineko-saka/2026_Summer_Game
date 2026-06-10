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
{
	isAnswerPosition_ = false;
	ansVec_ = ansVec;
	viewWorld_ = world;
	world_ = world;
	type_ = type;
	pushPow_ = { 0.0f, 0.0f, 0.0f };
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
		transform_.SetModel(resMng_.Load(ResourceManager::SRC::WOODBOX).handleId_);
		break;
	case OBJECT_TYPE::AKEG:
		transform_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::BARREL));
		break;
	case OBJECT_TYPE::SCENE_PROP:
		transform_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::WALL));
		break;
	case OBJECT_TYPE::BUTTOM:
		transform_.SetModel(resMng_.Load(ResourceManager::SRC::BUTTON).handleId_);
		break;
	case OBJECT_TYPE::DEFAULT:
		transform_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::BARREL));
		break;
	default:
		break;
	}
}

void ObjectBase::InitTransform(void)
{
	// モデルの位置
	transform_.scl = defaultScale_;
	transform_.quaRot = Quaternion::Identity();

	transform_.quaRotLocal = Quaternion::Identity();

	transform_.pos = { -1000.0f, 80.0f, -10.0f };
	transform_.Update();
}

void ObjectBase::InitCollider(void)
{
	MV1SetupCollInfo(transform_.modelId);

	// モデルのコライダ
	ColliderModel* colModel =
		new ColliderModel(ColliderBase::TAG::OBJECT, &transform_);
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::MODEL), colModel);

	// 主に地面との衝突で使用する線分コライダ
	ColliderLine* colLine = new ColliderLine(
		ColliderBase::TAG::PLAYER, &transform_,
		COL_LINE_START_LOCAL_POS, COL_LINE_END_LOCAL_POS);
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::LINE), colLine);

	// 主に壁や木などの衝突で仕様するカプセルコライダ
	ColliderCapsule* colCapsule = new ColliderCapsule(
		ColliderBase::TAG::PLAYER, &transform_,
		COL_CAPSULE_TOP_LOCAL_POS, COL_CAPSULE_DOWN_LOCAL_POS,
		COL_CAPSULE_RADIUS);
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::CAPSULE), colCapsule);
}
void ObjectBase::InitAnimation(void)
{
	animController_ = new AnimationController(transform_.modelId);
}

void ObjectBase::InitPost(void)
{
}

void ObjectBase::UpdateProcess(void)
{
	if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_C))
	{
		world_ = (world_ == SceneBase::WORLD::LEFT) ? SceneBase::WORLD::RIGHT : SceneBase::WORLD::LEFT;
		transform_.pos.x = -transform_.pos.x;
	}

	isGrabbed_ = false; // デフォルトはつかまれていない

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

void ObjectBase::UpdateProcessPost(void)
{
}

void ObjectBase::CheckAnswer(void)
{
	bool isHit = false;

	float distance1 = VSize(VSub(transform_.pos, ansVec_));

	isAnswerPosition_ = (distance1 < 150.0f);
}