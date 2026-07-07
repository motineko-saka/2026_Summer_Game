#pragma once

#include "ObjectBase.h"

class Gaer
	: public ObjectBase
{
public:
	Gaer(SceneBase::WORLD world, VECTOR ansVec, OBJECT_TYPE type);
	virtual ~Gaer() override = default;

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

	void ObjectUpdateProcess(void)override;
private:
	float gearRot_;
	bool isRot_ = false;
};

