#pragma once
#include "ObjectBase.h"

class Player;

class Button : public ObjectBase
{
public:
	Button(SceneBase::WORLD world, VECTOR ansVec, OBJECT_TYPE type, Player& player);

	bool isPushButtom(void) const { return isButtomPushed_; }

private:
	std::vector<Player::PlayerS>Player& player_;

	// リソースロード
	void InitLoad(void)override;

	void ObjectUpdateProcess(void) override;

	bool isButtomPushed_ = false;
};

