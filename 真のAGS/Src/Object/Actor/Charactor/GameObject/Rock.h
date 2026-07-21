#pragma once
#include "ObjectBase.h"

class Rock : public ObjectBase
{
public:
	Rock(SceneBase::WORLD world, VECTOR ansVec, OBJECT_TYPE type);

	bool GetIsRockExist(void) { return isRockExist_; }

private:

	void Draw() override;
	// リソースロード
	void InitLoad(void)override;

	// 衝突判定の初期化
	void InitObjCol(void)override;

	// 初期化後の個別処理
	void InitPost(void)override;

	void ObjectUpdateProcess(void)override;

	bool isRockExist_ = true;
};