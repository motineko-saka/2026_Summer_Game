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
	// UI配置用の定数
	static constexpr int IMG_TITLE_POS_Y = 125;
	static constexpr int IMG_TITLE_POS_X = 960;

	static constexpr int IMG_PUSH_SPACE_POS_Y = 690;
	static constexpr int IMG_PUSH_SPACE_POS_X = 640;

	// 縦書きテキスト用の定数
	static constexpr int VERTICAL_TEXT_X = 100;
	static constexpr int VERTICAL_TEXT_Y = 200;
	static constexpr int TEXT_SCREEN_W = 400;
	static constexpr int TEXT_SCREEN_H = 40;

	// リソースハンドル
	int imgTitle_ = -1;
	int imgPushSpace_ = -1;
	int imgHondana_ = -1;
	int movTitle_ = -1;

	int imgUIStart_ = -1;
	int imgUITutorial_ = -1;
	int imgUIExit_ = -1;

	// 選択インデックス (0: Start, 1: Tutorial, 2: Exit)
	int uiSelect_ = 0;

	// 前フレームのマウス位置
	float prevMouseX_;
	float prevMouseY_;

	// 前フレームのスティックX
	float prevStickX_ = 0.0f;
};