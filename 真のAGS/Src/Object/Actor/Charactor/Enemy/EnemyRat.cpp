#include "../../../../Utility/AsoUtility.h"
#include "../../../../Manager/ResourceManager.h"
#include "../../../../Manager/Resource.h"
#include "../../../../Manager/SceneManager.h"
#include "../../../../Manager/InputManager.h"
#include "../../../../Manager/Camera.h"
#include "../../../../Application.h"
#include "../../../Collider/ColliderLine.h"
#include "../../../Collider/ColliderCapsule.h"
#include "../Player.h"
#include "EnemyRat.h"

EnemyRat::EnemyRat(const EnemyBase::EnemyData& data,Player* player)
	:
	EnemyBase(data,player),
	state_(STATE::NONE),
	step_(0.0f)
{
}

EnemyRat::~EnemyRat(void)
{
}

void EnemyRat::InitLoad(void)
{
	// 基底クラスのリソースロード
	CharactorBase::InitLoad();

	transform_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::ENEMY_RAT));
}

void EnemyRat::InitTransform(void)
{
	transform_.scl = VScale(AsoUtility::VECTOR_ONE, SCALE);
	//transform_.pos = { 0.0f, 100.0f, 1500.0f };
	transform_.quaRot = Quaternion::Identity();
	transform_.quaRotLocal = Quaternion::Euler(ROT);

	transform_.Update();
}

void EnemyRat::InitCollider(void)
{
	// 主に地面との衝突で使用する線分コライダ
	ColliderLine* colLine = new ColliderLine(
		ColliderBase::TAG::ENEMY, &transform_,
		COL_LINE_START_LOCAL_POS, COL_LINE_END_LOCAL_POS);
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::LINE), colLine);

	// 主に壁や木などの衝突で仕様するカプセルコライダ
	ColliderCapsule* colCapsule = new ColliderCapsule(
		ColliderBase::TAG::ENEMY, &transform_,
		COL_CAPSULE_TOP_LOCAL_POS, COL_CAPSULE_DOWN_LOCAL_POS,
		COL_CAPSULE_RADIUS);
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::CAPSULE), colCapsule);
}

void EnemyRat::InitAnimation(void)
{
	animController_ = new AnimationController(transform_.modelId);

	int idleType = -1;
	int walkType = -1;

	idleType = static_cast<int>(ANIM_TYPE::IDLE);
	walkType = static_cast<int>(ANIM_TYPE::WALK);

	animController_->AddInFbx(idleType, 20.0f, idleType);
	animController_->AddInFbx(walkType, 20.0f, walkType);

	animController_->Play(idleType, true);
}

void EnemyRat::InitPost(void)
{
	// 状態遷移初期処理登録
	stateChanges_.emplace(static_cast<int>(STATE::NONE),
		std::bind(&EnemyRat::ChangeStateNone, this));
	stateChanges_.emplace(static_cast<int>(STATE::THINK),
		std::bind(&EnemyRat::ChangeStateThink, this));
	stateChanges_.emplace(static_cast<int>(STATE::IDLE),
		std::bind(&EnemyRat::ChangeStateIdle, this));
	stateChanges_.emplace(static_cast<int>(STATE::WANDER),
		std::bind(&EnemyRat::ChangeStateWander, this));
	stateChanges_.emplace(static_cast<int>(STATE::END),
		std::bind(&EnemyRat::ChangeStateEnd, this));

	ChangeState(STATE::THINK);
}

void EnemyRat::UpdateProcess(void)
{
	/*switch (state_)
	{
	case STATE::NONE:
		UpdateNone();
		break;
	case STATE::THINK:
		UpdateThink();
		break;
	case STATE::IDLE:
		UpdateIdle();
		break;
	case STATE::WANDER:
		UpdateWander();
		break;
	case STATE::END:
		UpdateEnd();
		break;
	}*/

	// 状態別更新
	stateUpdate_();

	//movePow_ = { -3.0f,0.0f,1.0f };
}

void EnemyRat::UpdateProcessPost(void)
{
	EnemyBase::UpdateProcessPost();

	if (!InMovableRange())
	{
		transform_.pos = prevPos_;

		// モデル制御更新
		transform_.Update();

		ChangeState(STATE::THINK);
	}

	
}

void EnemyRat::ChangeState(STATE state)
{
	state_ = state;

	// 各状態遷移の初期処理
	stateChanges_[static_cast<int>(state_)]();

	/*switch (state_) 
	{
	case STATE::NONE:
		ChangeStateNone();
		break;
	case STATE::THINK:
		ChangeStateThink();
		break;
	case STATE::IDLE:
		ChangeStateIdle();
		break;
	case STATE::WANDER:
		ChangeStateWander();
		break;
	case STATE::END:
		ChangeStateEnd();
		break;
	}*/
}

void EnemyRat::ChangeStateNone(void)
{
	stateUpdate_ = std::bind(&EnemyRat::UpdateNone, this);
}

void EnemyRat::ChangeStateThink(void)
{
	stateUpdate_ = std::bind(&EnemyRat::UpdateThink, this);

	// 思考
	// ランダムに次の行動を決定
	// 30%で待機、70%で徘徊
	int rand = GetRand(100);

	if (rand < 30)
	{
		ChangeState(STATE::IDLE);
	}
	else
	{
		ChangeState(STATE::WANDER);
	}
}

void EnemyRat::ChangeStateIdle(void)
{
	stateUpdate_ = std::bind(&EnemyRat::UpdateIdle, this);

	// ランダムな待機時間
	step_ = 3.0f + static_cast<float>(GetRand(3));

	// 移動量ゼロ
	movePow_ = AsoUtility::VECTOR_ZERO;

	// 待機アニメーション再生
	animController_->Play(
		static_cast<int>(ANIM_TYPE::IDLE), true);
}

void EnemyRat::ChangeStateWander(void)
{
	stateUpdate_ = std::bind(&EnemyRat::UpdateWander, this);

	// ランダムな角度
	float angle = static_cast<float>(GetRand(360)) * DX_PI_F / 180.0f;

	// 移動方向
	moveDir_ = VGet(cosf(angle), 0.0f, sinf(angle));

	// ランダムな移動時間
	step_ = 2.0f + static_cast<float>(GetRand(5));

	// 移動スピード
	moveSpeed_ = 3.0f;


	// 歩きアニメーション再生
	animController_->Play(
		static_cast<int>(ANIM_TYPE::WALK), true);
}

void EnemyRat::ChangeStateEnd(void)
{
	stateUpdate_ = std::bind(&EnemyRat::UpdateEnd, this);
}

void EnemyRat::UpdateNone(void)
{
}

void EnemyRat::UpdateThink(void)
{
}

void EnemyRat::UpdateIdle(void)
{
	if (step_ < 0.0f) {
		ChangeState(STATE::THINK);
	}

	step_--;
}

void EnemyRat::UpdateWander(void)
{
	if (step_ < 0.0f) {
		ChangeState(STATE::THINK);
	}

	movePow_ = VScale(moveDir_, moveSpeed_);

	step_--;
}

void EnemyRat::UpdateEnd(void)
{
}
