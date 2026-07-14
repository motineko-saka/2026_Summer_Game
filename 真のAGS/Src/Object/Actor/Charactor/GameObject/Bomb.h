#pragma once
#include "ObjectBase.h"
class Bomb :
    public ObjectBase

{
public:
	Bomb(SceneBase::WORLD world, VECTOR ansVec, OBJECT_TYPE type);

	bool isPushButtom(void) const { return isButtomPushed_; }

private:
	// リソースロード
	void InitLoad(void)override;

	// 衝突判定の初期化
	void InitObjCol(void)override;

	void ObjectUpdateProcess(void) override;

	bool isButtomPushed_ = false;
};