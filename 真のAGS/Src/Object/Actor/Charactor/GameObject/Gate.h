#pragma once

#include "ObjectBase.h"

class Gate :
	public ObjectBase
{
public:
	Gate(SceneBase::WORLD world, VECTOR ansVec, OBJECT_TYPE type);

private:
	// オブジェクト種類
	OBJECT_TYPE type_{ OBJECT_TYPE::DEFAULT };

	// リソースロード
	void InitLoad(void)override;
	void InitObjTrans(void)override;

	// 初期化後の個別処理
	void InitPost(void)override;

	void ObjectUpdateProcess(void)override;
};

