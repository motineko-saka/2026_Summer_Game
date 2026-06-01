#pragma once
#include "ActorBase.h"
class Wall :
    public ActorBase
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

    Wall(void);
    ~Wall(void) override;

	void Update(void)override;
	void Draw(void)override;
	void Release(void)override;
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
	static constexpr VECTOR STAGE_DEFAULT_POS = { 0.0f, 0.0f, 0.0f };

	static constexpr VECTOR STAGE_DEFAULT_SCALE = { 1000.0f,1000.0f,1000.0f };
};