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

	// プレイヤー番号の列挙型
	enum class PLAYER_NO
	{
		PLAYER1,
		PLAYER2,
	};

	Player(void);
	Player(PLAYER_NO playerNo, Camera& camera); // プレイヤー番号指定のコンストラクタ
	virtual ~Player(void);

	// 描画
	void Draw(void)override;

	// 解放
	void Release(void)override;

	// プレイヤーの複製
	Player* Clone(PLAYER_NO playerNo) const;

	// プレイヤー番号の取得
	PLAYER_NO GetPlayerNo(void) const;

	// 初期位置の設定
	void SetInitialPosition(const VECTOR& pos);

	VECTOR GetMovePow(void) { return movePow_; }

protected:

	// リソースロード
	void InitLoad(void) override;

	// 大きさ、回転、座標の初期化
	void InitTransform(void) override;

	// 衝突判定の初期化
	void InitCollider(void) override;

	// アニメーションの初期化
	void InitAnimation(void) override;

	// 初期化後の後処理
	void InitPost(void) override;

	// 更新系
	void UpdateProcess(void) override;

	void UpdateProcessPost(void) override;


private:

	static constexpr VECTOR PLAYER_ONE__DEFAULT_POS = { -100.0f, 0.0f, 0.0f };
	static constexpr VECTOR PLAYER_TWO__DEFAULT_POS = { 100.0f, 0.0f, 0.0f };

	static constexpr VECTOR PLAYER_DEFAULT_SCALE = { 1.0f,1.0f,1.0f };

	static constexpr VECTOR PLAYER_DEFAULT_ROT_LOCAL = { 0.0f,180.0f * DX_PI_F / 180.0f,0.0f };

	// 移動速度(通常)
	static constexpr float SPEED_MOVE = 5.0f;

	// 移動速度(ダッシュ)
	static constexpr float SPEED_DASH = 10.0f;

	// 衝突判定用ライン開始
	static constexpr VECTOR COL_LINE_START_LOCAL_POS = { 0.0f, 80.0f, 0.0f };

	// 衝突判定用ライン終了
	static constexpr VECTOR COL_LINE_END_LOCAL_POS = { 0.0f, -10.0f, 0.0f };

	// 衝突判定用ライン開始(ジャンプ時)
	static constexpr VECTOR COL_LINE_JUMP_START_LOCAL_POS = { 0.0f, 130.0f, 0.0f };

	// 衝突判定用ライン終了(ジャンプ時)
	static constexpr VECTOR COL_LINE_JUMP_END_LOCAL_POS = { 0.0f, 50.0f, 0.0f };

	// ジャンプ力
	static constexpr float POW_JUMP_INIT = 3500.0f;

	// 溜めジャンプ力
	static constexpr float POW_JUMP_KEEP = 400.0f;

	// ジャンプ力付与時間
	static constexpr float TIME_JUMP_INPUT = 0.5f;

	// 衝突判定用カプセル上部座標
	static constexpr VECTOR COL_CAPSULE_TOP_LOCAL_POS = { 0.0f, 110.0f, 0.0f };

	// 衝突判定用カプセル下部座標
	static constexpr VECTOR COL_CAPSULE_DOWN_LOCAL_POS = { 0.0f, 30.0f, 0.0f };

	// 衝突判定用カプセル球の半径
	static constexpr float COL_CAPSULE_RADIUS = 20.0f;

	// 衝突判定用カプセル上部座標(ジャンプ時)
	static constexpr VECTOR COL_CAPSULE_TOP_JUMP_LOCAL_POS =
	{ 0.0f, 160.0f, 0.0f };

	// 衝突判定用カプセル下部座標(ジャンプ時)
	static constexpr VECTOR COL_CAPSULE_DOWN_JUMP_LOCAL_POS =
	{ 0.0f, 80.0f, 0.0f };

	// プレイヤー番号
	PLAYER_NO playerNo_;

	Camera* camera_;

	// 処理
	void ProcessMove(void);
	void ProcessJump(void);
	void ProcessAnimPos(void);
	void ProcessAnimCapsule(void);

	// 衝突判定
	void CollisionReserve(void) override;

	// デバッグ描画
	void DrawDebug(void);
};