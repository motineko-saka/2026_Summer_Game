#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "../Utility/AsoUtility.h"
#include "../Manager/InputManager.h"
#include "../Object/Common/Transform.h"
#include "../Object/Collider/ColliderModel.h"
#include "../Object/Collider/ColliderSphere.h"
#include "../Object/Collider/ColliderBase.h"
#include "Camera.h"

Camera::Camera(void)
	:
	followTransform_(nullptr),
	mode_(MODE::NONE),
	//pos_(AsoUtility::VECTOR_ZERO),
	angles_(AsoUtility::VECTOR_ZERO),
	//rot_(Quaternion::Identity()),
	rotY_(Quaternion::Identity()),
	targetPos_(AsoUtility::VECTOR_ZERO),
	controlEnabled_(true)
	//cameraUp_(AsoUtility::DIR_U)
{
	// DxLibの初期設定では、
	// カメラの位置が x = 320.0f, y = 240.0f, z = (画面のサイズによって変化)、 
	// 注視点の位置は x = 320.0f, y = 240.0f, z = 1.0f
	// カメラの上方向は x = 0.0f, y = 1.0f, z = 0.0f
	// 右上位置からZ軸のプラス方向を見るようなカメラ
}

Camera::~Camera(void)
{
}

void Camera::Update(void)
{
	// Ctrlキーで TOP モードと前のモードを切り替える
	if (!controlEnabled_) return;

	// Ctrlキーで TOP モードと前のモードを切り替える
	auto& ins = InputManager::GetInstance();

	// 押下トリガーで切替
	if (ins.IsTrgDown(KEY_INPUT_LCONTROL) || ins.IsTrgDown(KEY_INPUT_RCONTROL))
	{
		if (mode_ != MODE::TOP)
		{
			// 現在のモードを保存してTOP
			prevMode_ = mode_;
			ChangeMode(MODE::TOP);
		}
		else
		{
			// TOP から戻す（保存していたモードへ）
			ChangeMode(prevMode_);
		}
	}
}

void Camera::SetBeforeDraw(void)
{
	// クリップ距離を設定する(SetDrawScreenでリセットされる)
	SetCameraNearFar(VIEW_NEAR, VIEW_FAR);

	// 更新前情報
	prePos_ = transform_.pos;

	switch (mode_)
	{
	case Camera::MODE::FIXED_POINT:
		SetBeforeDrawFixedPoint();
		break;
	case Camera::MODE::FREE:
		SetBeforeDrawFree();
		break;
	case Camera::MODE::FOLLOW:
		SetBeforeDrawFollow();
		break;
	case Camera::MODE::TOP:
		SetBeforeDrawTop();
		break;
	}

	// カメラの設定(位置と注視点による制御)
	SetCameraPositionAndTargetAndUpVec(
		transform_.pos,
		targetPos_,
		transform_.quaRot.GetUp()
	);

	// DXライブラリのカメラとEffekseerのカメラを同期する。
	Effekseer_Sync3DSetting();

}

void Camera::DrawDebug(void)
{
}

void Camera::Release(void)
{
}

void Camera::SetFollow(const Transform* follow)
{
	followTransform_ = follow;
}

void Camera::InitLoad(void)
{
}

void Camera::InitTransform(void)
{
	// カメラの位置
	transform_.pos = AsoUtility::VECTOR_ZERO;

	// カメラの角度
	transform_.quaRot = Quaternion::Identity();

	// カメラの上方向
	transform_.quaRot.GetUp();
}

void Camera::InitCollider(void)
{
	// 主に地面との衝突で使用する球体コライダ
	ColliderSphere* colliderSphere = new ColliderSphere(
		ColliderBase::TAG::CAMERA,
		&transform_,
		AsoUtility::VECTOR_ZERO,
		COL_CAPSULE_SPHERE
	);
	ownColliders_.emplace(
		static_cast<int>(COLLIDER_TYPE::SPHERE), colliderSphere);
}

void Camera::InitAnimation(void)
{
}

void Camera::InitPost(void)
{
	InputManager::GetInstance().Add(KEY_INPUT_LCONTROL);
	InputManager::GetInstance().Add(KEY_INPUT_RCONTROL);
	ChangeMode(MODE::FIXED_POINT);

	isCollision_ = false;
}

const VECTOR& Camera::GetPos(void) const
{
	return transform_.pos;
}

const VECTOR& Camera::GetAngles(void) const
{
	return angles_;
}

const VECTOR& Camera::GetTargetPos(void) const
{
	return targetPos_;
}

const Quaternion& Camera::GetQuaRot(void) const
{
	return transform_.quaRot;
}

const Quaternion& Camera::GetQuaRotY(void) const
{
	return rotY_;
}

VECTOR Camera::GetForward(void) const
{
	return VNorm(VSub(targetPos_, transform_.pos));
}

void Camera::ChangeMode(MODE mode)
{

	// カメラの初期設定
	SetDefault();

	// カメラモードの変更
	mode_ = mode;

	// 変更時の初期化処理
	switch (mode_)
	{
	case Camera::MODE::FIXED_POINT:
		break;
	case Camera::MODE::FREE:
		break;
	case Camera::MODE::FOLLOW:
		break;
	case Camera::MODE::TOP:
		// TOPモードに入ったときに一度だけ角度を決める
		angles_.x = -DX_PI_F * 0.5f; // 真上から見下ろす
		angles_.y = 0.0f;

		// 初期位置はここで一度だけ設定する（毎フレーム上書きしない）
		if (followTransform_ != nullptr)
		{
			transform_.pos = VAdd(followTransform_->pos, TOP_CAMERA_LOCAL_POS);
			targetPos_ = VAdd(followTransform_->pos, TOP_TARGET_LOCAL_POS);
		}
		else
		{
			transform_.pos = TOP_CAMERA_LOCAL_POS;
			targetPos_ = TOP_TARGET_LOCAL_POS;
		}
		break;
	}
}

void Camera::SetDefault(void)
{
	// カメラの初期設定
	transform_.pos = DERFAULT_POS;

	// カメラ角
	angles_ = DERFAULT_ANGLES;
	transform_.quaRot = Quaternion::Identity();

	// 注視点
	targetPos_ = AsoUtility::VECTOR_ZERO;

	// カメラの上方向
	transform_.quaRot.GetUp();
}

void Camera::SyncFollow(void)
{

	// 同期先の位置
	VECTOR pos = followTransform_->pos;

	// Y軸
	rotY_ = Quaternion::AngleAxis(angles_.y, AsoUtility::AXIS_Y);

	// Y軸 + X軸
	transform_.quaRot = rotY_.Mult(Quaternion::AngleAxis(angles_.x, AsoUtility::AXIS_X));

	VECTOR localPos;

	// 注視点
	localPos = transform_.quaRot.PosAxis(FOLLOW_TARGET_LOCAL_POS);
	targetPos_ = VAdd(pos, localPos);

	// カメラ位置
	localPos = transform_.quaRot.PosAxis(FOLLOW_CAMERA_LOCAL_POS);
	transform_.pos = VAdd(pos, localPos);

	// カメラの上方向
	transform_.quaRot.GetUp();

}

void Camera::ProcessRot(bool isLimit)
{
	if (!controlEnabled_) return;

	if (GetJoypadNum() == 0)
	{
		// 方向回転によるXYZの移動(キーボード)
		RotKeyboard(isLimit);
	}
	else
	{
		// 方向回転によるXYZの移動(ゲームパッド)
		RotGamePad(isLimit);
	}
}

void Camera::ProcessMove(void)
{
	if (!controlEnabled_) return;

	auto& ins = InputManager::GetInstance();

	VECTOR moveDir = AsoUtility::VECTOR_ZERO;

	if (GetJoypadNum() == 0)
	{
		if (ins.IsNew(KEY_INPUT_W)) { moveDir = AsoUtility::DIR_F; }
		if (ins.IsNew(KEY_INPUT_S)) { moveDir = AsoUtility::DIR_B; }
		if (ins.IsNew(KEY_INPUT_A)) { moveDir = AsoUtility::DIR_L; }
		if (ins.IsNew(KEY_INPUT_D)) { moveDir = AsoUtility::DIR_R; }
	}
	else
	{
		InputManager::JOYPAD_IN_STATE padState =
			ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

		// 左スティックの傾き
		moveDir = ins.GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);
	}

	// 移動処理
	if (!AsoUtility::EqualsVZero(moveDir))
	{
		// 移動させたい方向(ベクトル)に変換
		// 現在の向きからの進行方向を取得
		VECTOR direction = VNorm(transform_.quaRot.PosAxis(moveDir));

		// 移動させたい方向に移動量をかける(=移動量)
		VECTOR movePow = VScale(direction, SPEED);

		// カメラ位置も注視点も移動させる
		transform_.pos = VAdd(transform_.pos, movePow);
		targetPos_ = VAdd(targetPos_, movePow);
	}

}

void Camera::SetBeforeDrawFixedPoint(void)
{
	// 何もしない
}

void Camera::SetBeforeDrawFree(void)
{

	// カメラ操作(回転/移動)は制御が有効な場合のみ
	if (controlEnabled_)
	{
		ProcessRot(false);
		ProcessMove();
	}

	// Y軸
	rotY_ = Quaternion::AngleAxis(angles_.y, AsoUtility::AXIS_Y);

	// Y軸 + X軸
	transform_.quaRot = rotY_.Mult(Quaternion::AngleAxis(angles_.x, AsoUtility::AXIS_X));

	// 注視点更新
	targetPos_ = VAdd(transform_.pos, transform_.quaRot.PosAxis(FOLLOW_TARGET_LOCAL_POS));

	// カメラの上方向更新
	transform_.quaRot.GetUp();

}

void Camera::SetBeforeDrawFollow(void)
{

	// カメラ操作(回転)は制御が有効な場合のみ
	if (controlEnabled_)
	{
		ProcessRot(true);
	}

	// 追従対象との相対位置を同期
	SyncFollow();

	// 衝突判定
	Collision();

	if (isCollision_)
	{
		// カメラ位置の補間
		transform_.pos =
			AsoUtility::Lerp(prePos_, transform_.pos, LERP_RATE_MOVE);
	}
}

void Camera::SetBeforeDrawTop(void)
{
	if (!controlEnabled_) return;

	// NOTE:
	// ここでは毎フレーム transform_.pos を固定値で上書きしない。
	// ChangeMode で初期位置を設定し、以降は入力で移動できるようにする。

	// 回転を計算
	rotY_ = Quaternion::AngleAxis(angles_.y, AsoUtility::AXIS_Y);
	transform_.quaRot = rotY_.Mult(Quaternion::AngleAxis(angles_.x, AsoUtility::AXIS_X));

	auto& ins = InputManager::GetInstance();

	// 押し続けで移動させる（連続移動）
	VECTOR movePow = AsoUtility::VECTOR_ZERO;

	if (ins.IsNew(KEY_INPUT_UP))
	{
		VECTOR f = VNorm(transform_.quaRot.PosAxis(AsoUtility::DIR_F));
		movePow = VAdd(movePow, VScale(f, SPEED));
	}
	if (ins.IsNew(KEY_INPUT_DOWN))
	{
		VECTOR b = VNorm(transform_.quaRot.PosAxis(AsoUtility::DIR_B));
		movePow = VAdd(movePow, VScale(b, SPEED));
	}
	if (ins.IsNew(KEY_INPUT_LEFT))
	{
		VECTOR l = VNorm(transform_.quaRot.PosAxis(AsoUtility::DIR_L));
		movePow = VAdd(movePow, VScale(l, SPEED));
	}
	if (ins.IsNew(KEY_INPUT_RIGHT))
	{
		VECTOR r = VNorm(transform_.quaRot.PosAxis(AsoUtility::DIR_R));
		movePow = VAdd(movePow, VScale(r, SPEED));
	}

	// 移動を適用（注視点も同量移動）
	if (!AsoUtility::EqualsVZero(movePow))
	{
		transform_.pos = VAdd(transform_.pos, movePow);
		targetPos_ = VAdd(targetPos_, movePow);
	}

	transform_.quaRot.GetUp();
}
void Camera::Collision(void)
{
	// プレイヤーのルートフレーム
	VECTOR start = MV1GetFramePosition(followTransform_->modelId, 1);
	isCollision_ = false;

	for (const auto& hitCol : hitColliders_)
	{
		// モデル以外は処理を飛ばす
		if (hitCol->GetShape() != ColliderBase::SHAPE::MODEL) continue;

		// 派生クラスへキャスト
		const ColliderModel* colliderModel =
			dynamic_cast<const ColliderModel*>(hitCol);

		if (colliderModel == nullptr) continue;

		auto hitPoly = colliderModel->GetNearestHitPolyLine(start, transform_.pos, false, true);

		if (!hitPoly.HitFlag)
		{
			isCollision_ = true;

			// 衝突していなければ次のコライダへ
			continue;
		}

		// カメラ位置から注視点への方向
		VECTOR dirToTarget = VNorm(VSub(targetPos_, transform_.pos));

		// 衝突点の少し手前にカメラを置く
		transform_.pos =
			VAdd(hitPoly.HitPosition, VScale(dirToTarget, COLLISION_BACK_DIS));

		// カメラ位置の球体コライダ
		int typeSphere = static_cast<int>(COLLIDER_TYPE::SPHERE);

		// 球体コライダが無ければ処理を抜ける
		if (ownColliders_.count(typeSphere) == 0) continue;

		if (ownColliders_.at(typeSphere) == nullptr) return;

		transform_.pos = ownColliders_.at(typeSphere)->
			GetPosPushBackAlongNormal(hitPoly, CNT_TRY_COLLISION_CAMERA, COLLISION_BACK_DIS);
	}
}

void Camera::RotKeyboard(bool isLimit)
{
	const auto& ins = InputManager::GetInstance();

	// カメラ回転
	if (ins.IsNew(KEY_INPUT_RIGHT))
	{
		// 右回転
		angles_.y += ROT_POW_RAD;
	}
	if (ins.IsNew(KEY_INPUT_LEFT))
	{
		// 左回転
		angles_.y -= ROT_POW_RAD;
	}

	// 上回転
	if (ins.IsNew(KEY_INPUT_UP))
	{
		angles_.x += ROT_POW_RAD;
		if (isLimit && angles_.x > LIMIT_X_UP_RAD)
		{
			angles_.x = LIMIT_X_UP_RAD;
		}
	}

	// 下回転
	if (ins.IsNew(KEY_INPUT_DOWN))
	{
		angles_.x -= ROT_POW_RAD;
		if (isLimit && angles_.x < -LIMIT_X_DW_RAD)
		{
			angles_.x = -LIMIT_X_DW_RAD;
		}
	}
}

void Camera::RotGamePad(bool isLimit)
{

	auto& ins = InputManager::GetInstance();

	// 接続されているゲームパッド１の情報を取得
	InputManager::JOYPAD_IN_STATE padState =
		ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

	// 右スティックの傾き
	VECTOR dir = ins.GetDirectionXZAKey(padState.AKeyRX, padState.AKeyRY);

	// 右スティック左右の傾き
	angles_.y += dir.x * ROT_POW_RAD;

	// 右スティック上下の傾き
	angles_.x += dir.z * ROT_POW_RAD;

	// 角度制限
	if (isLimit && angles_.x < -LIMIT_X_DW_RAD)
	{
		angles_.x = -LIMIT_X_DW_RAD;
	}
	if (isLimit && angles_.x > LIMIT_X_UP_RAD)
	{
		angles_.x = LIMIT_X_UP_RAD;
	}

}