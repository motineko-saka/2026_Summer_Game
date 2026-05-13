#pragma once
#include "StageBase.h"
class StageManager
{
public:
	// コンストラクタ
	StageManager();

	// デストラクタ
	~StageManager(void);

	// 初期化
	void Init(void);

	// 更新
	void Update(void);

	// 描画
	void Draw(void);

	// 解放
	void Release(void);
private:
	StageBase* stage_;
};

