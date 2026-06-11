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

Object::Object(SceneBase::WORLD world, VECTOR ansVec, OBJECT_TYPE type)
	:
	CharactorBase(),
	isAnswerPosition_(false),
	isGrabbed_(false),
	isPushButtom_(false),
	world_(world),
	viewWorld_(world),
	ansVec_(ansVec),
	type_(type),
	pushPow_(AsoUtility::VECTOR_ZERO)
{
	//isAnswerPosition_ = false;
	//ansVec_ = ansVec;
	//viewWorld_ = world;
	//world_ = world;
	//type_ = type;
	//pushPow_ = { 0.0f, 0.0f, 0.0f };
}

Object::~Object()
{
}

void Object::Draw(void)
{
	ActorBase::Draw();
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

void Object::PushButton(void)
{
	
}

void Object::InitLoad(void)
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
	case OBJECT_TYPE::BUTTOM:
		transform_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::BUTTON));
		break;
	case OBJECT_TYPE::DEFAULT:
		transform_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::BARREL));
		break;
	case OBJECT_TYPE::PUSH_BUTTON:
		transform_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::CUBE));
		break;
	default:
		break;
	}
}

void Object::InitTransform(void)
{
	// モデルの位置
	transform_.scl = defaultScale_;
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
		world_ = (world_ == SceneBase::WORLD::LEFT) ? SceneBase::WORLD::RIGHT : SceneBase::WORLD::LEFT;
		transform_.pos.x = -transform_.pos.x;
	}

	isGrabbed_ = false; // デフォルトはつかまれていない
	isPushButtom_ = false; // デフォルトはボタンが踏まれていない

	// PUSH_BUTTON タイプの場合、プレイヤーが乗っているか判定
	if (type_ == OBJECT_TYPE::PUSH_BUTTON)
	{
		// hitColliders_ に登録されているコライダをチェック
		for (const auto& hitCol : hitColliders_)
		{
			if (hitCol == nullptr) continue;

			// プレイヤーのコライダのみをチェック
			if (hitCol->GetTag() != ColliderBase::TAG::PLAYER) continue;

			const Transform* playerTransform = hitCol->GetFollow();
			if (playerTransform == nullptr) continue;

			// プレイヤーの足元から下方への線分を定義
			// （プレイヤーのコライダから相対位置を計算）
			VECTOR playerPos = playerTransform->pos;
			auto distance = 1000;

			if (playerPos.x < transform_.pos.x + distance &&
				playerPos.x > transform_.pos.x - distance &&
				playerPos.z < transform_.pos.z + distance &&
				playerPos.z > transform_.pos.z - distance)
			{
				int a = 0;
			}

			//playerFootPos.y -= 50.0f; // 足元の高さ

			//// ボタンの上面の高さ範囲
			//float buttonTopY = transform_.pos.y + 30.0f; // ボタンの上面
			//float buttonBottomY = transform_.pos.y - 20.0f;

			//// 水平距離でボタンの範囲内か判定
			//VECTOR diff = VSub(playerTransform->pos, transform_.pos);
			//float horizDist = sqrtf(diff.x * diff.x + diff.z * diff.z);

			//// プレイヤーの足がボタンの上面より下、ボタンのY範囲にあるか
			//// 且つ水平距離がボタンのサイズ内か
			//const float HORIZ_THRESHOLD = 80.0f; // ボタンのサイズに合わせて調整
			//const float VERT_THRESHOLD = 100.0f; // プレイヤーの足がボタンの高さ範囲内

			/*if (horizDist < HORIZ_THRESHOLD && 
				playerTransform->pos.y < buttonTopY &&
				playerTransform->pos.y > (buttonBottomY - VERT_THRESHOLD))
			{
				isPushButtom_ = true;
				break;
			}*/
		}
	}

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

void Object::UpdateProcessPost(void)
{
}

void Object::CheckAnswer(void)
{
	bool isHit = false;

	float distance1 = VSize(VSub(transform_.pos, ansVec_));

	isAnswerPosition_ = (distance1 < 150.0f);
}