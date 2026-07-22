#pragma once

#include <DxLib.h>
#include "TextManager.h"

class TextRenderer
{
public:
	TextRenderer();
	~TextRenderer();

	// 初期化（フォントサイズ、画面サイズを指定）
	void Init(int fontSize, int screenW = 640, int screenH = 480);

	// 1 ステップ分の描画処理を行う。
	// 戻り値: true = 継続、 false = 描画終了（'E' を受けた、または TextManager が終端）
	bool RenderStep(TextManager& tm);

	// 画面クリア・位置リセット
	void ClearScreen();
	void ResetPosition();

	// 文字送り速度（ミリ秒）
	void SetDelayMs(int ms);

	// 描画オフセット（枠の左上）
	void SetOrigin(int x, int y);

private:
	// 行折り返し・スクロール処理（内部 Kaigyou）
	void NewLineIfNeeded();
	void ScrollOneLine();

	// レイアウト状態
	int fontSize_;
	int screenW_;
	int screenH_;
	int drawXChars_;
	int drawYLines_;
	int delayMs_;

	// 描画起点
	int originX_;
	int originY_;

	// 枠内の描画を保持するオフスクリーン
	int textGraph_;

	// 時間管理（非ブロッキングで1文字ずつ進めるため）
	unsigned int lastAdvanceTimeMs_;

	// 'B' 命令のための待機フラグ（入力を待つ）
	bool waitingForInput_;
};