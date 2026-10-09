#pragma once
#include <DxLib.h>

class LightPillar
{
public:

	// コンストラクタ
	LightPillar();

	// デスクトラクタ
	~LightPillar();

	// 初期化
	void Init(VECTOR pos);

	// 更新
	void Update();

	// 描画
	void Draw();

private:

	VECTOR pos_;

	float scale_;
	int alpha_;
	int timer_;

	bool active_;
};