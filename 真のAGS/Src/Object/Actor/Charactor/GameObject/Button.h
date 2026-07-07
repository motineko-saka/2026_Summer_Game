#pragma once
#include "ObjectBase.h"

class Button : public ObjectBase
{
public:
	Button(SceneBase::WORLD world, VECTOR ansVec, OBJECT_TYPE type);
	virtual ~Button() override = default;

private:
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

	bool isPushButtom(void) const { return isButtomPushed_; }

	void ObjectUpdateProcess(void) override;

	bool isButtomPushed_ = false;
};

