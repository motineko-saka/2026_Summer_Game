#pragma once
#include "../Common/Quaternion.h"
#include "../Object/Actor/ActorBase.h"

class Transform;

class Camera : public ActorBase
{
public:

	// カメラの初期座標
	static constexpr VECTOR DEFAULT_POS = { 0.0f, 200.0f, -500.0f };

	// カメラの初期角度
	static constexpr VECTOR DEFAULT_ANGLES = { 0.0f, 0.0f, 0.0f };

	// 旧名（綴り誤り）。他のファイルから参照されている場合に備えて残す
	static constexpr VECTOR DERFAULT_POS = DEFAULT_POS;
	static constexpr VECTOR DERFAULT_ANGLES = DEFAULT_ANGLES;

	// カメラの回転量
	static constexpr float ROT_POW_DEG = 2.0f;
	static constexpr float ROT_POW_RAD = ROT_POW_DEG * DX_PI_F / 180.0f;

	// カメラの移動スピード
	static constexpr float SPEED = 20.0f;

	// カメラのクリップ範囲
	static constexpr float VIEW_NEAR = 10.0f;
	static constexpr float VIEW_FAR = 20000.0f;

	// 追従位置からカメラ位置までの相対座標
	static constexpr VECTOR FOLLOW_CAMERA_LOCAL_POS = { 0.0f, 50.0f, -400.0f };

	// 追従位置から注視点までの相対座標
	static constexpr VECTOR FOLLOW_TARGET_LOCAL_POS = { 0.0f, 0.0f, 500.0f };

	// トップダウン用
	// y を大きくすると俯瞰になるので実機で調整すること
	static constexpr VECTOR TOP_CAMERA_LOCAL_POS = { 200.0f, -800.0f, 0.0f };
	static constexpr VECTOR TOP_TARGET_LOCAL_POS = { 0.0f, -1500.0f, 0.0f };

	// カメラのX回転の上限・下限
	static constexpr float LIMIT_X_UP_RAD = 40.0f * (DX_PI_F / 180.0f);
	static constexpr float LIMIT_X_DW_RAD = 10.0f * (DX_PI_F / 180.0f);

	// カメラモード
	enum class MODE
	{
		NONE,
		FIXED_POINT,
		FREE,
		FOLLOW,
	};

	// 衝突判定種別
	enum class COLLIDER_TYPE
	{
		SPHERE,
		MAX,
	};

	Camera(void);
	~Camera(void) override;

	// 更新
	void Update(void) override;

	// 描画前のカメラ設定
	void SetBeforeDraw(void);

	// デバッグ用描画
	void DrawDebug(void);

	// 解放
	void Release(void) override;

	// 座標の取得
	const VECTOR& GetPos(void) const { return transform_.pos; }

	// 角度の取得
	const VECTOR& GetAngles(void) const { return angles_; }
	const Quaternion& GetQuaRot(void) const { return transform_.quaRot; }

	// X回転を抜いたY軸のみのカメラ角度
	const Quaternion& GetQuaRotY(void) const { return rotY_; }

	// 注視点の取得
	const VECTOR& GetTargetPos(void) const { return targetPos_; }

	// カメラの前方方向
	VECTOR GetForward(void) const;

	// カメラモードの変更
	void ChangeMode(MODE mode);

	// 追従対象の設定
	void SetFollow(const Transform* follow) { followTransform_ = follow; }

	// 注視点を外部から明示的に指定する
	void SetTargetPos(const VECTOR& pos) { targetPos_ = pos; }

	// カメラ操作の有効無効（非アクティブ時に入力での回転/移動を止める）
	void SetControlEnabled(bool enabled) { controlEnabled_ = enabled; }
	bool IsControlEnabled(void) const { return controlEnabled_; }

	// マウスの基準位置（毎フレームここに戻す）
	void SetMouseCenter(int x, int y);

protected:

	void InitLoad(void) override;
	void InitTransform(void) override;
	void InitCollider(void) override;
	void InitAnimation(void) override;
	void InitPost(void) override;

private:

	// 衝突時の押し戻し試行回数
	static constexpr int CNT_TRY_COLLISION_CAMERA = 30;

	// 衝突時の押し戻し量
	static constexpr float COLLISION_BACK_DIS = 2.0f;

	// 衝突判定用球の半径
	static constexpr float COL_CAPSULE_SPHERE = 50.0f;

	// カメラの補間移動率
	static constexpr float LERP_RATE_MOVE = 0.5f;

	// マウスでの回転感度
	static constexpr float MOUSE_ROT_SENS = 0.005f;

	// マウスでの移動感度
	static constexpr float MOUSE_MOVE_SENS = 0.5f;

	// マウス移動とみなす最小ピクセル数（ノイズ除去）
	static constexpr int PIXEL_THRESHOLD = 1;

	// 追従時、追従対象の足元から注視点までの高さ
	static constexpr float FOLLOW_LOOK_AT_HEIGHT = 100.0f;

	// 追従時のピッチ角の範囲
	static constexpr float FOLLOW_PITCH_MIN_RAD = -30.0f * (DX_PI_F / 180.0f);
	static constexpr float FOLLOW_PITCH_MAX_RAD = 60.0f * (DX_PI_F / 180.0f);

	// 衝突判定の始点にする追従対象のフレーム番号
	static constexpr int FOLLOW_ROOT_FRAME = 1;

	// カメラの更新前位置
	VECTOR prePos_ = { 0.0f, 0.0f, 0.0f };

	// カメラが追従対象とするTransform
	const Transform* followTransform_ = nullptr;

	// カメラモード
	MODE mode_ = MODE::NONE;

	// カメラ角度(rad)
	VECTOR angles_ = { 0.0f, 0.0f, 0.0f };

	// カメラ角度(Y軸のみ)
	Quaternion rotY_;

	// 注視点
	VECTOR targetPos_ = { 0.0f, 0.0f, 0.0f };

	// 補間移動を行うか
	bool isCollision_ = false;

	// カメラ操作の有効フラグ（true=入力で回転/移動を受け付ける）
	bool controlEnabled_ = true;

	// マウスの基準位置
	int mouseCenterX_ = 0;
	int mouseCenterY_ = 0;

	// カメラを初期位置に戻す
	void SetDefault(void);

	// 追従対象との位置を同期する
	void SyncFollow(void);

	// angles_ から回転を計算する
	void UpdateRotation(void);

	// カメラ操作
	void ProcessRot(bool isLimit);
	void ProcessMove(void);

	// 回転入力
	void RotKeyboard(bool isLimit);
	void RotMouse(void);
	void RotGamePad(bool isLimit);

	// 移動入力（ローカル方向）
	VECTOR GetMoveDirKeyboardMouse(void) const;
	VECTOR GetMoveDirGamePad(void) const;

	// モード別更新
	void SetBeforeDrawFixedPoint(void);
	void SetBeforeDrawFree(void);
	void SetBeforeDrawFollow(void);

	// 衝突判定
	void Collision(void);
};