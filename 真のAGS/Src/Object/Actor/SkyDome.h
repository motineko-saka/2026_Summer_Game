#pragma once
#include "../../Application.h"
#include "ActorBase.h"

class SkyDome : public ActorBase
{
public:

	//状態
	enum class STATE
	{
		NONE,
		STAY,
		FOLLOW,
	};

	SkyDome(const Transform& transform);
	virtual ~SkyDome(void);

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

	// 追従対象のTransform
	const Transform& followTransform_;

	// 状態
	STATE state_;

	static constexpr VECTOR DEFAULT_SKYDOME_SCALE = { 100.0f,100.0f,100.0f };
	static constexpr VECTOR DEFAULT_SKYDOME_POS = { 0.0f,0.0f,0.0f };


	static constexpr VECTOR DEFAULT_ROT_LOCAL = { 0.0f,180.0f,0.0f };

	// 状態遷移
	void ChangeState(STATE state);

	void ChangeStateNone(void);
	void ChangeStateStay(void);
	void ChangeStateFollow(void);

	void UpdateNone(void);
	void UpdateStay(void);
	void UpdateFollow(void);

};