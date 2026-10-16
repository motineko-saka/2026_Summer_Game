#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include <algorithm>
#include <cstdlib>
#include "../Utility/AsoUtility.h"
#include "../Manager/InputManager.h"
#include "../Object/Common/Transform.h"
#include "../Object/Collider/ColliderBase.h"
#include "../Object/Collider/ColliderModel.h"
#include "../Object/Collider/ColliderSphere.h"
#include "Camera.h"

namespace
{
	InputManager& Input(void)
	{
		return *InputManager::GetInstance();
	}

	constexpr float TWO_PI = DX_PI_F * 2.0f;
}

Camera::Camera(void) = default;

Camera::~Camera(void)
{
	Release();
}

// ---------------------------------------------------------------
// 更新・描画
// ---------------------------------------------------------------
void Camera::Update(void)
{
	// 更新前の位置を保存
	prePos_ = transform_.pos;

	switch (mode_)
	{
	case MODE::FIXED_POINT:	SetBeforeDrawFixedPoint();	break;
	case MODE::FREE:		SetBeforeDrawFree();		break;
	case MODE::FOLLOW:		SetBeforeDrawFollow();		break;
	default:												break;
	}
}

void Camera::SetBeforeDraw(void)
{
	// クリップ距離を設定する(SetDrawScreenでリセットされる)
	SetCameraNearFar(VIEW_NEAR, VIEW_FAR);

	// カメラの設定(位置と注視点による制御)
	SetCameraPositionAndTargetAndUpVec(
		transform_.pos,
		targetPos_,
		transform_.quaRot.GetUp());

	// DXライブラリのカメラとEffekseerのカメラを同期する
	Effekseer_Sync3DSetting();
}

void Camera::DrawDebug(void)
{
}

void Camera::Release(void)
{
}

// ---------------------------------------------------------------
// 初期化
// ---------------------------------------------------------------
void Camera::InitLoad(void)
{
}

void Camera::InitTransform(void)
{
	transform_.pos = AsoUtility::VECTOR_ZERO;
	transform_.quaRot = Quaternion::Identity();
}

void Camera::InitCollider(void)
{
	// 主に地面との衝突で使用する球のコライダ
	ColliderSphere* colliderSphere = new ColliderSphere(
		ColliderBase::TAG::CAMERA,
		&transform_,
		AsoUtility::VECTOR_ZERO,
		COL_CAPSULE_SPHERE);

	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::SPHERE), colliderSphere);
}

void Camera::InitAnimation(void)
{
}

void Camera::InitPost(void)
{
	Input().Add(KEY_INPUT_LCONTROL);
	Input().Add(KEY_INPUT_RCONTROL);

	ChangeMode(MODE::FIXED_POINT);
	isCollision_ = false;
}

// ---------------------------------------------------------------
// 取得・設定
// ---------------------------------------------------------------
VECTOR Camera::GetForward(void) const
{
	return VNorm(VSub(targetPos_, transform_.pos));
}

void Camera::ChangeMode(MODE mode)
{
	SetDefault();
	mode_ = mode;
}

void Camera::SetDefault(void)
{
	transform_.pos = DEFAULT_POS;
	transform_.quaRot = Quaternion::Identity();
	angles_ = DEFAULT_ANGLES;
	targetPos_ = AsoUtility::VECTOR_ZERO;
}

void Camera::SetMouseCenter(int x, int y)
{
	mouseCenterX_ = x;
	mouseCenterY_ = y;

	// マウスを基準位置へ移動（初回に急回転しないように）
	SetMousePoint(mouseCenterX_, mouseCenterY_);
}

// ---------------------------------------------------------------
// モード別更新
// ---------------------------------------------------------------
void Camera::SetBeforeDrawFixedPoint(void)
{
}

void Camera::SetBeforeDrawFree(void)
{
	ProcessRot(false);
	ProcessMove();

	UpdateRotation();

	// 注視点はカメラの前方
	targetPos_ = VAdd(transform_.pos, transform_.quaRot.PosAxis(FOLLOW_TARGET_LOCAL_POS));
}

void Camera::SetBeforeDrawFollow(void)
{
	ProcessRot(true);

	// 追従対象との相対位置を同期
	SyncFollow();

	// 衝突判定
	Collision();

	if (isCollision_)
	{
		// カメラ位置の補間
		transform_.pos = AsoUtility::Lerp(prePos_, transform_.pos, LERP_RATE_MOVE);
	}
}

void Camera::SyncFollow(void)
{
	if (followTransform_ == nullptr) return;

	// 注視点は追従対象の少し上
	VECTOR lookAt = followTransform_->pos;
	lookAt.y += FOLLOW_LOOK_AT_HEIGHT;

	// ヨー角を 0〜2π に収める
	if (angles_.y >= TWO_PI) angles_.y -= TWO_PI;
	if (angles_.y < 0.0f) angles_.y += TWO_PI;

	// ピッチ角を制限
	angles_.x = std::clamp(angles_.x, FOLLOW_PITCH_MIN_RAD, FOLLOW_PITCH_MAX_RAD);

	// ローカルオフセットを回転させてワールド座標にする
	const MATRIX mat = MMult(MGetRotX(angles_.x), MGetRotY(angles_.y));
	transform_.pos = VAdd(lookAt, VTransform(FOLLOW_CAMERA_LOCAL_POS, mat));

	targetPos_ = lookAt;

	UpdateRotation();
}

void Camera::UpdateRotation(void)
{
	// Y軸 → X軸の順で合成
	rotY_ = Quaternion::AngleAxis(angles_.y, AsoUtility::AXIS_Y);
	transform_.quaRot = rotY_.Mult(Quaternion::AngleAxis(angles_.x, AsoUtility::AXIS_X));
}

// ---------------------------------------------------------------
// 入力
// ---------------------------------------------------------------
void Camera::ProcessRot(bool isLimit)
{
	if (!controlEnabled_) return;

	if (GetJoypadNum() > 0)
	{
		RotGamePad(isLimit);
		return;
	}

	RotKeyboard(isLimit);
	RotMouse();
}

void Camera::ProcessMove(void)
{
	if (!controlEnabled_) return;

	const VECTOR moveDir = (GetJoypadNum() == 0)
		? GetMoveDirKeyboardMouse()
		: GetMoveDirGamePad();

	if (AsoUtility::EqualsVZero(moveDir)) return;

	// カメラの向きを基準にした移動方向
	const VECTOR direction = VNorm(transform_.quaRot.PosAxis(moveDir));
	const VECTOR movePow = VScale(direction, SPEED);

	transform_.pos = VAdd(transform_.pos, movePow);
	targetPos_ = VAdd(targetPos_, movePow);
}

void Camera::RotKeyboard(bool isLimit)
{
	auto& input = Input();

	// 左右回転
	if (input.IsNew(KEY_INPUT_RIGHT)) angles_.y += ROT_POW_RAD;
	if (input.IsNew(KEY_INPUT_LEFT))  angles_.y -= ROT_POW_RAD;

	// 上回転
	if (input.IsNew(KEY_INPUT_UP))
	{
		angles_.x += ROT_POW_RAD;
		if (isLimit) angles_.x = (std::min)(angles_.x, LIMIT_X_UP_RAD);
	}

	// 下回転
	if (input.IsNew(KEY_INPUT_DOWN))
	{
		angles_.x -= ROT_POW_RAD;
		if (isLimit) angles_.x = (std::max)(angles_.x, -LIMIT_X_DW_RAD);
	}
}

void Camera::RotMouse(void)
{
	// 基準位置からのずれで回転させる
	int mx = 0;
	int my = 0;
	GetMousePoint(&mx, &my);

	angles_.y += (mx - mouseCenterX_) * MOUSE_ROT_SENS;
	angles_.x -= (my - mouseCenterY_) * MOUSE_ROT_SENS;

	// 基準位置へ戻す
	SetMousePoint(mouseCenterX_, mouseCenterY_);
}

void Camera::RotGamePad(bool isLimit)
{
	const auto padState = Input().GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

	// 右スティックの傾き
	const VECTOR dir = Input().GetDirectionXZAKey(padState.AKeyRX, padState.AKeyRY);

	angles_.y += dir.x * ROT_POW_RAD;
	angles_.x += dir.z * ROT_POW_RAD;

	if (isLimit)
	{
		angles_.x = std::clamp(angles_.x, -LIMIT_X_DW_RAD, LIMIT_X_UP_RAD);
	}
}

VECTOR Camera::GetMoveDirKeyboardMouse(void) const
{
	// 基準位置からのマウスのずれ
	int mx = 0;
	int my = 0;
	GetMousePoint(&mx, &my);
	const int dx = mx - mouseCenterX_;
	const int dy = my - mouseCenterY_;

	// マウスが動いたらその方向へ平行移動
	// （右 → カメラ右、上 → カメラ前）
	if (std::abs(dx) > PIXEL_THRESHOLD || std::abs(dy) > PIXEL_THRESHOLD)
	{
		return VGet(
			static_cast<float>(dx) * MOUSE_MOVE_SENS,
			0.0f,
			-static_cast<float>(dy) * MOUSE_MOVE_SENS);
	}

	// キーボード（同時押しで斜め移動）
	auto& input = Input();
	VECTOR dir = AsoUtility::VECTOR_ZERO;
	if (input.IsNew(KEY_INPUT_W)) dir = VAdd(dir, AsoUtility::DIR_F);
	if (input.IsNew(KEY_INPUT_S)) dir = VAdd(dir, AsoUtility::DIR_B);
	if (input.IsNew(KEY_INPUT_A)) dir = VAdd(dir, AsoUtility::DIR_L);
	if (input.IsNew(KEY_INPUT_D)) dir = VAdd(dir, AsoUtility::DIR_R);
	return dir;
}

VECTOR Camera::GetMoveDirGamePad(void) const
{
	// 左スティックの傾き
	const auto padState = Input().GetJPadInputState(InputManager::JOYPAD_NO::PAD1);
	return Input().GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);
}

// ---------------------------------------------------------------
// 衝突判定
// ---------------------------------------------------------------
void Camera::Collision(void)
{
	isCollision_ = false;
	if (followTransform_ == nullptr) return;

	// 追従対象のルートフレームからカメラまでの線分で判定
	const VECTOR start = MV1GetFramePosition(followTransform_->modelId, FOLLOW_ROOT_FRAME);

	// 押し戻し用の球コライダ
	const auto sphereIt = ownColliders_.find(static_cast<int>(COLLIDER_TYPE::SPHERE));
	const bool hasSphere = (sphereIt != ownColliders_.end() && sphereIt->second != nullptr);

	for (const auto& hitCol : hitColliders_)
	{
		// モデル以外は対象外
		if (hitCol->GetShape() != ColliderBase::SHAPE::MODEL) continue;

		const auto* colliderModel = dynamic_cast<const ColliderModel*>(hitCol);
		if (colliderModel == nullptr) continue;

		const auto hitPoly =
			colliderModel->GetNearestHitPolyLine(start, transform_.pos, false, true);

		if (!hitPoly.HitFlag)
		{
			// ※元の挙動を維持：当たっていない時に補間を有効にしている
			isCollision_ = true;
			continue;
		}

		// 衝突点から少し注視点側へカメラを置く
		const VECTOR dirToTarget = VNorm(VSub(targetPos_, transform_.pos));
		transform_.pos = VAdd(hitPoly.HitPosition, VScale(dirToTarget, COLLISION_BACK_DIS));

		if (!hasSphere) continue;

		// 法線方向に押し戻す
		transform_.pos = sphereIt->second->GetPosPushBackAlongNormal(
			hitPoly, CNT_TRY_COLLISION_CAMERA, COLLISION_BACK_DIS);
	}
}