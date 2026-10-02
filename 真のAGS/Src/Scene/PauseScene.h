#pragma once
#include "SceneBase.h"

// ベースを継承
class PauseScene : public SceneBase
{
public:

	enum MENU
	{
		TITLE = 0,
		EXIT,
		BACK
	};

	// コンストラクタ
	PauseScene(void);

	// デストラクタ
	~PauseScene(void) override;

public:
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

	int selectMenu_;
};
