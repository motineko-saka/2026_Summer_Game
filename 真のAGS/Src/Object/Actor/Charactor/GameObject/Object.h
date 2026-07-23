#pragma once

#include "ObjectBase.h"

class Object :
    public ObjectBase
{
public:
	Object(SceneBase::WORLD world, VECTOR ansVec, OBJECT_TYPE type);

private:
	// リソースロード
	void InitLoad(void)override;
	void InitObjTrans(void)override;

	// 初期化後の個別処理
	void InitPost(void)override;

	void ObjectUpdateProcess(void)override;
};

