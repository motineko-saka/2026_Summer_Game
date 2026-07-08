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

	// 初期化後の個別処理
	void InitPost(void)override;

	void ObjectUpdateProcess(void)override;
private:
	float gearRot_;
	bool isRot_ = false;
};

