#include "../../../Utility/AsoUtility.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/Resource.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/InputManager.h"
#include "../../../Manager/Camera.h"
#include "../../../Application.h"
#include "../../Collider/ColliderLine.h"
#include "../../Collider/ColliderCapsule.h"
#include "Player.h"

Player::Player(void)
	:
	CharactorBase(),
	playerNo_(PLAYER_NO::PLAYER1)
{
}

Player::Player(PLAYER_NO playerNo, Camera& camera)
	:
	CharactorBase(),
	playerNo_(playerNo)
{
	camera_ = &camera;
}

Player::~Player(void)
{
}

void Player::Draw(void)
{
	CharactorBase::Draw();
	DrawDebug();
}

void Player::Release(void)
{
	CharactorBase::Release();
}

Player::PLAYER_NO Player::GetPlayerNo(void) const
{
	return playerNo_;
}

void Player::InitLoad(void)
{
	// 親クラスのリソースロード
	CharactorBase::InitLoad();

	transform_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::PLAYER));
}

void Player::InitTransform(void)
{
	transform_.scl = PLAYER_DEFAULT_SCALE;

	transform_.quaRot = Quaternion::Identity();

	transform_.quaRotLocal = Quaternion::Identity();
	transform_.quaRotLocal = Quaternion::Euler(PLAYER_DEFAULT_ROT_LOCAL);

	transform_.pos = PLAYER_DEFAULT_POS;
	transform_.Update();
}

void Player::InitCollider(void)
{
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

void Player::InitAnimation(void)
{
	animController_ = new AnimationController(transform_.modelId);

	animController_->Add(static_cast<int>(ANIM_TYPE::IDLE), 30.0f,
		Application::PATH_MODEL + "Player/Idle.mv1");

	animController_->Add(static_cast<int>(ANIM_TYPE::RUN), 30.0f,
		Application::PATH_MODEL + "Player/Run.mv1");

	animController_->Add(static_cast<int>(ANIM_TYPE::FAST_RUN), 30.0f,
		Application::PATH_MODEL + "Player/FastRun.mv1");

	animController_->Add(static_cast<int>(ANIM_TYPE::JUMP), 60.0f,
		Application::PATH_MODEL + "Player/JumpRising.mv1");


	animController_->Play(static_cast<int>(ANIM_TYPE::IDLE));
}

void Player::InitPost(void)
{
	isAnim_ = false;
}

void Player::UpdateProcess(void)
{
	// 移動処理
	ProcessMove();

	// ジャンプ処理
	ProcessJump();

	if (InputManager::GetInstance().IsTrgMouseRight())
	{
		playerNo_ = playerNo_ == PLAYER_NO::PLAYER1 ? PLAYER_NO::PLAYER2 : PLAYER_NO::PLAYER1;
	}
}

void Player::UpdateProcessPost(void)
{
}

void Player::ProcessMove(void)
{
	auto& ins = InputManager::GetInstance();

	VECTOR dir = AsoUtility::VECTOR_ZERO;

	bool isDash = false;

	moveSpeed_ = 0.0f;

	movePow_ = AsoUtility::VECTOR_ZERO;

	// プレイヤー番号に応じて入力キーを変更
	if (playerNo_ == PLAYER_NO::PLAYER1)
	{
		// ゲームパッドが接続されている数で処理を分ける
		if (GetJoypadNum() == 0)
		{
			// WASD で移動処理
			if (ins.IsNew(KEY_INPUT_W)) { dir = AsoUtility::DIR_F; }
			if (ins.IsNew(KEY_INPUT_A)) { dir = AsoUtility::DIR_L; }
			if (ins.IsNew(KEY_INPUT_S)) { dir = AsoUtility::DIR_B; }
			if (ins.IsNew(KEY_INPUT_D)) { dir = AsoUtility::DIR_R; }

			if (ins.IsNew(KEY_INPUT_W) && ins.IsNew(KEY_INPUT_A)) { dir = AsoUtility::DIR_FL; }
			if (ins.IsNew(KEY_INPUT_W) && ins.IsNew(KEY_INPUT_D)) { dir = AsoUtility::DIR_FR; }
			if (ins.IsNew(KEY_INPUT_S) && ins.IsNew(KEY_INPUT_A)) { dir = AsoUtility::DIR_BL; }
			if (ins.IsNew(KEY_INPUT_S) && ins.IsNew(KEY_INPUT_D)) { dir = AsoUtility::DIR_BR; }

			if (ins.IsNew(KEY_INPUT_RSHIFT)) { isDash = true; }
		}
		else
		{
			// 接続しているゲームパッド1の情報取得
			InputManager::JOYPAD_IN_STATE padState =
				ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

			// アナログキーの入力値を正規化して取得
			dir = ins.GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);

			if (ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1,
				InputManager::JOYPAD_BTN::R_TRIGGER))
			{
				isDash = true;
			}
		}
	}
	else if (playerNo_ == PLAYER_NO::PLAYER2)
	{
		// プレイヤー2は矢印キーで操作
		if (ins.IsNew(KEY_INPUT_I)) { dir = AsoUtility::DIR_F; }
		if (ins.IsNew(KEY_INPUT_J)) { dir = AsoUtility::DIR_L; }
		if (ins.IsNew(KEY_INPUT_K)) { dir = AsoUtility::DIR_B; }
		if (ins.IsNew(KEY_INPUT_L)) { dir = AsoUtility::DIR_R; }

		if (ins.IsNew(KEY_INPUT_I) && ins.IsNew(KEY_INPUT_J)) { dir = AsoUtility::DIR_FL; }
		if (ins.IsNew(KEY_INPUT_I) && ins.IsNew(KEY_INPUT_L)) { dir = AsoUtility::DIR_FR; }
		if (ins.IsNew(KEY_INPUT_K) && ins.IsNew(KEY_INPUT_J)) { dir = AsoUtility::DIR_BL; }
		if (ins.IsNew(KEY_INPUT_K) && ins.IsNew(KEY_INPUT_L)) { dir = AsoUtility::DIR_BR; }

		if (ins.IsNew(KEY_INPUT_RCONTROL)) { isDash = true; }
	}

	if (!AsoUtility::EqualsVZero(dir))
	{
		// 移動スピード
		moveSpeed_ = SPEED_MOVE;

		if (isDash)
		{
			moveSpeed_ = SPEED_DASH;
		}

		// ジャンプ中はアニメーション変えない
		if (!isJump_)
		{
			// アニメーション
			if (isDash)
			{
				animController_->Play(
					static_cast<int>(ANIM_TYPE::FAST_RUN), true);
			}
			else
			{
				animController_->Play(
					static_cast<int>(ANIM_TYPE::RUN), true);
			}
		}

		// Y軸のみのカメラ角度を取得

		Quaternion cameraRot = camera_->GetQuaRotY();

		// 移動方向をカメラに合わせる
		moveDir_ = Quaternion::PosAxis(cameraRot, dir);

		// 移動量を計算
		movePow_ = VScale(moveDir_, moveSpeed_);
	}
	else
	{
		// ジャンプ中はアニメーション変えない
		if (!isJump_)
		{
			// IDLE状態に戻す
			animController_->Play(
				static_cast<int>(ANIM_TYPE::IDLE), true);

		}
	}
}

void Player::ProcessJump(void)
{
	auto& ins = InputManager::GetInstance();

	// プレイヤー番号に応じてジャンプキーを変更
	bool isHitKeyNew = false;
	bool isHitKey = false;

	if (playerNo_ == PLAYER_NO::PLAYER1)
	{
		isHitKeyNew = ins.IsNew(KEY_INPUT_BACKSLASH)
			|| ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN);
		isHitKey = ins.IsTrgDown(KEY_INPUT_BACKSLASH)
			|| ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN);
	}
	else if (playerNo_ == PLAYER_NO::PLAYER2)
	{
		isHitKeyNew = ins.IsNew(KEY_INPUT_RETURN); // Enterキー
		isHitKey = ins.IsTrgDown(KEY_INPUT_RETURN);
	}

	// 溜めジャンプ処理
	if (isHitKeyNew)
	{
		// ジャンプの入力受付時間加算
		stepJump_ += scnMng_.GetDeltaTime();
		if (stepJump_ < TIME_JUMP_INPUT)
		{
			// ジャンプ力の計算
			float jumpSpeed = POW_JUMP_KEEP * scnMng_.GetDeltaTime();
			jumpPow_ = VAdd(jumpPow_, VScale(AsoUtility::DIR_U, jumpSpeed));
		}
	}
	else
	{
		// ボタンを離したらジャンプ力に加算しない
		return;
	}

	// ジャンプ
	if (isHitKey && !isJump_)
	{
		// ジャンプ力の計算
		float jumpSpeed = POW_JUMP_INIT * scnMng_.GetDeltaTime();
		jumpPow_ = VScale(AsoUtility::DIR_U, jumpSpeed);
		isJump_ = true;

		// アニメーション再生
		animController_->Play(
			static_cast<int>(ANIM_TYPE::JUMP), false);
	}
}

void Player::ProcessAnimPos(void)
{
	// アニメーション毎との線分座標
	if (animController_->GetPlayType() == static_cast<int>(ANIM_TYPE::JUMP))
	{
		// ジャンプ中は線分座標を伸ばす
		if (ownColliders_.count(static_cast<int>(COLLIDER_TYPE::LINE)) != 0)
		{
			ColliderLine* colLine = dynamic_cast<ColliderLine*>(
				ownColliders_.at(static_cast<int>(COLLIDER_TYPE::LINE)));
			colLine->SetLocalPosStart(COL_LINE_JUMP_START_LOCAL_POS);
			colLine->SetLocalPosEnd(COL_LINE_JUMP_END_LOCAL_POS);
		}
	}
	else
	{
		// 通常時の線分に戻す
		if (ownColliders_.count(static_cast<int>(COLLIDER_TYPE::LINE)) != 0)
		{
			ColliderLine* colLine = dynamic_cast<ColliderLine*>(
				ownColliders_.at(static_cast<int>(COLLIDER_TYPE::LINE)));
			colLine->SetLocalPosStart(COL_LINE_START_LOCAL_POS);
			colLine->SetLocalPosEnd(COL_LINE_END_LOCAL_POS);
		}
	}
}

void Player::ProcessAnimCapsule(void)
{
	// アニメーション毎との線分座標
	if (animController_->GetPlayType() == static_cast<int>(ANIM_TYPE::JUMP))
	{
		// ジャンプ中は線分座標を伸ばす
		if (ownColliders_.count(static_cast<int>(COLLIDER_TYPE::CAPSULE)) != 0)
		{
			ColliderCapsule* colCapsule = dynamic_cast<ColliderCapsule*>(
				ownColliders_.at(static_cast<int>(COLLIDER_TYPE::CAPSULE)));
			colCapsule->SetLocalPosTop(COL_CAPSULE_TOP_JUMP_LOCAL_POS);
			colCapsule->SetLocalPosDown(COL_CAPSULE_DOWN_JUMP_LOCAL_POS);
		}
	}
	else
	{
		// 通常時の線分に戻す
		if (ownColliders_.count(static_cast<int>(COLLIDER_TYPE::CAPSULE)) != 0)
		{
			ColliderCapsule* colCapsule = dynamic_cast<ColliderCapsule*>(
				ownColliders_.at(static_cast<int>(COLLIDER_TYPE::CAPSULE)));
			colCapsule->SetLocalPosTop(COL_CAPSULE_TOP_LOCAL_POS);
			colCapsule->SetLocalPosDown(COL_CAPSULE_DOWN_LOCAL_POS);
		}
	}
}

void Player::CollisionReserve(void)

{
	// アニメーション毎との位置座標
	ProcessAnimPos();

	ProcessAnimCapsule();
}

void Player::DrawDebug(void)
{
	int offsetX = (playerNo_ == PLAYER_NO::PLAYER2) ? 400 : 15;
	DrawFormatString(offsetX, 20, 0x000000, "%f,%f,%f", transform_.pos.x, transform_.pos.y, transform_.pos.z);
}