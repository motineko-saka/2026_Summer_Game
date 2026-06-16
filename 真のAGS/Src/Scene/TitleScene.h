#pragma once
#include "SceneBase.h"
#include "../Application.h"
class SkyDome;

class TitleScene : public SceneBase
{

public:


	// コンストラクタ
	TitleScene(void);

	// デストラクタ
	~TitleScene(void) override;

	// 初期化
	void Init(void) override;

	// ロード
	void Load(void) override;

	// ロード後の初期化
	void LoadEnd(void) override;

	// 更新
	void Update(void) override;

	// 描画
	void Draw(void) override;

	// 解放
	void Release(void) override;

private:

	static constexpr int IMG_TITLE_POS_Y = 260;
	static constexpr int IMG_TITLE_POS_X = 640;

	static constexpr int IMG_PUSH_SPACE_POS_Y = 410;
	static constexpr int IMG_PUSH_SPACE_POS_X = 640;

	int imgTitle_;
	int imgPushSpace_;
	int imgHondana_;

	int movTitle_;

};
