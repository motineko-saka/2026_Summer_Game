#pragma once
#include <vector>
#include "../Object/Actor/Stage/StageBase.h"
#include "../Manager/SceneManager.h"

class StageManager
{
public:
	explicit StageManager(SceneManager::SCENE scene);
	~StageManager(void);

	// ステージを持つポインタを二重に解放しないよう、コピー禁止
	StageManager(const StageManager&) = delete;
	StageManager& operator=(const StageManager&) = delete;

	// 初期化
	void InitStage(void);

	// 更新
	void Update(void);

	// 描画
	void Draw(void);
	void DrawDebug(void);

	// 解放
	void Release(void);

	const std::vector<StageBase*>& GetStage(void) const { return stages_; }

private:
	static constexpr int STAGE_NUM = 1;

	// 種類に応じたステージを生成
	StageBase* CreateStage(StageBase::STAGE_TYPE type) const;

	// シーンに応じたステージモデルを読み込む
	int LoadStageModel(void) const;

	std::vector<StageBase*> stages_;

	const SceneManager::SCENE scene_;
};