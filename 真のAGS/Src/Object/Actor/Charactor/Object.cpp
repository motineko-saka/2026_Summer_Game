#include "Object.h"
#include "../../../Utility/AsoUtility.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/Resource.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Application.h"
#include "../../Collider/ColliderLine.h"
#include "../../Collider/ColliderCapsule.h"
#include "../../Collider/ColliderModel.h"
#include "../../../Manager/InputManager.h"

Object::Object(GameScene::WORLD world)
{
	viewWorld_ = world;
	world_ = world;
	pushPow_ = { 0.0f, 0.0f, 0.0f };
}

Object::~Object()
{
}


void Object::Draw(void)
{
	if (world_ == viewWorld_)
	{
		ActorBase::Draw();
	}
}

void Object::Release(void)
{
	ActorBase::Release();
}

void Object::Push(const VECTOR& direction, float speed)
{
	// 押す力を加算
	pushPow_ = VAdd(pushPow_, VScale(direction, speed));
}

void Object::InitLoad(void)
{
	transform_.SetModel(resMng_.Load(ResourceManager::SRC::CUBE).handleId_);
}

void Object::InitTransform(void)
{
	// モデルの位置
	transform_.scl = { 0.5f, 0.5f, 0.5f };
	transform_.quaRot = Quaternion::Identity();

	transform_.quaRotLocal = Quaternion::Identity();

	transform_.pos = { -1000.0f, 80.0f, -10.0f };
	transform_.Update();
}

void Object::InitCollider(void)
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
void Object::InitAnimation(void)
{
	animController_ = new AnimationController(transform_.modelId);	
}

void Object::InitPost(void)
{
}

void Object::UpdateProcess(void)
{
	if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_C))
	{
		world_ = (world_ == GameScene::WORLD::LEFT) ? GameScene::WORLD::RIGHT : GameScene::WORLD::LEFT;
		transform_.pos.x = -transform_.pos.x;
	}

	// 掴まれているコライダがあれば transform をそれに同期する
	for (const auto& ct : ownColliders_)
	{
		ColliderBase* col = ct.second;

		if (col == nullptr) continue;

		const Transform* follow = col->GetFollow();
		// ハードゲイ が無い、または自分自身を追従先にしている場合は無視
		if (follow == nullptr || follow == &transform_) continue;

		// コライダのローカル位置をワールド座標に変換して合わせる

		const VECTOR localPos = col->GetLocalPos();
		const VECTOR worldPos = VAdd(follow->pos, follow->quaRot.PosAxis(localPos));
		transform_.pos = worldPos;
	
		// 回転・スケールを追従
		transform_.quaRot = follow->quaRot;
		//transform_.scl = follow->scl;

		// 掴まれている間はきんに君を無効化して終了
		pushPow_ = { 0.0f, 0.0f, 0.0f };
		break;
	}

	// 押された力を位置に適用（減衰）
	transform_.pos = VAdd(transform_.pos, pushPow_);
	pushPow_ = VScale(pushPow_, PUSH_RESISTANCE);

	CollisionCapsule();
	transform_.Update();
}

void Object::UpdateProcessPost(void)
{
	
}
