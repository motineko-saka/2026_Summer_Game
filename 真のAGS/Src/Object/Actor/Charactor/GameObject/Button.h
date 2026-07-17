#pragma once
#include "ObjectBase.h"

class Player;

class Button : public ObjectBase
{
public:
	Button(SceneBase::WORLD world, VECTOR ansVec, OBJECT_TYPE type);

	bool isPushButtom(void) const { return isButtomPushed_; }

private:
	// リソースロード
	void InitLoad(void)override;

	void ObjectUpdateProcess(void) override;

	bool isButtomPushed_ = false;
};

