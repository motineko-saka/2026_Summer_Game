#pragma once
#include <DxLib.h>
#include "../Charactor/CharactorBase.h"
#include "../../../Scene/GameScene.h"

class Object : public CharactorBase
{
public:
	// 衝突判定種別
	enum class COLLIDER_TYPE
	{
		LINE,
		MODEL,
		CAPSULE,
		MAX,
		VIEW_RANGE,
	};

	Object(GameScene::WORLD world);
	virtual ~Object(void);

	//void Update(void)override;
	void Draw(void)override;
	void Release(void)override;

	GameScene::WORLD GetViewWorld(void) { return viewWorld_; }

	GameScene::WORLD GetWorld(void) { return world_; }

	void SetViewWorld(GameScene::WORLD world) { viewWorld_ = world; }

	Transform& GetTransform(void) { return transform_; }
	VECTOR& GetPos(void) { return transform_.pos; }

	// プレイヤーから押される処理
	void Push(const VECTOR& direction, float speed);

	// 衝突処理
	void CollisionCapsule() override;

protected:

	// リソースロード
	void InitLoad(void)override;

	// 大きさ、回転、座標の初期化
	void InitTransform(void) override;

	// 衝突判定の初期化
	void InitCollider(void) override;

	// アニメーションの初期化
	void InitAnimation(void) override;

	// 初期化後の個別処理
	void InitPost(void)override;

	void UpdateProcess(void) override;

	void UpdateProcessPost(void) override;

private:
	// 衝突判定用ライン開始
	static constexpr VECTOR COL_LINE_START_LOCAL_POS = { 0.0f, 80.0f, 0.0f };

	// 衝突判定用ライン終了
	static constexpr VECTOR COL_LINE_END_LOCAL_POS = { 0.0f, -10.0f, 0.0f };

	// 衝突判定用カプセル上部座標
	static constexpr VECTOR COL_CAPSULE_TOP_LOCAL_POS = { 0.0f, 60.0f, 0.0f };

	// 衝突判定用カプセル下部座標
	static constexpr VECTOR COL_CAPSULE_DOWN_LOCAL_POS = { 0.0f, 0.0f, 0.0f };

	// 衝突判定用カプセルの半径
	static constexpr float COL_CAPSULE_RADIUS = 30.0f;

	// オブジェクトの押される抵抗力（0.0～1.0）
	static constexpr float PUSH_RESISTANCE = 0.1f;

	GameScene::WORLD world_;		// 
	GameScene::WORLD viewWorld_;	// 今写っている世界

	// 押されて移動する量
	VECTOR pushPow_;
};