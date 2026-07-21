#pragma once
#include "ObjectBase.h"
class Axe :
    public ObjectBase

{
public:
	Axe(SceneBase::WORLD world, VECTOR ansVec, OBJECT_TYPE type);

	bool isPushButtom(void) const { return isButtomPushed_; }

private:
	// リソースロード
	void InitLoad(void)override;

	// 衝突判定の初期化
	void InitObjCol(void)override;

	void ObjectUpdateProcess(void) override;

	void SetFlame(const Transform* follow) override{ handFrame_ = MV1SearchFrame(follow->modelId, "mixamorig:LeftHand"); };

	bool isButtomPushed_ = false;
};