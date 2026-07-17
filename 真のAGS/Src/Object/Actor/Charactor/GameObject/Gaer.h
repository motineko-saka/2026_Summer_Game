#pragma once

#include "ObjectBase.h"

class Gaer
	: public ObjectBase
{
public:
	Gaer(SceneBase::WORLD world, VECTOR ansVec, OBJECT_TYPE type, ObjectBase& object);
	virtual ~Gaer() override = default;

	bool isPushButtom(void) const { return isRot_; }
private:

	float gearRot_;
	bool isRot_ = false;

	// リソースロード
	void InitLoad(void)override;

	// 初期化後の個別処理
	void InitPost(void)override;

	void ObjectUpdateProcess(void)override;

	ObjectBase& object_;
};

