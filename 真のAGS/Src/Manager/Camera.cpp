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
	Release();
}

void Camera::Update(void)
{
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

void Camera::SetTargetPos(const VECTOR& pos)
{
	// 明示的に注視点を設定
	targetPos_ = pos;
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
	InputManager::GetInstance()->Add(KEY_INPUT_LCONTROL);
	InputManager::GetInstance()->Add(KEY_INPUT_RCONTROL);
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
	// フォロー対象
	if (followTransform_ == nullptr)
		return;

	// 注視点プレイヤー位置
	VECTOR lookAt = followTransform_->pos;
	lookAt.y += 100.0f;

	// ヨー角のラップ
	const float TWO_PI = AsoUtility::Deg2RadF(360.0f);
	if (angles_.y >= TWO_PI) angles_.y -= TWO_PI;
	if (angles_.y < 0.0f) angles_.y += TWO_PI;

	const float PITCH_MIN = AsoUtility::Deg2RadF(-30.0f);
	const float PITCH_MAX = AsoUtility::Deg2RadF(60.0f);
	if (angles_.x < PITCH_MIN) angles_.x = PITCH_MIN;
	if (angles_.x > PITCH_MAX) angles_.x = PITCH_MAX;

	// 回転行列
	MATRIX mat = MGetIdent();
	mat = MMult(mat, MGetRotX(angles_.x));
	mat = MMult(mat, MGetRotY(angles_.y));

	// ローカルオフセットを回転行列で変換してワールド座標にする
	VECTOR localCam = VTransform(FOLLOW_CAMERA_LOCAL_POS, mat);
	transform_.pos = VAdd(lookAt, localCam);

	// 注視点はプレイヤー
	targetPos_ = lookAt;

	// カメラの回転
	rotY_ = Quaternion::AngleAxis(angles_.y, AsoUtility::AXIS_Y);
	transform_.quaRot = rotY_.Mult(Quaternion::AngleAxis(angles_.x, AsoUtility::AXIS_X));
	transform_.quaRot.GetUp();
}

void Camera::ProcessRot(bool isLimit)
{
	if (!controlEnabled_) return;

	VECTOR moveDir = AsoUtility::VECTOR_ZERO;

	if (GetJoypadNum() == 0)
	{
		// キーボード回転
		RotKeyboard(isLimit);

		// --- マウスの相対移動で回転させる
		int mx, my;
		GetMousePoint(&mx, &my);

		int dx = mx - mouseCenterX_;
		int dy = my - mouseCenterY_;

		dx = mx - mouseCenterX_;
		dy = my - mouseCenterY_;

		angles_.y += dx * MOUSE_ROT_SENS;
		angles_.x -= dy * MOUSE_ROT_SENS;

		// 中央へ戻す
		SetMousePoint(mouseCenterX_, mouseCenterY_);
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

	VECTOR moveDir = AsoUtility::VECTOR_ZERO;

	if (GetJoypadNum() == 0)
	{
		// マウスのフレーム間移動量を取得
		int dx, dy;
		GetMousePoint(&dx, &dy);

		// 感度
		const float MOUSE_MOVE_SENS = 0.5f;
		// ノイズ除去の閾値
		const int PIXEL_THRESHOLD = 1;

		// マウスが動いたらその方向でカメラを平行移動させたい
		if (dx > PIXEL_THRESHOLD || dx < -PIXEL_THRESHOLD || dy > PIXEL_THRESHOLD || dy < -PIXEL_THRESHOLD)
		{
			// ローカル軸での移動ベクトルを設定
			// マウス右移動 -> カメラ右移動、マウス上移動 -> カメラ前方移動になるよう符号調整
			moveDir.x = static_cast<float>(dx) * MOUSE_MOVE_SENS;
			moveDir.z = -static_cast<float>(dy) * MOUSE_MOVE_SENS;
		}
		else
		{
			// キーボードフォールバック（必要なければ削除可）
			if (InputManager::GetInstance()->IsNew(KEY_INPUT_W)) { moveDir = AsoUtility::DIR_F; }
			if (InputManager::GetInstance()->IsNew(KEY_INPUT_S)) { moveDir = AsoUtility::DIR_B; }
			if (InputManager::GetInstance()->IsNew(KEY_INPUT_A)) { moveDir = AsoUtility::DIR_L; }
			if (InputManager::GetInstance()->IsNew(KEY_INPUT_D)) { moveDir = AsoUtility::DIR_R; }
		}
	}
	else
	{
		InputManager::JOYPAD_IN_STATE padState =
			InputManager::GetInstance()->GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

		// 左スティックの傾き
		moveDir = InputManager::GetInstance()->GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);
	}

	// 移動処理
	if (!AsoUtility::EqualsVZero(moveDir))
	{
		// 現在の向きからの進行方向を取得
		VECTOR direction = VNorm(transform_.quaRot.PosAxis(moveDir));

		// 移動量を計算して適用
		VECTOR movePow = VScale(direction, SPEED);
		transform_.pos = VAdd(transform_.pos, movePow);
		targetPos_ = VAdd(targetPos_, movePow);
	}
}

void Camera::SetBeforeDrawFixedPoint(void)
{

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

	// カメラ回転
	if (InputManager::GetInstance()->IsNew(KEY_INPUT_RIGHT))
	{
		// 右回転
		angles_.y += ROT_POW_RAD;
	}
	if (InputManager::GetInstance()->IsNew(KEY_INPUT_LEFT))
	{
		// 左回転
		angles_.y -= ROT_POW_RAD;
	}

	// 上回転
	if (InputManager::GetInstance()->IsNew(KEY_INPUT_UP))
	{
		angles_.x += ROT_POW_RAD;
		if (isLimit && angles_.x > LIMIT_X_UP_RAD)
		{
			angles_.x = LIMIT_X_UP_RAD;
		}
	}

	// 下回転
	if (InputManager::GetInstance()->IsNew(KEY_INPUT_DOWN))
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

	// 接続されているゲームパッド１の情報を取得
	InputManager::JOYPAD_IN_STATE padState =
		InputManager::GetInstance()->GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

	// 右スティックの傾き
	VECTOR dir = InputManager::GetInstance()->GetDirectionXZAKey(padState.AKeyRX, padState.AKeyRY);

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

void Camera::SetMouseCenter(int x, int y)
{
	mouseCenterX_ = x;
	mouseCenterY_ = y;

	// マウスをその位置へ移動
	SetMousePoint(mouseCenterX_, mouseCenterY_);

	// 差分が出ないよう同期
	prevMouseX_ = mouseCenterX_;
	prevMouseY_ = mouseCenterY_;
}