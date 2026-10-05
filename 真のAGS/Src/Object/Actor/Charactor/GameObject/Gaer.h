#pragma once

#include "ObjectBase.h"

class Gaer
	: public ObjectBase
{
public:
	Gaer(SceneBase::WORLD world, VECTOR ansVec, OBJECT_TYPE type);
	virtual ~Gaer() override = default;

	bool IsRot(void) const { return isRot_; }

	void AddObject(ObjectBase* object);
private:
	static constexpr float GEAR_ROT_SPEED = 5.0f;

	float gearRot_;
	bool isRot_ = false;

	// リソースロード
	void InitLoad(void)override;

	// 初期化後の個別処理
	void InitPost(void)override;

	void ObjectUpdateProcess(void)override;

	ObjectBase* object_;
};

