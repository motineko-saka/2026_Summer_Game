#pragma once
#include "SceneBase.h"
class GameClearScene :
    public SceneBase
{
public:
	// コンストラクタ
	GameClearScene(void);

	// デストラクタ
	~GameClearScene(void) override;

	// 初期化
	void Init(void) override;

	// 読み込み
	void Load(void) override;

	// 読み込み後の初期化
	void LoadEnd(void) override;

	// 更新
	void Update(void) override;

	// 描画
	void Draw(void) override;

	// 解放
	void Release(void) override;

private:
	int bgImage_;
};

