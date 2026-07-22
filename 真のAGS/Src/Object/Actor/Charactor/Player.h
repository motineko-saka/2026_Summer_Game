#pragma once
#include "CharactorBase.h"

class Camera;

class Player : public CharactorBase
{

public:

	enum class ANIM_TYPE
	{
		IDLE,
		RUN,
		FAST_RUN,
		JUMP,
		//MAX,
	};

	// プレイヤー番号
	enum class PLAYER_NO
	{
		PLAYER1,
		PLAYER2,
	};

	Player(void);
	Player(PLAYER_NO playerNo, Camera& camera, bool isGameScene = false); // プレイヤー番号付きコンストラクタ
	virtual ~Player(void);

	// 描画
	void Draw(void)override;

	// 解放
	void Release(void)override;

	// プレイヤー番号の取得
	PLAYER_NO GetPlayerNo(void) const;

	VECTOR GetMovePow(void) { return movePow_; }

	// 選択/非選択の切替
	void SetActive(bool active);
	bool IsActive() const { return isActive_; }

	void HitColliderErase(int index) { 
		hitColliders_.erase(hitColliders_.begin() + index); }

protected:

	// 初期ロード
	void InitLoad(void) override;

	// 初期Transform
	void InitTransform(void) override;

	// 初期コライダ
	void InitCollider(void) override;

	// 初期アニメーション
	void InitAnimation(void) override;

	// 初期後処理
	void InitPost(void) override;

	// 更新処理
	void UpdateProcess(void) override;

	void UpdateProcessPost(void) override;

private:

	static constexpr VECTOR PLAYER_ONE__DEFAULT_POS = { -1000.0f, 0.0f, 1000.0f };
	static constexpr VECTOR PLAYER_TWO__DEFAULT_POS = { 1000.0f, 0.0f, 1000.0f };

	static constexpr VECTOR TUTORIAL_PLAYER_ONE_DEFAULT_POS = { -1000.0f, 0.0f, 0.0f };
	static constexpr VECTOR TUTORIAL_PLAYER_TWO_DEFAULT_POS = { 1000.0f, 0.0f, 0.0f };

	static constexpr VECTOR PLAYER_DEFAULT_SCALE = { 0.5f, 0.5f, 0.5f };

	static constexpr VECTOR PLAYER_DEFAULT_ROT_LOCAL = { 0.0f,180.0f * DX_PI_F / 180.0f,0.0f };

	// 移動
	static constexpr float SPEED_MOVE = 5.0f;

	// ダッシュ
	static constexpr float SPEED_DASH = 10.0f;

	// コライダ（線分）開始位置（ローカル）
	static constexpr VECTOR COL_LINE_START_LOCAL_POS = { 0.0f, 80.0f, 0.0f };

	// コライダ（線分）終了位置（ローカル）
	static constexpr VECTOR COL_LINE_END_LOCAL_POS = { 0.0f, -10.0f, 0.0f };

	// コライダ（線分）開始位置（ジャンプ時）
	static constexpr VECTOR COL_LINE_JUMP_START_LOCAL_POS = { 0.0f, 130.0f, 0.0f };

	// コライダ（線分）終了位置（ジャンプ時）
	static constexpr VECTOR COL_LINE_JUMP_END_LOCAL_POS = { 0.0f, 50.0f, 0.0f };

	// ジャンプ力初期
	static constexpr float POW_JUMP_INIT = 3500.0f;

	// ジャンプ力維持
	static constexpr float POW_JUMP_KEEP = 400.0f;

	// ジャンプ入力受付時間
	static constexpr float TIME_JUMP_INPUT = 0.5f;

	// カプセルコライダ上位置（ローカル）
	static constexpr VECTOR COL_CAPSULE_TOP_LOCAL_POS = { 0.0f, 110.0f, 0.0f };

	// カプセルコライダ下位置（ローカル）
	static constexpr VECTOR COL_CAPSULE_DOWN_LOCAL_POS = { 0.0f, 30.0f, 0.0f };

	// カプセル半径
	static constexpr float COL_CAPSULE_RADIUS = 20.0f;

	// カプセル上位置（ジャンプ時）
	static constexpr VECTOR COL_CAPSULE_TOP_JUMP_LOCAL_POS =
	{ 0.0f, 160.0f, 0.0f };

	// カプセル下位置（ジャンプ時）
	static constexpr VECTOR COL_CAPSULE_DOWN_JUMP_LOCAL_POS =
	{ 0.0f, 80.0f, 0.0f };

	// 目の前に浮かせるオフセット
	static constexpr float PICKUP_FRONT_DIST = 60.0f;
	static constexpr float PICKUP_UP_DIST = 30.0f;

	// プレイヤー番号
	PLAYER_NO playerNo_;

	Camera* camera_;

	bool isGameScene_;

	// オブジェクトとの衝突処理
	void CollisionObject(void);

	// 移動、ジャンプ処理
	void ProcessMove(void);
	void ProcessJump(void);
	void ProcessAnimPos(void);
	void ProcessAnimCapsule(void);

	// 衝突関連
	void CollisionReserve(void) override;

	// デバッグ描画
	void DrawDebug(void);

	// 掴んでいるコライダ（NULL=何も掴んでいない）
	ColliderBase* heldCollider_{ nullptr };
	// 掴む前にコライダが追従していたTransform（戻すために保持）
	const Transform* heldPrevFollow_{ nullptr };

	// 入力で掴む/放す処理
	void ProcessPickup(void);

	// 実際に掴む（followを切り替える）
	void PickupCollider(ColliderBase* collider);

	// 放す（followを元に戻す）
	void DropHeldObject(void);

	// 掴んでいるか
	bool IsHolding() const { return heldCollider_ != nullptr; }

	// 選択状態（true=操作可能、false=操作停止）
	bool isActive_{ true };

	// 歩行ループSE 再生フラグ
	bool isWalkSePlaying_{ false };
};