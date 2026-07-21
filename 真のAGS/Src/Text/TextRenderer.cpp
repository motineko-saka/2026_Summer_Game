#include "TextRenderer.h"

TextRenderer::TextRenderer()
	: fontSize_(24)
	, screenW_(640)
	, screenH_(480)
	, drawXChars_(0)
	, drawYLines_(0)
	, delayMs_(10)
{
}

TextRenderer::~TextRenderer()
{
}

void TextRenderer::Init(int fontSize, int screenW, int screenH)
{
	fontSize_ = fontSize;
	screenW_ = screenW;
	screenH_ = screenH;
	drawXChars_ = 0;
	drawYLines_ = 0;
	SetFontSize(fontSize_);
}

void TextRenderer::SetDelayMs(int ms)
{
	delayMs_ = ms;
}

void TextRenderer::ClearScreen()
{
	ClearDrawScreen();
	drawXChars_ = 0;
	drawYLines_ = 0;
}

void TextRenderer::ResetPosition()
{
	drawXChars_ = 0;
	drawYLines_ = 0;
}

bool TextRenderer::RenderStep(TextManager& tm)
{
	if (tm.IsAllTextConsumed()) return false;

	char moji = tm.CurrentByte();
	switch (moji)
	{
	case '@': // 改行
		// 改行処理および参照位置を1進める
		drawYLines_++;
		drawXChars_ = 0;
		tm.AdvanceBytes(1);

		// 画面外ならスクロール
		if (drawYLines_ * fontSize_ + fontSize_ > screenH_)
		{
			ScrollOneLine();
			drawYLines_--;
		}
		break;

	case 'B': // ボタン押し待ち
		WaitKey();
		tm.AdvanceBytes(1);
		break;

	case 'E': // 終了
		// 参照位置を1進めて終了を呼び出し元に伝える
		tm.AdvanceBytes(1);
		return false;

	case 'C': // クリア
		ClearScreen();
		tm.AdvanceBytes(1);
		break;

	case '\0':
		// ただし全部終わっていれば終了
		if (tm.IsAllTextConsumed()) return false;
		break;

	default:
	{
		// 2バイト文字を取得して描画
		char buf[3] = { 0,0,0 };
		tm.GetCurrentTwoByte(buf);

		DrawString(drawXChars_ * fontSize_, drawYLines_ * fontSize_, buf, GetColor(255, 255, 255));

		// 位置・参照を進める
		tm.AdvanceBytes(2);
		drawXChars_++;

		// 簡易な待ち
		WaitTimer(delayMs_);

		// 行幅超過で改行
		if (drawXChars_ * fontSize_ + fontSize_ > screenW_)
		{
			drawYLines_++;
			drawXChars_ = 0;

			if (drawYLines_ * fontSize_ + fontSize_ > screenH_)
			{
				ScrollOneLine();
				drawYLines_--;
			}
		}
		break;
	}
	}

	return !tm.IsAllTextConsumed();
}

void TextRenderer::ScrollOneLine()
{
	int TempGraph = MakeGraph(screenW_, screenH_);
	if (TempGraph == -1) return;

	GetDrawScreenGraph(0, 0, screenW_, screenH_, TempGraph);
	DrawGraph(0, -fontSize_, TempGraph, FALSE);
	DrawBox(0, screenH_ - fontSize_, screenW_, screenH_, 0, TRUE);
	DeleteGraph(TempGraph);
}