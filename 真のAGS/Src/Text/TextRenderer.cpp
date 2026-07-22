#include "TextRenderer.h"
#include "../Manager/InputManager.h" // 入力チェック用（非ブロッキング処理）

TextRenderer::TextRenderer()
	: fontSize_(24)
	, screenW_(640)
	, screenH_(480)
	, drawXChars_(0)
	, drawYLines_(0)
	, delayMs_(10)
	, originX_(0)
	, originY_(0)
	, textGraph_(-1)
	, lastAdvanceTimeMs_(0)
	, waitingForInput_(false)
{
}

TextRenderer::~TextRenderer()
{
	if (textGraph_ != -1)
	{
		DeleteGraph(textGraph_);
		textGraph_ = -1;
	}
}

void TextRenderer::Init(int fontSize, int screenW, int screenH)
{
	fontSize_ = fontSize;
	screenW_ = screenW;
	screenH_ = screenH;
	drawXChars_ = 0;
	drawYLines_ = 0;
	originX_ = 0;
	originY_ = 0;
	lastAdvanceTimeMs_ = 0;
	waitingForInput_ = false;
	SetFontSize(fontSize_);

	// 既存のグラフがあれば削除して再作成
	if (textGraph_ != -1)
	{
		DeleteGraph(textGraph_);
		textGraph_ = -1;
	}
	textGraph_ = MakeScreen(screenW_, screenH_, true); // アルファ対応オフスクリーン
	if (textGraph_ != -1)
	{
		int prev = GetDrawScreen();
		SetDrawScreen(textGraph_);
		ClearDrawScreen();
		SetDrawScreen(prev);
	}
}

void TextRenderer::SetDelayMs(int ms)
{
	delayMs_ = ms;
}

void TextRenderer::SetOrigin(int x, int y)
{
	originX_ = x;
	originY_ = y;
}

void TextRenderer::ClearScreen()
{
	// オフスクリーンをクリアして位置リセット
	if (textGraph_ != -1)
	{
		int prev = GetDrawScreen();
		SetDrawScreen(textGraph_);
		ClearDrawScreen();
		SetDrawScreen(prev);
	}
	drawXChars_ = 0;
	drawYLines_ = 0;
	lastAdvanceTimeMs_ = 0;
	waitingForInput_ = false;

	// 直ちにバックバッファへ反映
	if (textGraph_ != -1) DrawGraph(originX_, originY_, textGraph_, TRUE);
}

void TextRenderer::ResetPosition()
{
	drawXChars_ = 0;
	drawYLines_ = 0;
	lastAdvanceTimeMs_ = 0;
	waitingForInput_ = false;
}

bool TextRenderer::RenderStep(TextManager& tm)
{
	// 毎フレームオフスクリーンをブリットして表示維持
	if (textGraph_ != -1)
	{
		DrawGraph(originX_, originY_, textGraph_, TRUE);
	}

	// 全部終わっていたら何もしない（描画は既にブリット済み）
	if (tm.IsAllTextConsumed()) return false;

	unsigned int now = GetNowCount(); // ミリ秒

	// まず「ボタン待ち」状態の処理（非ブロッキング）
	if (waitingForInput_)
	{
		// InputManager のトリガーをいくつか監視（スペース・Z・Enter・パッド右 等）
		auto input = InputManager::GetInstance();
		bool pressed = false;
		if (input)
		{
			if (input->IsTrgDown(KEY_INPUT_SPACE) || input->IsTrgDown(KEY_INPUT_Z) || input->IsTrgDown(KEY_INPUT_RETURN) ||
				input->IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT))
			{
				pressed = true;
			}
		}
		if (pressed)
		{
			// 後続の処理を進めるために advance（'B' 自体は既に次に進むべきか元コードだと AdvanceBytes(1) を行っていた）
			tm.AdvanceBytes(1);
			waitingForInput_ = false;
		}
		else
		{
			// 入力が来るまで待機（毎フレーム表示は維持）
			return true;
		}
	}

	// 現在の文字を確認
	char moji = tm.CurrentByte();
	switch (moji)
	{
	case '@': // 改行（即時処理）
		drawYLines_++;
		drawXChars_ = 0;
		tm.AdvanceBytes(1);
		if (drawYLines_ * fontSize_ + fontSize_ > screenH_)
		{
			ScrollOneLine();
			drawYLines_--;
		}
		break;

	case 'B': // ボタン待ち（非ブロッキングにする）
		// フラグを立てて、このフレームはこれ以上進めない
		waitingForInput_ = true;
		// 既にブリット済みなのでただ待つ
		return true;

	case 'E': // 終了（元の仕様通り）
		tm.AdvanceBytes(1);
		return false;

	case 'C': // クリア（即時）
		ClearScreen();
		tm.AdvanceBytes(1);
		break;

	case '\0':
		if (tm.IsAllTextConsumed()) return false;
		break;

	default:
	{
		// 文字送り制御（非ブロッキング）
		// 最後に文字を進めた時間から delayMs_ 経っていなければ何もしない
		if (lastAdvanceTimeMs_ != 0 && now - lastAdvanceTimeMs_ < static_cast<unsigned int>(delayMs_))
		{
			// 表示はオフスクリーンを毎フレームブリットしているので何もしない
			return true;
		}

		// 進めて描画する
		char buf[3] = { 0,0,0 };
		tm.GetCurrentTwoByte(buf);

		if (textGraph_ != -1)
		{
			int prev = GetDrawScreen();
			SetDrawScreen(textGraph_);
			DrawString(drawXChars_ * fontSize_, drawYLines_ * fontSize_, buf, GetColor(255, 255, 255));
			SetDrawScreen(prev);
		}
		else
		{
			// フォールバック（直接描画）
			DrawString(originX_ + drawXChars_ * fontSize_, originY_ + drawYLines_ * fontSize_, buf, GetColor(255, 255, 255));
		}

		// 位置・参照を進める
		tm.AdvanceBytes(2);
		drawXChars_++;

		// 進めた時間を記録（これで WaitTimer の代替)
		lastAdvanceTimeMs_ = now;

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

	// 既に冒頭でブリットしているが、念のため最後にもブリット（不要なら省けます）
	if (textGraph_ != -1)
	{
		DrawGraph(originX_, originY_, textGraph_, TRUE);
	}

	return !tm.IsAllTextConsumed();
}

void TextRenderer::ScrollOneLine()
{
	if (textGraph_ == -1)
	{
		// フォールバック
		int TempGraph = MakeGraph(screenW_, screenH_);
		if (TempGraph == -1) return;

		GetDrawScreenGraph(0, 0, screenW_, screenH_, TempGraph);
		DrawGraph(0, -fontSize_, TempGraph, FALSE);
		DrawBox(0, screenH_ - fontSize_, screenW_, screenH_, 0, TRUE);
		DeleteGraph(TempGraph);
		return;
	}

	int TempGraph = MakeGraph(screenW_, screenH_);
	if (TempGraph == -1) return;

	int prev = GetDrawScreen();
	SetDrawScreen(textGraph_);
	GetDrawScreenGraph(0, 0, screenW_, screenH_, TempGraph);
	ClearDrawScreen();
	DrawGraph(0, -fontSize_, TempGraph, FALSE);
	DrawBox(0, screenH_ - fontSize_, screenW_, screenH_, 0, TRUE);
	SetDrawScreen(prev);

	DeleteGraph(TempGraph);
}