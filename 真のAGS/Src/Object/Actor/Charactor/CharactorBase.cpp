#include "../../../Utility/AsoUtility.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Application.h"
#include "../../Collider/ColliderBase.h"
#include "../../Collider/ColliderLine.h"
#include "../../Collider/ColliderModel.h"
#include "../../Collider/ColliderCapsule.h"
#include "../../../Manager/Resource.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Audio/AudioManager.h"
#include "CharactorBase.h"

CharactorBase::CharactorBase(void)
	:
	ActorBase(),
	animController_(nullptr),
	moveDir_(AsoUtility::VECTOR_ZERO),
	moveSpeed_(0.0f),
	movePow_(AsoUtility::VECTOR_ZERO),
	imgShadow_(0),
	isAnim_(false),
	isJump_(false),
	jumpPow_(AsoUtility::VECTOR_ONE),
	prevPos_(AsoUtility::VECTOR_ONE),
	stepJump_(0.0f)
{
}

CharactorBase::~CharactorBase(void)
{
}

void CharactorBase::Update(void)
{
	// 移動前座標を更新
	prevPos_ = transform_.pos;

	// 各キャラクターごとの更新処理
	UpdateProcess();

	// 移動方向に応じた遅延回
	DelayRotate();

	if (isGrav)
	{
		// 重力による移動量
		CalcGravityPow();
	}

	// 衝突判定前準備
	CollisionReserve();

	// 衝突判定
	Collision();

	// モデル制御更新
	transform_.Update();

	// アニメーション再生
	animController_->Update();

	// 各キャラクターごとの更新後処理
	UpdateProcessPost();

}

void CharactorBase::Draw(void)
{
	// 基底クラスの描画処理
	ActorBase::Draw();
}

void CharactorBase::Release(void)
{
	if (animController_ != nullptr)
	{
		animController_->Release();
		delete animController_;
	}

	ActorBase::Release();

	AudioManager::GetInstance()->DeleteInstance();
}

void CharactorBase::InitLoad(void)
{
	// 丸影画像
	imgShadow_ = resMng_.Load(ResourceManager::SRC::PLAYER_SHADOW).handleId_;

	AudioManager::GetInstance()->CreateInstance();
	AudioManager::GetInstance()->Init();
}

void CharactorBase::DelayRotate(void)
{
	// 移動方向から回転に変換する
	Quaternion goalRot = Quaternion::LookRotation(moveDir_);

	// 回転の補間
	transform_.quaRot =
		Quaternion::Slerp(transform_.quaRot, goalRot, 0.2f);
}

void CharactorBase::CalcGravityPow(void)
{
	// 重力方向
	VECTOR dirGravity = AsoUtility::DIR_D;

	// 重力の強さ
	float gravityPow = Application::GetInstance().GetGravityPow() * SceneManager::GetInstance()->GetDeltaTime();

	// 重力
	VECTOR gravity = VScale(dirGravity, gravityPow);
	jumpPow_ = VAdd(jumpPow_, gravity);

	// ジャンプ量を加算
	//transform_.pos = VAdd(transform_.pos, jumpPow_);
	// 重力速度の制限
	if (jumpPow_.y < MAX_FALL_SPEED)
	{
		jumpPow_.y = MAX_FALL_SPEED;
	}
}

void CharactorBase::Collision(void)
{
	// 移動処理
	transform_.pos = VAdd(transform_.pos, movePow_);

	// 衝突(カプセル)
	CollisionCapsule();

	// ジャンプ量を加算
	transform_.pos = VAdd(transform_.pos, jumpPow_);

	// 衝突(重力)
	CollisionGravity();
}

void CharactorBase::CollisionCapsule(void)
{
	int capsuleType = static_cast<int>(COLLIDER_TYPE::CAPSULE);
	if (ownColliders_.count(capsuleType) == 0) return;

	ColliderCapsule* colliderCapsule =
		dynamic_cast<ColliderCapsule*>(ownColliders_.at(capsuleType));
	if (colliderCapsule == nullptr) return;

	for (const auto& hitCol : hitColliders_)
	{
		if (hitCol->GetShape() != ColliderBase::SHAPE::MODEL) continue;
		const ColliderModel* colliderModel =
			dynamic_cast<const ColliderModel*>(hitCol);
		if (colliderModel == nullptr) continue;

		// プレイヤーの押し戻し
		colliderCapsule->PushBackAlongNormal(colliderModel, transform_, CNT_TRY_COLLISION, 
			COLLISION_BACK_DIS, true, false, false);
	}
}

void CharactorBase::CollisionGravity(void)
{
	// 線分コライダ
	int lineType = static_cast<int>(COLLIDER_TYPE::LINE);

	// 線分コライダが無ければ処理を抜ける
	if (ownColliders_.count(lineType) == 0) return;

	// 線分コライダ情報
	ColliderLine* colliderLine_ =
		dynamic_cast<ColliderLine*>(ownColliders_.at(lineType));

	if (colliderLine_ == nullptr) return;

	// 線分の始点と終点を取得
	/*VECTOR s = colliderLine_->GetPosStart();
	VECTOR e = colliderLine_->GetPosEnd();*/

	// 登録されている衝突物を全てチェック
	for (const auto& hitCol : hitColliders_)
	{
		// ステージ以外は処理を飛ばす
		if (hitCol->GetTag() != ColliderBase::TAG::STAGE) continue;

		// 派生クラスへキャスト
		const ColliderModel* colliderModel =
			dynamic_cast<const ColliderModel*>(hitCol);

		if (colliderModel == nullptr) continue;

		bool isHit = colliderLine_->PushBackUp(colliderModel, transform_, true, false);

		if (isHit)
		{
			isJump_ = false;
		}
	}
	if (!isJump_)
	{
		// ジャンプリセット
		jumpPow_ = AsoUtility::VECTOR_ZERO;

		// ジャンプの入力受付時間をリセット
		stepJump_ = 0.0f;
	}
}