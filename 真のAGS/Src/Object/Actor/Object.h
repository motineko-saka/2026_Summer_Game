#pragma once
#include <DxLib.h>
#include "ActorBase.h"
#include "../../Scene/GameScene.h"

class Object : public ActorBase
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

	void Update(void)override;
	void Draw(void)override;
	void Release(void)override;

	void SetWorld(GameScene::WORLD world) { world_ = world; }

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
private:
	// 衝突判定用線分開始
	static constexpr VECTOR COL_LINE_START_LOCAL_POS = { 0.0f, 80.0f, 0.0f };

	// 衝突判定用線分終了
	static constexpr VECTOR COL_LINE_END_LOCAL_POS = { 0.0f, -10.0f, 0.0f };

	GameScene::WORLD world_;
	GameScene::WORLD viewWorld_;
};
