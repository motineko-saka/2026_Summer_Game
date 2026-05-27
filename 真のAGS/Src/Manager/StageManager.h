#pragma once
#include "../Object/Actor/Stage/StageBase.h"
#include <vector>

class StageManager
{
public:
	// コンストラクタ
	StageManager();

	// デストラクタ
	~StageManager(void);

	// 初期化
	void InitStage(void);

	// 更新
	void Update(void);

	// 描画
	void Draw(void);

	// 解放
	void Release(void);

	const std::vector<StageBase*>& GetStage() const { return stages_; }
private:
	constexpr static int STAGE_NUM = 1;

	std::vector<StageBase*> stages_;
};

